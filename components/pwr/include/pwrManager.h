#pragma once

#define POWER_KEY_PIN 41
#define POWER_SIM_PIN 38
#define GNSS_LED_PIN  19
#define POWER_LED_PIN 20
#define SIM_DTR_PIN   42
#define IGNITION_PIN  10

void power_init();
void power_off_module();
void power_restart();
