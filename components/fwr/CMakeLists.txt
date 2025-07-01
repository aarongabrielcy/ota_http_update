idf_component_register(
    SRCS "otaUpdate.c"
    INCLUDE_DIRS "include"
    REQUIRES uart app_update esp_system esp_partition esp_event
)