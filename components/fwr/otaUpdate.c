#include "otaUpdate.h"
#include "esp_log.h"
#include "esp_system.h"
#include "uartProcess.h"

#define TAG "OTA UPDATE"

esp_err_t test_ota_dos() {
 esp_err_t err;

    const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL);
    if (!ota_partition) {
        ESP_LOGE(TAG, "No se encontró partición OTA");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Iniciando OTA en partición %s", ota_partition->label);
    esp_ota_handle_t update_handle;
    err = esp_ota_begin(ota_partition, OTA_SIZE_UNKNOWN, &update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin falló");
        return err;
    }

    err = download_firmware_via_uart(update_handle);
    if (err != ESP_OK) {
        esp_ota_abort(update_handle);
        return err;
    }

    err = esp_ota_end(update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end falló");
        return err;
    }

    err = esp_ota_set_boot_partition(ota_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition falló");
        return err;
    }

    ESP_LOGI(TAG, "OTA completada. Reiniciando...");
    esp_restart();
    return ESP_OK;   
}