/**
 * peer — demonstração bidirecional do packetizer sobre UDP.
 *
 * Uso:
 *   peer --local-port <N> --remote-port <M> [--remote-host <IP>]
 *
 * Execute duas instâncias na mesma máquina:
 *   Terminal A:  ./peer --local-port 5000 --remote-port 5001
 *   Terminal B:  ./peer --local-port 5001 --remote-port 5000
 *
 * Comandos (digitados no stdin):
 *   msg  <texto>         Envia uma mensagem de texto
 *   file <caminho>       Envia um arquivo
 *   quit                 Encerra
 *
 * O app é full-duplex: uma thread em segundo plano injeta datagramas UDP
 * recebidos no packetizer e chama pkt_tick(); a thread principal lê o stdin e
 * chama pkt_send(). Um pthread_mutex protege a instância compartilhada.
 */

#define _POSIX_C_SOURCE 200809L

#include "packetizer.h"
#include "udp_transport.h"

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

/* -------------------------------------------------------------------------- */
/* Utilitários                                                                 */
/* -------------------------------------------------------------------------- */

static uint32_t now_ms(void)
{
    /* --- Lê o relógio monotônico e converte para milissegundos --- */
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)((uint64_t)ts.tv_sec * 1000u +
        (uint64_t)ts.tv_nsec / 1000000u);
}

/* -------------------------------------------------------------------------- */
/* Estado global compartilhado entre as threads                                */
/* -------------------------------------------------------------------------- */

typedef struct {
    packetizer_t* pkt;
    udp_transport_t* udp;
    pthread_mutex_t  lock;
    volatile int     running;
} app_t;

/* -------------------------------------------------------------------------- */
/* Callbacks do packetizer (chamados com mutex pela thread de rede)           */
/* -------------------------------------------------------------------------- */

static int transport_write(const uint8_t* data, size_t len, void* ctx)
{
    /* --- Callback do packetizer: repassa os bytes codificados ao socket UDP --- */
    app_t* app = (app_t*)ctx;
    return udp_send(app->udp, data, len);
}

static void on_message(const uint8_t* data, size_t len, void* ctx)
{
    (void)ctx;

    /* --- Verifica se o payload começa com o prefixo de transferência de arquivo --- */
    const char* prefix = "FILE:";
    size_t plen = strlen(prefix);

    if (len > plen && memcmp(data, prefix, plen) == 0) {
        /* --- Formato esperado: "FILE:<nome>\n<conteúdo binário>" --- */
        const char* name_start = (const char*)data + plen;
        const char* nl = memchr(name_start, '\n', len - plen);
        if (nl) {
            /* --- Extrai o nome do arquivo e localiza o início do conteúdo --- */
            size_t name_len = (size_t)(nl - name_start);
            char filename[256];
            snprintf(filename, sizeof(filename), "received_%.*s",
                (int)name_len, name_start);

            const uint8_t* content = (const uint8_t*)(nl + 1);
            size_t content_len = len - plen - name_len - 1;

            /* --- Grava o conteúdo em disco --- */
            FILE* f = fopen(filename, "wb");
            if (f) {
                fwrite(content, 1, content_len, f);
                fclose(f);
                printf("\n[RX] File saved: %s (%zu bytes)\n> ",
                    filename, content_len);
            }
            else {
                fprintf(stderr, "\n[RX] Cannot write %s: %s\n> ",
                    filename, strerror(errno));
            }
            fflush(stdout);
            return;
        }
    }

    /* --- Mensagem de texto ou binária: exibe no terminal --- */
    printf("\n[RX] (%zu bytes): %.*s\n> ", len, (int)len, (char*)data);
    fflush(stdout);
}

static void on_status(uint8_t msg_id, pkt_status_t status, void* ctx)
{
    (void)ctx;
    /* --- Notifica o usuário sobre o resultado final da transmissão --- */
    if (status == PKT_STATUS_OK) {
        printf("\n[TX] msg#%u delivered\n> ", (unsigned)msg_id);
    }
    else {
        printf("\n[TX] msg#%u FAILED (max retries)\n> ", (unsigned)msg_id);
    }
    fflush(stdout);
}

/* -------------------------------------------------------------------------- */
/* Thread receptora de rede                                                    */
/* -------------------------------------------------------------------------- */

static void* net_thread(void* arg)
{
    app_t* app = (app_t*)arg;
    uint8_t buf[2048];

    while (app->running) {
        /* --- Aguarda atividade no socket por até 10 ms (permite chamar pkt_tick regularmente) --- */
        fd_set fds;
        FD_ZERO(&fds);
        int fd = udp_fd(app->udp);
        FD_SET(fd, &fds);
        struct timeval tv = { .tv_sec = 0, .tv_usec = 10000 }; /* 10 ms */

        int r = select(fd + 1, &fds, NULL, NULL, &tv);

        /* --- Adquire o mutex para acesso exclusivo ao packetizer --- */
        pthread_mutex_lock(&app->lock);

        /* --- Se chegou um datagrama, injeta os bytes no packetizer --- */
        if (r > 0 && FD_ISSET(fd, &fds)) {
            ssize_t n = udp_recv(app->udp, buf, sizeof(buf));
            if (n > 0) {
                pkt_feed(app->pkt, buf, (size_t)n);
            }
        }

        /* --- Avança o timer do protocolo (detecta timeouts e dispara retransmissões) --- */
        pkt_tick(app->pkt, now_ms());

        pthread_mutex_unlock(&app->lock);
    }

    return NULL;
}

/* -------------------------------------------------------------------------- */
/* Envio de arquivo                                                            */
/* -------------------------------------------------------------------------- */

static int send_file(app_t* app, const char* path)
{
    /* --- Abre o arquivo e verifica se é válido --- */
    FILE* f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "[TX] Cannot open %s: %s\n", path, strerror(errno));
        return -1;
    }

    struct stat st;
    if (stat(path, &st) < 0 || st.st_size <= 0) {
        fprintf(stderr, "[TX] Cannot stat %s\n", path);
        fclose(f);
        return -1;
    }

    /* --- Extrai o nome base do caminho (ex: "/foo/bar.txt" → "bar.txt") --- */
    const char* basename = strrchr(path, '/');
    basename = basename ? basename + 1 : path;

    /* --- Calcula o tamanho total e aloca o buffer do payload --- */
    /* Formato: "FILE:<nome>\n<conteúdo binário>" */
    const char* prefix = "FILE:";
    size_t plen = strlen(prefix);
    size_t nlen = strlen(basename);
    size_t total = plen + nlen + 1 + (size_t)st.st_size;

    if (total > 64 * 1024) {
        fprintf(stderr, "[TX] File too large (max 64 KiB)\n");
        fclose(f);
        return -1;
    }

    uint8_t* buf = malloc(total);
    if (!buf) { fclose(f); return -1; }

    /* --- Preenche o buffer com prefixo, nome, separador e conteúdo do arquivo --- */
    memcpy(buf, prefix, plen);
    memcpy(buf + plen, basename, nlen);
    buf[plen + nlen] = '\n';
    size_t read = fread(buf + plen + nlen + 1, 1, (size_t)st.st_size, f);
    fclose(f);

    if (read != (size_t)st.st_size) {
        fprintf(stderr, "[TX] Read error\n");
        free(buf);
        return -1;
    }

    /* --- Aguarda o packetizer ficar livre antes de enviar --- */
    for (int retries = 0; retries < 100; retries++) {
        pthread_mutex_lock(&app->lock);
        int busy = pkt_is_busy(app->pkt);
        pthread_mutex_unlock(&app->lock);
        if (!busy) break;
        struct timespec ts = { .tv_sec = 0, .tv_nsec = 50000000L }; /* 50 ms */
        nanosleep(&ts, NULL);
    }

    /* --- Envia o payload via packetizer e exibe o resultado --- */
    pthread_mutex_lock(&app->lock);
    uint8_t msg_id;
    int rc = pkt_send(app->pkt, buf, total, &msg_id);
    pthread_mutex_unlock(&app->lock);

    free(buf);

    if (rc == 0) {
        printf("[TX] Sending file '%s' (%zu bytes) as msg#%u\n",
            basename, total, (unsigned)msg_id);
    }
    else {
        fprintf(stderr, "[TX] pkt_send failed (busy?)\n");
    }
    return rc;
}

/* -------------------------------------------------------------------------- */
/* Função principal                                                             */
/* -------------------------------------------------------------------------- */

static void usage(const char* prog)
{
    fprintf(stderr,
        "Usage: %s --local-port <N> --remote-port <M> "
        "[--remote-host <IP>]\n\n"
        "Commands:\n"
        "  msg  <text>   Send a text message\n"
        "  file <path>   Send a file\n"
        "  quit          Exit\n",
        prog);
}

int main(int argc, char* argv[])
{
    /* --- Analisa os argumentos da linha de comando --- */
    uint16_t    local_port = 0;
    uint16_t    remote_port = 0;
    const char* remote_host = "127.0.0.1";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--local-port") == 0 && i + 1 < argc) {
            local_port = (uint16_t)atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--remote-port") == 0 && i + 1 < argc) {
            remote_port = (uint16_t)atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--remote-host") == 0 && i + 1 < argc) {
            remote_host = argv[++i];
        }
        else {
            usage(argv[0]);
            return 1;
        }
    }

    /* --- Valida que as portas obrigatórias foram informadas --- */
    if (local_port == 0 || remote_port == 0) {
        usage(argv[0]);
        return 1;
    }

    /* --- Abre o transporte UDP --- */
    udp_transport_t* udp = udp_open(local_port, remote_host, remote_port);
    if (!udp) {
        fprintf(stderr, "Failed to open UDP transport\n");
        return 1;
    }

    /* --- Cria e configura a instância do packetizer --- */
    app_t app;
    memset(&app, 0, sizeof(app));
    app.udp = udp;
    app.running = 1;
    pthread_mutex_init(&app.lock, NULL);

    pkt_config_t cfg = {
        .write = transport_write,
        .on_message = on_message,
        .on_status = on_status,
        .user_ctx = &app,
        .max_payload = 240,
        .timeout_ms = 500,
        .max_retries = 5,
    };

    app.pkt = pkt_create(&cfg);
    if (!app.pkt) {
        fprintf(stderr, "Failed to create packetizer\n");
        udp_close(udp);
        return 1;
    }

    /* --- Inicia a thread de rede --- */
    pthread_t thread;
    pthread_create(&thread, NULL, net_thread, &app);

    printf("Peer ready — local:%u → remote:%s:%u\n",
        local_port, remote_host, remote_port);
    printf("Commands: msg <text> | file <path> | quit\n\n");

    /* --- Loop principal: lê comandos do stdin --- */
    char line[4096];
    while (1) {
        printf("> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) break;

        /* Remove \r e \n (compatível com terminações Unix e Windows) */
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        if (strcmp(line, "quit") == 0 || strcmp(line, "exit") == 0) break;

        if (strncmp(line, "msg ", 4) == 0) {
            const char* text = line + 4;
            size_t tlen = strlen(text);
            if (tlen == 0) { printf("Usage: msg <text>\n"); continue; }

            /* --- Aguarda o packetizer ficar livre antes de enviar --- */
            for (int i = 0; i < 100; i++) {
                pthread_mutex_lock(&app.lock);
                int busy = pkt_is_busy(app.pkt);
                pthread_mutex_unlock(&app.lock);
                if (!busy) break;
                struct timespec ts = { .tv_sec = 0, .tv_nsec = 50000000L }; /* 50 ms */
                nanosleep(&ts, NULL);
            }

            /* --- Envia a mensagem e exibe o resultado --- */
            pthread_mutex_lock(&app.lock);
            uint8_t msg_id;
            int rc = pkt_send(app.pkt, (const uint8_t*)text, tlen, &msg_id);
            pthread_mutex_unlock(&app.lock);

            if (rc == 0) {
                printf("[TX] Sent msg#%u (%zu bytes)\n", (unsigned)msg_id, tlen);
            }
            else {
                fprintf(stderr, "[TX] pkt_send failed\n");
            }

        }
        else if (strncmp(line, "file ", 5) == 0) {
            send_file(&app, line + 5);

        }
        else {
            printf("Unknown command. Try: msg <text> | file <path> | quit\n");
        }
    }

    /* --- Encerramento: aguarda a thread e libera recursos --- */
    app.running = 0;
    pthread_join(thread, NULL);
    pthread_mutex_destroy(&app.lock);
    pkt_destroy(app.pkt);
    udp_close(udp);

    printf("Goodbye.\n");
    return 0;
}
