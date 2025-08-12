#pragma once
#include "esp_err.h"
#include "esp_ota_ops.h"
#define UART_SIM_OTA UART_NUM_1  // Usamos UART1
#define BUF_SIZE_OTA (1024)
#define OTA_URL_BIN  "https://gruposisprovisa.mx/fw/1.0.1/project-name.bin"
#define OTA_URL_MD5  "https://gruposisprovisa.mx/fw/1.0.1/project-name.bin.md5"

esp_err_t  ota_uart_sim7600_start(void);

