#include "pwrManager.h"
#include "uartManager.h"

void app_main(void) {
    power_init();
    uarts_init();
    uartManager_start();
}