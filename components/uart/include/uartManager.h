#pragma once

#define UART_SIM UART_NUM_1  // Usamos UART1
#define TXD_PIN 4
#define RXD_PIN 5
#define BUF_SIZE (1024)

void uart_init();
void uartManager_start();