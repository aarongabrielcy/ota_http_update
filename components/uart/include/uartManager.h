#include <stdint.h>
#pragma once

#define UART_SIM UART_NUM_1  // Usamos UART1
#define TXD_PIN 4
#define RXD_PIN 5
#define BUF_SIZE_SIM (1024)

#define UART_SER UART_NUM_0
#define BUF_SIZE_SER (1024)

#define SERIAL_DEBUG true 

#define BUF_SIZE_OTA (1024)

void uarts_init();
void uartManager_start();
