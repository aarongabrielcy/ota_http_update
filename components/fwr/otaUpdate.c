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

#define TAG "OTA UPDATE"
#define UART_SIM_OTA UART_NUM_1
#define BUF_SIZE_OTA 1024   // Tamaño de chunk según doc SIM7600 (~1 KB)
#define OTA_URL      "https://gruposisprovisa.mx/fw/1.0.1/project-name.bin"
#define OTA_MD5_URL  "https://gruposisprovisa.mx/fw/1.0.1/project-name.bin.md5"

static esp_err_t download_and_write_ota(esp_ota_handle_t update_handle, mbedtls_md5_context *md5_ctx);
static esp_err_t get_md5_from_server(char *md5_str, size_t len);

// Función principal OTA
esp_err_t ota_uart_sim7600_start() {
    ESP_LOGI(TAG, "Inicializando OTA...");

    const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL);
    if (!ota_partition) {
        ESP_LOGE(TAG, "No se encontró partición OTA");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Partición destino: %s", ota_partition->label);

    esp_ota_handle_t update_handle;
    esp_err_t err = esp_ota_begin(ota_partition, OTA_SIZE_UNKNOWN, &update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin falló");
        return err;
    }

    // Inicializamos MD5
    mbedtls_md5_context md5_ctx;
    mbedtls_md5_init(&md5_ctx);
    mbedtls_md5_starts(&md5_ctx);

    // Descargamos y escribimos firmware
    err = download_and_write_ota(update_handle, &md5_ctx);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Descarga falló");
        esp_ota_abort(update_handle);
        return err;
    }

    // Finalizamos OTA
    err = esp_ota_end(update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end falló");
        return err;
    }

    // Obtenemos MD5 calculado
    unsigned char md5_calc[16];
    mbedtls_md5_finish(&md5_ctx, md5_calc);
    mbedtls_md5_free(&md5_ctx);

    char md5_calc_str[33];
    for (int i = 0; i < 16; i++) {
        sprintf(&md5_calc_str[i * 2], "%02x", md5_calc[i]);
    }
    md5_calc_str[32] = '\0';

    ESP_LOGI(TAG, "MD5 calculado: %s", md5_calc_str);

    // Obtenemos MD5 esperado
    char md5_expected[33];
    if (get_md5_from_server(md5_expected, sizeof(md5_expected)) != ESP_OK) {
        ESP_LOGE(TAG, "No se pudo obtener MD5 del servidor");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "MD5 esperado: %s", md5_expected);

    // Comparación
    if (strcasecmp(md5_calc_str, md5_expected) != 0) {
        ESP_LOGE(TAG, "MD5 no coincide, abortando...");
        return ESP_FAIL;
    }

    // Cambiamos partición activa
    err = esp_ota_set_boot_partition(ota_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition falló");
        return err;
    }

    ESP_LOGI(TAG, "OTA completada con éxito. Reiniciando...");
    esp_restart();
    return ESP_OK;
}

// Simulación de descarga vía UART desde SIM7600
static esp_err_t download_and_write_ota(esp_ota_handle_t update_handle, mbedtls_md5_context *md5_ctx) {
    uint8_t buf[BUF_SIZE_OTA];
    int len;

    ESP_LOGI(TAG, "Descargando firmware desde SIM7600...");

    // Aquí enviarías comandos AT al SIM7600 para iniciar descarga HTTPREAD por chunks
    // Ejemplo:
    uartProcess_sendCommand("AT+HTTPINIT");
    // AT+HTTPINIT
    uartProcess_sendCommand("AT+HTTPPARA=\"CID\",1");
    // AT+HTTPPARA="CID",1
    char sendCmdBuffer[100];
    snprintf(sendCmdBuffer, sizeof(sendCmdBuffer), "AT+HTTPPARA=\"URL\",\"%s\"", OTA_URL);
    // AT+HTTPPARA="URL", OTA_URL
    uartProcess_sendCommand(sendCmdBuffer);
    // AT+HTTPACTION=0
    uartProcess_sendCommand("AT+HTTPACTION=0");
    // AT+HTTPREAD=0,1024 (y leer respuesta) agregar a bucle

    while (1) {
        // Simulación: uart_read_bytes debe leer chunk desde SIM7600
        len = uart_read_bytes(UART_SIM_OTA, buf, BUF_SIZE_OTA, pdMS_TO_TICKS(5000));
        if (len > 0) {
            esp_err_t err = esp_ota_write(update_handle, buf, len);
            if (err != ESP_OK) return err;
            mbedtls_md5_update(md5_ctx, buf, len);
        } else {
            ESP_LOGI(TAG, "Fin de datos");
            break;
        }
    }
    return ESP_OK;
}

// Obtiene el MD5 desde un archivo .md5 alojado en el servidor
static esp_err_t get_md5_from_server(char *md5_str, size_t len) {
    // Aquí igual usarías comandos AT para descargar el .md5
    // Por simplicidad, simulamos que llega el string correcto:
    snprintf(md5_str, len, "fe012d6203f84db58bdc11a1c73754a4"); // ejemplo
    return ESP_OK;
}
