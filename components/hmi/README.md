# HMI (Human-Machine Interface)

Descrição
- Componente responsável pela interface textual da aplicação no terminal serial.
- Exibe menu interativo e executa operações de soma e determinante.

Objetivo
- Fornecer uma camada de interação com o usuário via UART/USB Serial JTAG usando menu de opções.

Estrutura
- `hmi.c` — implementação do loop de menu, leitura de entrada e eco de caracteres.
- `include/hmi.h` — API pública do componente.

Integração
- `hmi_run()` é chamado pela `main` após configurar o console serial.
- O componente usa `operations_add()` e `operations_matrix_determinant()` do componente `operations`.

API pública (atual)
- `esp_err_t hmi_run(void);` — inicia o menu interativo e bloqueia enquanto a interface estiver ativa.

Dependências
- `operations` para execução de soma e determinante.
- Console serial configurado na `main` (USB Serial JTAG + VFS).

Comportamento de entrada
- Leitura de linha com eco de caracteres digitados.
- Suporte a `Backspace` e `Del` durante a digitação.
- `Enter` finaliza a leitura da opção/valor.

Observações de terminal
- Em `idf_monitor`, use apenas números e `Enter` no prompt `Opcao:`.
- Evite `Ctrl+T` durante a entrada de dados, pois ele abre o menu local do monitor.

Exemplo de uso (C, ESP-IDF)
```c
#include "hmi.h"
#include "esp_log.h"
#include "driver/usb_serial_jtag.h"
#include "driver/usb_serial_jtag_vfs.h"
#include <stdio.h>

void app_main(void)
{
    usb_serial_jtag_driver_config_t usb_cfg = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    usb_serial_jtag_driver_install(&usb_cfg);
    usb_serial_jtag_vfs_use_driver();
    usb_serial_jtag_vfs_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    usb_serial_jtag_vfs_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    hmi_run();
}
```

Testes e verificação
- Selecionar opção `1` (soma), inserir dois valores e validar o resultado.
- Selecionar opção `2` (determinante), inserir matriz NxN e validar o resultado.
- Selecionar opção `0` para encerrar.

Notas de desenvolvimento
- A implementação atual é síncrona e bloqueante por design (modo terminal interativo).
- Mensagens de erro retornam `esp_err_t` para facilitar diagnóstico no `main`.



