#include "uartManager.h"
#include "uartProcess.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "esp_log.h"
#include <string.h>
#include "tpcmdManager.h"

static const char *TAG = "UART_process";

void uartSim_task(void *arg) {
    char response[256];
    /*char message[256]; //Mensaje que se va a mandar al servidor*/
    while (1) {
        /*int len = uartManager_readBinary((uint8_t *)response, sizeof(response), 100);*/
        int len = uartManager_readEvent(response, sizeof(response), 100);
        if (len > 0) {
            if (strstr(response, "+CGNSSINFO:") != NULL ) {

                ESP_LOGI(TAG, "GNSS RAWDATA =>%s", response);

            } else if(strstr(response, "+IPD") != NULL) {
            
                ESP_LOGI(TAG, "Comando TCP =>%s", response);

            } else if (strstr(response, "+CMTI:") != NULL) {

                ESP_LOGI(TAG, "Comando SMS =>%s", response);

            } else if(strstr(response, "+CPSI:") != NULL) { 

                ESP_LOGI(TAG, "PSI RAWDATA =>%s", response);
            } else {
                            
                ESP_LOGI(TAG, "RSP UART =>%s", response);
            }
            
        }
    }
}

void serialConsole_task(void *arg) {
    uint8_t data[BUF_SIZE_SER];
    while (1) {
        int len = uart_read_bytes(UART_SER, data, BUF_SIZE_SER - 1, pdMS_TO_TICKS(100));
        if (len > 0) {
            data[len] = '\0'; // Convertir a string
            if (strncmp((char*)data, "AT", 2) == 0 && SERIAL_DEBUG) {
                uartManager_sendCommand((char*)data);
            } else {
                ESP_LOGI(TAG,"%s",readCmd((char*)data));                
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int uartManager_readBinary(uint8_t *buffer, int max_length, int timeout_ms) {
    return uart_read_bytes(UART_SIM, buffer, max_length, pdMS_TO_TICKS(timeout_ms));
}

void uartManager_sendCommand(const char *command) {
    uart_write_bytes(UART_SIM, command, strlen(command));
    uart_write_bytes(UART_SIM, "\r\n", 2);
}
int uartManager_readEvent(char *buffer, int max_length, int timeout_ms) {
    int len = uart_read_bytes(UART_SIM, (uint8_t *)buffer, max_length - 1, pdMS_TO_TICKS(timeout_ms));
    if (len > 0) {
        buffer[len] = '\0';
    }
    return len;
}

esp_err_t download_firmware_via_uart(esp_ota_handle_t update_handle) {
    uint8_t data[BUF_SIZE_OTA];
    int len;

    ESP_LOGI(TAG, "Esperando datos OTA desde SIM7600...");

    while (1) {
        len = uart_read_bytes(UART_SIM, data, BUF_SIZE_OTA, pdMS_TO_TICKS(5000));
        if (len > 0) {
            esp_err_t err = esp_ota_write(update_handle, data, len);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error escribiendo datos OTA: %s", esp_err_to_name(err));
                return err;
            }
        } else {
            ESP_LOGI(TAG, "Fin de datos OTA o timeout.");
            break;
        }
    }

    return ESP_OK;
}