#include "cmdManager.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/uart.h"
#include <string.h>
#include "uartProcess.h"
#include "cmdProcess.h"

#define TAG "COMMAND MANAGER"

static void uart_serial_init() {
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


void serialConsole_task(void *arg) {
    uint8_t data[BUF_SIZE_SER];
    while (1) {
        int len = uart_read_bytes(UART_SER, data, BUF_SIZE_SER - 1, pdMS_TO_TICKS(100));
        if (len > 0) {
            data[len] = '\0'; // Convertir a string
            if (strncmp((char*)data, "AT", 2) == 0 && SERIAL_DEBUG) {
                uartProcess_sendCommand((char*)data);
            } else {
                //Evalua esta logica si esta funcion debe devolver algo
                ESP_LOGI(TAG, "%s",readCmd((char*)data) );
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void uartSerial_start() {
    uart_serial_init();
    xTaskCreate(serialConsole_task, "serial_console_task", 8192, NULL, 5, NULL);
}