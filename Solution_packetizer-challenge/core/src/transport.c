#include <transport.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>


int transport_server_init(Transport* transport, uint16_t port)
{
    struct sockaddr_in addr;

    transport->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (transport->socket_fd < 0)
        return -1;

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(transport->socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        return -1;

    if (listen(transport->socket_fd, 1) < 0)
        return -1;

    return 0;
}


int transport_accept(Transport* transport)
{
    transport->connection_fd = accept(transport->socket_fd, NULL, NULL);

    if (transport->connection_fd < 0)
        return -1;

    return 0;
}


int transport_client_connect(Transport* transport, const char* ip, uint16_t port)
{
    struct sockaddr_in addr;

    transport->connection_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (transport->connection_fd < 0)
        return -1;

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (connect(transport->connection_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        return -1;

    return 0;
}


int transport_send(Transport* transport, const uint8_t* buffer, size_t length) 
{
    return send(transport->connection_fd, buffer, length, 0);
}


int transport_receive(Transport* transport, uint8_t* buffer, size_t max_size)
{
    return recv(transport->connection_fd, buffer, max_size, 0);
}


void transport_close(Transport* transport)
{
    if (transport->connection_fd > 0)
        close(transport->connection_fd);

    if (transport->socket_fd > 0)
        close(transport->socket_fd);
}