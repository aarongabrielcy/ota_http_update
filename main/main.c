#include "pwrManager.h"
#include "uartManager.h"
#include "cmdManager.h"

void app_main(void) {
    power_init();
    uartSerial_start();
    uartSim_start();
}