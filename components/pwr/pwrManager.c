#include "pwrManager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define TAG "POWER"

static void power_on_module(); 
static void power_press_key();

void power_init() {
    ESP_LOGI(TAG, "Inicializando Power Manager...");
    power_on_module();
    power_press_key();
}
// Encender el módulo SIM
static void power_on_module() {
    gpio_reset_pin(POWER_SIM_PIN);
    ESP_LOGI(TAG, "Inicializando POWER SIM en PIN=%d", POWER_SIM_PIN);

    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << POWER_SIM_PIN);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&io_conf);
    gpio_set_level(POWER_SIM_PIN, 1);
}

// Apagar el módulo SIM
void power_off_module() {
    gpio_set_direction(POWER_SIM_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(POWER_SIM_PIN, 0);
}

void power_restart() {
    esp_restart();
}
// Activar la tecla de encendido del módulo SIM
static void power_press_key() {
    gpio_reset_pin(POWER_KEY_PIN);
    ESP_LOGI(TAG, "Inicializando POWER KEY en PIN=%d", POWER_KEY_PIN);

    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << POWER_KEY_PIN);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&io_conf);
    gpio_set_level(POWER_KEY_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(POWER_KEY_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(3000));
    //gpio_set_level(POWER_KEY_PIN, 0);
}