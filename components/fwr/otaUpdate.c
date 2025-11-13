#include "otaUpdate.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_timer.h"
#include "mbedtls/md5.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uartProcess.h"
#include "freertos/FreeRTOS.h"


#define TAG "OTA UPDATE"
#define UART_SIM_OTA UART_NUM_1
#define BUF_SIZE_OTA 1024   // Tamaño de chunk según doc SIM7600 (~1 KB)
#define OTA_URL      "https://gruposisprovisa.mx/fw/1.0.1/project-name.bin"

static esp_err_t download_and_write_ota();
esp_ota_handle_t update_handle;
esp_partition_t *ota_partition;

// Función principal OTA
esp_err_t ota_uart_sim7600_start() {
    ESP_LOGI(TAG, "Inicializando OTA...");

    ota_partition = esp_ota_get_next_update_partition(NULL);
    if (!ota_partition) {
        ESP_LOGE(TAG, "No se encontró partición OTA");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Partición destino: %s", ota_partition->label);

    
    esp_err_t err = esp_ota_begin(ota_partition, OTA_SIZE_UNKNOWN, &update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin falló");
        return err;
    }

    // Descargamos y escribimos firmware
    err = download_and_write_ota();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error en descarga");
        esp_ota_abort(update_handle);
        return err;
    }
    return ESP_OK;
}

esp_err_t ota_writeChunk(uint8_t *buf, size_t  len){
    esp_err_t err = esp_ota_write(update_handle, buf, len);
    if (err != ESP_OK) return err;
    return ESP_OK;
}

esp_err_t end_ota(){
    // Finalizamos OTA
    esp_err_t err = esp_ota_end(update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end falló: %s", esp_err_to_name(err));
        return err;
    }

    // Changing partition
    err = esp_ota_set_boot_partition(ota_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition falló");
        return err;
    }

    ESP_LOGI(TAG, "OTA completada con éxito. Reiniciando...");
    esp_restart();
    return ESP_OK;
}

static esp_err_t download_and_write_ota() {
    uint8_t buf[BUF_SIZE_OTA];
    int len = 0 ;

    ESP_LOGI(TAG, "Descargando firmware desde SIM7600...");

    // Send commands to get communication with HTTP

    // AT+HTTPINIT
    // Open the connection HTTP
    uartProcess_sendCommand("AT+HTTPINIT");
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // AT+HTTPPARA="URL", OTA_URL
    // SENDING DE URL FOR THE CONNECTION
    char sendCmdBuffer[100];
    snprintf(sendCmdBuffer, sizeof(sendCmdBuffer), "AT+HTTPPARA=\"URL\",\"%s\"", OTA_URL);
    uartProcess_sendCommand(sendCmdBuffer);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    
    // AT+HTTPACTION=0
    // SENDING THE ACTION 'GET' TO THE URL
    uartProcess_sendCommand("AT+HTTPACTION=0");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return ESP_OK;
}
