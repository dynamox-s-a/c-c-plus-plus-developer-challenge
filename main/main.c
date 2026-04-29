#include "esp_err.h"
#include "esp_log.h"
#include "driver/usb_serial_jtag.h"
#include "driver/usb_serial_jtag_vfs.h"
#include <stdio.h>

#include "hmi.h"

static const char *TAG = "main";

void app_main(void)
{
    usb_serial_jtag_driver_config_t usb_cfg = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    esp_err_t err = usb_serial_jtag_driver_install(&usb_cfg);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Falha ao instalar USB Serial JTAG: %s", esp_err_to_name(err));
        return;
    }

    usb_serial_jtag_vfs_use_driver();
    usb_serial_jtag_vfs_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    usb_serial_jtag_vfs_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    // Make serial input/output immediate to avoid terminal buffering surprises.
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    err = hmi_run();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "HMI finalizou com erro: %s", esp_err_to_name(err));
    }
}
