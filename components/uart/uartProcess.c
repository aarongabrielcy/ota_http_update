#include "uartManager.h"
#include "uartProcess.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "esp_log.h"
#include <string.h>
#include <otaUpdate.h>
//#include "cmdProcess.h"

#define BUF_SIZE_OTA 1024

static const char *TAG = "UART_process";
static uart_state_t uart_state = UART_STATE_IDLE; 

void uartSim_task(void *arg) {
    char response[256];
    uint8_t buf[BUF_SIZE_OTA+47];
    uint8_t buf_anterior[BUF_SIZE_OTA];
    int offset = 47;
    int cuantofalta = 0;
    uint8_t *data_ptr = buf + offset;
    memset(buf_anterior, 0x00, sizeof(buf_anterior));

    while (1) {
        switch (uart_state){
        // Estado de funcionamiento normal - recepcion de cualquier comando AT
        case UART_STATE_IDLE:
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
                } else if(strstr(response, "+HTTPACTION: 0,200,") != NULL){
                    int data = -1;
                    char *data_start = strstr(response, "+HTTPACTION: 0,200,");
                    data_start += strlen("+HTTPACTION: 0,200,");
                    data = atoi(data_start);
                    cuantofalta = data;
                    ESP_LOGI(TAG, "RESPONSE HTTPACTION => %s", response);
                    ESP_LOGI(TAG, "ES EL TAMAnO DEL ARCHIVO => %d", data);
                    uart_state = UART_STATE_OTA;
                }else {           
                    ESP_LOGI(TAG, "RSP UART =>%s", response);
                }
            }
            break;

        //Estado de UART - Recepcion de binarios OTA
        case UART_STATE_OTA: {
            uart_flush(UART_SIM);
            vTaskDelay(200 / portTICK_PERIOD_MS);
            uartProcess_sendCommand("AT+HTTPREAD=1024");
            vTaskDelay(200 / portTICK_PERIOD_MS);
            len = uart_read_bytes(UART_SIM, buf, 1071, pdMS_TO_TICKS(5000));
            if (len > 0) {
                // Deteccion de bloque repetido
                if (memcmp(data_ptr, buf_anterior, len) == 0) {
                    ESP_LOGW(TAG, "Mismo bloque detectado: fin del archivo OTA.");
                    esp_err_t err = end_ota();
                    if (err != ESP_OK){
                        ESP_LOGI(TAG, "Error al finalizar OTA %s", esp_err_to_name(err));
                    }
                    setUart_state(UART_STATE_IDLE);
                    break;
                }
                //ESP_LOG_BUFFER_HEX("CADENA COMPLETA", buf, len);
                memcpy(buf_anterior, data_ptr, BUF_SIZE_OTA);
                
                char *ok_pos = strstr((char *)buf, "\r\nOK");
                uint8_t *parse_start = buf;

                if (ok_pos) {
                    parse_start = (uint8_t *)(ok_pos + 4); // Saltar eco "AT+HTTPREAD=1024\r\r\nOK"
                    ESP_LOGI(TAG, "Cabecera 'AT+HTTPREAD=1024' detectada, saltando eco inicial");
                }


                // Buscar cabecera +HTTPREAD: DATA,xxxx
                const char *pattern_data = "+HTTPREAD: DATA,";
                const char *pattern_fin  = "+HTTPREAD: 0";
                int data_len = -1;
                char *data_start = strstr((char *)buf, pattern_data);
                char *fin_start  = strstr((char *)buf, pattern_fin);
            
                if (fin_start) {
                    // Fin detectado (+HTTPREAD: 0)
                    int fin_index = fin_start - (char *)buf;
                    ESP_LOGI(TAG, "Fin detectado (+HTTPREAD: 0) en índice %d", fin_index);
                    ESP_LOG_BUFFER_HEX(TAG, buf, fin_index);
                    ESP_LOGI(TAG, "FIN DE DATOS OTA");
                    esp_err_t err = end_ota();
                    if (err != ESP_OK){
                        ESP_LOGI(TAG, "Error al finalizar OTA %s", esp_err_to_name(err));
                    }
                    uart_flush(UART_SIM);
                    setUart_state(UART_STATE_IDLE);
                    break;
                }
            
                if (data_start) {
                    // Extraer tamaño del bloque después de "+HTTPREAD: DATA,"
                    data_start += strlen(pattern_data);
                    data_len = atoi(data_start);
                
                    if (data_len > 0) {
                        // Buscar inicio real del binario (fin de cabecera \r\n)
                        char *bin_start = strstr(data_start, "\r\n");
                        if (bin_start) {
                            bin_start += 2; // Saltar \r\n
                            int bin_offset = bin_start - (char *)buf;
                            int bin_len = len - bin_offset;
                        
                            if (bin_len > data_len)
                                bin_len = data_len;
                                
                            ESP_LOGI(TAG, "Bloque HTTPREAD detectado => %d bytes binarios en offset %d", bin_len, bin_offset);
                            uint8_t *chunk_ptr = buf + bin_offset;
                            ESP_LOG_BUFFER_HEX(TAG, buf + bin_offset, bin_len);
                            esp_err_t err = ota_writeChunk(chunk_ptr, bin_len);
                            if(err != ESP_OK){
                                ESP_LOGI(TAG, "Error en el OTA %s", esp_err_to_name(err));
                                setUart_state(UART_STATE_IDLE);
                            }
                            cuantofalta -= bin_len;
                            ESP_LOGI(TAG, "Restan %d bytes por leer...", cuantofalta);
                        } else {
                            ESP_LOGW(TAG, "No se encontró fin de cabecera \\r\\n en +HTTPREAD");
                        }
                    }
                } else {
                    ESP_LOGW(TAG, "No se detectó cabecera +HTTPREAD válida en este bloque");
                }
                
            } else {
                ESP_LOGI(TAG, "FIN DE DATOS OTA");
                setUart_state(UART_STATE_IDLE);
                }
            break;
            }
        }
    }
}

void setUart_state(uart_state_t state){
    uart_state = state;
}

int uartManager_readBinary(uint8_t *buffer, int max_length, int timeout_ms) {
    return uart_read_bytes(UART_SIM, buffer, max_length, pdMS_TO_TICKS(timeout_ms));
}

void uartProcess_sendCommand(const char *command) {
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
