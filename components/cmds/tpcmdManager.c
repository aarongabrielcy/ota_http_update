#include "tpcmdManager.h"
#include "freertos/FreeRTOS.h"
#include <string.h>
#include "esp_log.h"
#include <ctype.h>
#include "tpcmdProcessing.h"

#define TAG "COMMAND MANAGER"

char id[20];
char ccid[25];
char pss_wf[10];
char location[100];
char lat[20] = "+0.000000";
char lon[20] = "+0.000000";

static char * cmdAction(ParsedCommand *parsed);
static char * cmdQuery(ParsedCommand *parsed);
static char * cmdQueryWithValue(ParsedCommand *parsed);


char *readCmd(const char *command) {
    static char buffer[256];
    ParsedCommand cmd;

    switch (validateCommand(command, &cmd)) {
        case EMPTY:
            snprintf(buffer, sizeof(buffer), "%s%s", command, "EMPTY CMD");
            return buffer;

        case QUERY_WITHOUT_VALUE:
            snprintf(buffer, sizeof(buffer), "%d%s&%s", cmd.number, cmd.value, cmdQuery(&cmd));
            return buffer;

        case ACTION:
            snprintf(buffer, sizeof(buffer), "%d%c%s&%s", cmd.number, cmd.symbol, cmd.value, cmdAction(&cmd));
            return buffer;

        case INVALID_CMD:
            snprintf(buffer, sizeof(buffer), "%s%s", command, "INVALID CMD");
            return buffer;

        case INVALID_SYMBOL:
            snprintf(buffer, sizeof(buffer), "%s%s", command, "INVALID SYMBOL");
            return buffer;

        case INVALID_ACTION:
            snprintf(buffer, sizeof(buffer), "%s%s", command, "INVALID ACTION");
            return buffer;

        case INVALID_NUMBER:
            snprintf(buffer, sizeof(buffer), "%s%s", command, "INVALID NUMBER");
            return buffer;

        case QUERY_WITH_VALUE:
            snprintf(buffer, sizeof(buffer), "%d%c%s&%s", cmd.number, cmd.symbol, cmd.value, cmdQueryWithValue(&cmd));
            return buffer;

        case INVALID_QUERY_VALUE:
            snprintf(buffer, sizeof(buffer), "%s%s", command, "INVALID QUERY VALUE");
            return buffer;

        case INVALID_END_SYMBOL:
            snprintf(buffer, sizeof(buffer), "%s%s", command, "INVALID END SYMBOL");
            return buffer;

        default:
            snprintf(buffer, sizeof(buffer), "%s%s", command, "NO RESULT");
            return buffer;
    }
}
int validateCommand(const char *input, ParsedCommand *parsed) {

    if (input == NULL || *input == '\0' || parsed == NULL) {
        return EMPTY;
    }

    int i = 0;
    while (isdigit((unsigned char)input[i])) {
        i++;
    }
    if (i == 0) {
        return INVALID_CMD;
    }

    // Guardar número
    char numberStr[6] = {0};
    if (i >= sizeof(numberStr)) {
        return INVALID_NUMBER;
    }
    strncpy(numberStr, input, i);
    parsed->number = atoi(numberStr);

    // Verificar símbolo
    char symbol = input[i];
    if (symbol != '#' && symbol != '?') {
        return INVALID_SYMBOL;
    }
    parsed->symbol = symbol;

    const char *value = input + i + 1;

    // Verificar terminación en '$'
    const char *end = strchr(value, '$');
    if (end == NULL) {
        return INVALID_END_SYMBOL;
    }

    int len = end - value;
    if (len >= sizeof(parsed->value)) {
        len = sizeof(parsed->value) - 1;
    }
    strncpy(parsed->value, value, len);
    parsed->value[len] = '\0';

    // Evaluar tipo de comando según el símbolo
    if (symbol == '#') {
        if (parsed->value[0] == '\0') {
            return INVALID_ACTION;
        }
        return ACTION;
    } else if (symbol == '?') {
        if (parsed->value[0] == '\0') {
            return QUERY_WITHOUT_VALUE;
        }
        if (!isdigit((unsigned char)parsed->value[0]) || parsed->value[1] != '\0') {
            return INVALID_QUERY_VALUE;
        }
        return QUERY_WITH_VALUE;
    }
    ESP_LOGI(TAG, "Saliendo de la validación...");
    return INVALID_CMD;
}

char *cmdAction(ParsedCommand *parsed) {
    switch (parsed->number) {
        case SVPT:
            return processSVPT(parsed->value);
        case CLOP:
            return proccessCLOP(parsed->value);
        case MRST:
            return resetDevice(parsed->value);
        case OPCT:
        /*if(atoi(parsed->value) == 1 ) {
             if(outputControl(OUTPUT1_PIN, atoi(parsed->value)) ) {
                return "ON";
             } else { return "ERR ON"; }
        } else if(atoi(parsed->value) == 0) {
            if(outputControl(OUTPUT1_PIN, atoi(parsed->value)) ) {
                return "OFF";
             } else { return "ERR OFF"; }
        }*/
        return "NA";
        case DLBF:
            /*if(atoi(parsed->value) > 0) {
                if(spiffs_delete_block(atoi(parsed->value) )) {
                   return "DELETE OK";
                } else {
                    return "NOT DELETED";
                }
            } else {return "NOT FOUND";}*/ 
        case PDWF:
            char * response = processPassword(parsed->value);

            if (strncmp(response, "save", 4) == 0) {
                //nvs_save_str("password_wifi", parsed->value);
                
            } else if (strncmp(response, "Error", 5) == 0) {
                return response;
            }
             return response;

        case AEWF:
            /*if(atoi(parsed->value) == 1 ) {
                if (nvs_read_str("password_wifi", pss_wf, sizeof(pss_wf)) != NULL) {
                    /////// Activa el wifi ///////
                    if(wifi_manager_enable(atoi(parsed->value)) ){
                        return "WIFI ON";
                    } else { return "ERR WIFI"; } 
                } else { return "PASS NO CONFIG"; }
                
            } else if(atoi(parsed->value) == 0 ) {
                if(wifi_manager_enable(atoi(parsed->value)) ){
                    return "WIFI OFF";
                } else { return "ERR WIFI"; }
            }*/
            return "ERR";
        case AEGP:
          /*if (strcmp(parsed->value, "1") == 0) {
            const char *value = "1";
            char command[20];
            snprintf(command, sizeof(command), "AT+CGPS=%s", value);
            printf("Comando AT: %s\n", command);
            if(sim7600_sendReadCommand(command)){
                return "GPS ON";
            } else {
                return "GPS ON ERR";
            }
        } else if (strcmp(parsed->value, "0") == 0) {
            const char *value = "0";
            char command[20];
            snprintf(command, sizeof(command), "AT+CGPS=%s", value);
            printf("Comando AT: %s\n", command);
            if(sim7600_sendReadCommand(command)){
                return "GPS OFF";
            } else {
                return "GPS OFF ERR";
            }
        }
            return "ERR";
        if(atoi(parsed->value) >= 10 ) {
                char command[50];
                snprintf(command, sizeof(command), "AT+CGNSSINFO=%s", parsed->value);
                printf("Comando AT: %s\n", command);
            } else { return "ERR: The reporting time cannot be less than 10 seconds."; }*/
            
            return "OK";
        case FWUP:

        return processUpdate(parsed->value);
        default:
            return "CMD ACTION NOT FOUND";
    }
}
char *cmdQuery(ParsedCommand *parsed) {
    //static char buffer[32];
    switch (parsed->number) {
        case DVID:
            /*if (nvs_read_str("device_id", id, sizeof(id)) != NULL) {
                return id;
            }else { return "ERR"; }*/
        case RTCT:
            /*int value = nvs_read_int("dev_reboots");
            snprintf(buffer, sizeof(buffer), "%d", value);  // convierte el int a string
            return buffer;*/
        case SIID:
            /*if (nvs_read_str("sim_id", ccid, sizeof(ccid)) != NULL) {
                return ccid;
            }else { return "ERR"; }*/
        case PWFR:
            /*if (nvs_read_str("password_wifi", pss_wf, sizeof(pss_wf)) != NULL) {
                return pss_wf;
            }else { return "ERR"; }*/
        case TKRP: 
            /*esp_event_loop_handle_t loop = get_event_loop();            
            if (loop) {
                esp_err_t err = esp_event_post_to(loop, SYSTEM_EVENTS, TRACKING_RPT, NULL, 0, portMAX_DELAY);
                if (err == ESP_OK) {
                    return "OK";  // El evento se propagó correctamente
                } else {
                    return "ERR";  // Falló al propagar
                }
            } else {
                return "ERR";  // El event loop no está disponible
            }*/
        case LOCA:
            /*if(nvs_read_str("last_valid_lon", lon, sizeof(lon)) != NULL) {
                ESP_LOGI(TAG, "last_lat_NVS=%s", lon);   
            } else { return "ERR LON"; }
            if(nvs_read_str("last_valid_lat", lat, sizeof(lat)) != NULL) {
                ESP_LOGI(TAG, "last_lat_NVS=%s", lat);   
            } else { return "ERR LAT"; }
            snprintf(location, sizeof(location), "https://www.google.com/maps/search/?api=1&query=%s,%s&zoom=20", lat, lon);
        return location;*/
        default:
            return "NOT FOUND";
    }
}
char *cmdQueryWithValue(ParsedCommand *parsed) {
    /*switch (parsed->number) {
        case OPST: 
            return outputState(atoi(parsed->value))? "ON" : "OFF";
        default:
            return "NOT FOUND";
        break;
    }*/
   return "NA";
}