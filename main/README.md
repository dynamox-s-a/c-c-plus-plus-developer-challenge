# Main

Descrição
- Aplicação principal que inicializa USB Serial JTAG e executa a HMI textual.

Objetivo
- Configurar o console serial interativo e iniciar o fluxo principal via `hmi_run()`.

Estrutura
- `main.c` — configura driver USB Serial JTAG/VFS e chama `hmi_run()`.
- `CMakeLists.txt` — configura dependências do componente (`hmi`, `esp_driver_usb_serial_jtag`).

API principal
- `void app_main(void);` — ponto de entrada do firmware.

Como compilar e gravar (ESP-IDF)
- Certifique-se de que o ESP-IDF está configurado (use o terminal ESP-IDF).

Exemplos de comandos:

```bash
# Compilar o projeto
idf.py build

# Ajuste a porta serial e grave + monitor
idf.py -p COM6 flash monitor
```

Sequência atual de inicialização
1. Instalar driver USB Serial JTAG.
2. Habilitar VFS com `usb_serial_jtag_vfs_use_driver()`.
3. Configurar line ending RX/TX para o monitor.
4. Desabilitar buffering de `stdin`/`stdout`.
5. Executar `hmi_run()`.

Exemplo de `main.c` (resumo)
```c
#include "driver/usb_serial_jtag.h"
#include "driver/usb_serial_jtag_vfs.h"
#include "hmi.h"
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

Depuração
- Use `idf.py -p COM6 monitor` para logs/interação.
- Se aparecer `Unknown menu character '\x7f'`, feche e reabra o monitor (isso é menu local do `idf_monitor`).

Integração e links úteis
- Veja: `components/hmi/README.md` e `components/operations/README.md` para APIs e exemplos.

Boas práticas
- Digite apenas números e `Enter` no prompt `Opcao:`.
- Evite atalhos do monitor (`Ctrl+T`) durante digitação de opções.

