#pragma once
/*#include "esp_err.h"
#include "esp_ota_ops.h"*/

typedef enum{
    UART_STATE_IDLE,
    UART_STATE_OTA
    //UART_PREPARE_OTA
} uart_state_t;

void uartSim_task(void *arg);
void serialConsole_task(void *arg);

int  uartManager_readBinary(uint8_t *buffer, int max_length, int timeout_ms);
int  uartManager_readEvent(   char *buffer, int max_length, int timeout_ms);
void uartProcess_sendCommand(const char *command);

void setUart_state(uart_state_t state);