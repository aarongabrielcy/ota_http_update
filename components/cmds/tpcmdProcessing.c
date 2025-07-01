#include "tpcmdProcessing.h"
#include "freertos/FreeRTOS.h"
#include <string.h>
#include "esp_log.h"
#include <ctype.h>
#include "otaUpdate.h"

#define TAG "COMMAND PROCESS"

char *proccessCLOP(const char *data) {
    char command[99];
    snprintf(command, sizeof(command), "AT+CGDCONT=1,\"IP\",\"%s\"", data);
    ESP_LOGI(TAG, "Comando AT: %s\n", command);
    if(1){
        return "OK1";
    }else {
        return "ERROR";
    }
}
char *processSVPT(const char *data) {
    char server[32];  // Almacena la IP
    char port[6];     // Almacena el puerto (máximo 5 dígitos + terminador)

    // Buscar la posición del ':' en la cadena
    char *ptr = strchr(data, ':');
    if (ptr == NULL) {
        return "Error format";
    }
    // Copiar la parte de la IP antes de ':'
    size_t len = ptr - data;
    strncpy(server, data, len);
    server[len] = '\0';  // Agregar terminador de cadena

    // Copiar la parte del puerto después de ':'
    strncpy(port, ptr + 1, sizeof(port) - 1);
    port[sizeof(port) - 1] = '\0';  // Asegurar terminador

    // Construir el comando AT
    char command[64];
    snprintf(command, sizeof(command), "AT+CIPOPEN=0,\"TCP\",\"%s\",%s", server, port);
    // Imprimir el comando resultante
    printf("Comando AT: %s\n", command);
    /*if(sim7600_sendReadCommand(command) ){
        return "OK";
    }else {
        return "ERR";
    }*/
   return "NA";    
}

char * resetDevice(const char *value) {
    if(atoi(value) == 1 ) { ///////// cambia el 1 por una contraseña (pass_reset)
        return "RST";
    }
    else { return "ERR"; }
}

char* processPassword(const char *password) {

    if (strlen(password) < 8) {
        return "Error: Password must be at least 8 characters long";
    }

    bool has_number = false;
    bool has_special = false;
    const char *special_chars = "!#%&/(){}[]?¡*+-.";

    for (size_t i = 0; i < strlen(password); ++i) {
        if (isdigit((unsigned char)password[i])) {
            has_number = true;
        }
        if (strchr(special_chars, password[i])) {
            has_special = true;
        }
    }

    if (!has_number) {
        return "Error: Password must contain at least one number";
    }

    if (!has_special) {
        return "Error: Password must contain at least one special character";
    }

    return "save successfully";
    //linkzero234.
}

char* processUpdate(const char *value) {
    //agregar validaciones
    //update_start(value);
    //ota_uart_sim7600_start();
    
    return "Ok";
}