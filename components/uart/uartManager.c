#include "uartManager.h"
#include "esp_log.h"
#include "driver/uart.h"

static const char *TAG = "UART_MANAGER";

void uart_init() {
    
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_SIM, &uart_config);
    uart_set_pin(UART_SIM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_SIM, BUF_SIZE * 2, 0, 0, NULL, 0);
}

int uartManager_readBinary(uint8_t *buffer, int max_length, int timeout_ms) {
    return uart_read_bytes(UART_SIM, buffer, max_length, pdMS_TO_TICKS(timeout_ms));
}

static void uart_task(void *arg) {
    char response[256];
    /*char message[256]; //Mensaje que se va a mandar al servidor*/
    while (1) {
        int len = uartManager_readBinary((uint8_t *)response, sizeof(response), 100);
        //////////// DEJAR FIJO EL TIEMPO DE REPORTE HAYA O NO HAYA IGNICIÓN ON, PERO EL EVENTO NO SE EMITE, SE GENERA EL EVENTO DEFAULT
        if (len > 0) {
            ESP_LOGI(TAG, "RSP UART =>%s", response);
        }
    }
}

void uartManager_start() {
    xTaskCreate(uart_task, "uart_task", 8192, NULL, 5, NULL);
}