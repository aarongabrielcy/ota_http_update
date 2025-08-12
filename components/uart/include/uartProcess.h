#pragma once
/*#include "esp_err.h"
#include "esp_ota_ops.h"*/

void uartSim_task(void *arg);
void serialConsole_task(void *arg);

int  uartManager_readBinary(uint8_t *buffer, int max_length, int timeout_ms);
int  uartManager_readEvent(   char *buffer, int max_length, int timeout_ms);
void uartProcess_sendCommand(const char *command);