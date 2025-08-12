#include "uartManager.h"
#include "esp_log.h"
#include "driver/uart.h"
#include <string.h>
#include "uartProcess.h"

static void uart_sim_init();


void uartSim_start() {
    uart_sim_init();
    xTaskCreate(uartSim_task, "uart_task", 8192, NULL, 5, NULL);
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
