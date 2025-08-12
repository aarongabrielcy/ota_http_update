#include "esp_event.h"

ESP_EVENT_DECLARE_BASE(SYSTEM_EVENTS);

typedef enum {
    IGNITION_OFF,
    IGNITION_ON,
    INPUT1_ON, 
    INPUT1_OFF,
    INPUT2_ON, 
    INPUT2_OFF, 
    KEEP_ALIVE,
    TRACKING_RPT,
    SMS_DETECTED,
    WIFI_CONNECTED,
    WIFI_DISCONNECTED,
    DEFAULT
} system_event_t;

esp_event_loop_handle_t get_event_loop(void);
void start_keep_alive_timer(void);
void stop_keep_alive_timer(void);

void start_tracking_report_timer(void);
void stop_tracking_report_timer(void);