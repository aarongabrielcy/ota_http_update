idf_component_register(
    SRCS "uartProcess.c" "uartManager.c"
    INCLUDE_DIRS "include"
    REQUIRES driver cmds esp_event app_update
)