idf_component_register(
    SRCS "uartProcess.c" "uartManager.c"
    INCLUDE_DIRS "include"
    REQUIRES
        #cmd 
        driver
        esp_event
        app_update
        fwr
)