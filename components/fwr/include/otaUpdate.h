#pragma once
#include "esp_err.h"

esp_err_t ota_uart_sim7600_start(void);
esp_err_t ota_writeChunk(uint8_t *buf, size_t  len);
esp_err_t end_ota(void);
