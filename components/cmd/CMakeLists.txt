idf_component_register(
    SRCS "cmdManager.c" "cmdProcess.c"
    INCLUDE_DIRS "include"
        REQUIRES 
        driver
        utils
        uart
)