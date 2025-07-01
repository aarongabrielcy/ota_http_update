#include "uartManager.h"
#include "esp_log.h"
#include "driver/uart.h"
#include <string.h>
#include "uartProcess.h"

static void uart_sim_init();
static void uart_ser_init();


void uarts_init() {
    uart_sim_init();
    uart_ser_init();

}

static void uart_sim_init() {
    
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_SIM, &uart_config);
    uart_set_pin(UART_SIM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_SIM, BUF_SIZE_SIM * 2, 0, 0, NULL, 0);
}

static void uart_ser_init() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    
    uart_param_config(UART_SER, &uart_config);
    uart_driver_install(UART_SER, BUF_SIZE_SER * 2, 0, 0, NULL, 0);
}

void uartManager_start() {
    xTaskCreate(uartSim_task, "uart_task", 8192, NULL, 5, NULL);
    xTaskCreate(serialConsole_task, "serial_console_task", 8192, NULL, 5, NULL);
}