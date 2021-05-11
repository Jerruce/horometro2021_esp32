#ifndef _CUSTOM_NVS_H
#define _CUSTOM_NVS_H

#include "nvs_flash.h"
#include "esp_log.h"
#include "string.h"
#include "sys/time.h"

#define wifi_nameSpace "wifi"
#define motorID_nameSpace "motorID"
#define segundos_nameSpace "segundos"

#define interfaces_nameSpace "interfaces"
#define interface_sensores 0
#define interface_wifi 1
#define interface_rele 2

#define alarmas_nameSpace "alarmas"
typedef struct
{
    int8_t alarma_id;
    uint16_t horas_programadas;
    uint32_t segundos_acumulados;
    char tag[20];
    int8_t tag_len;
} alarma_t;

#ifdef __cplusplus
extern "C"
{
#endif

    int motor_inicia_trabajo();
    esp_err_t motor_termina_trabajo();

    void horometro_init_nvs();
    esp_err_t horometro_extraer_credenciales_wifi_from_nvs(const char *nameSpace, char *wifi_ssid, char *wifi_pswd);
    esp_err_t horometro_actualizar_credenciales_wifi_to_nvs(const char *nameSpace, char *wifi_ssid, char *wifi_pswd);

    esp_err_t horometro_extraer_motorID_NVS(const char *nameSpace, char *motorID);
    esp_err_t horometro_actualizar_motorID_NVS(const char *nameSpace, char *motorID);

    esp_err_t horometro_extraer_interfaces_NVS(const char *nameSpace, uint8_t *interfaces);
    esp_err_t horometro_actualizar_interfaces_NVS(const char *nameSpace, uint8_t interface, bool estado);
    int sensores_habilitados();

    esp_err_t horometro_extraer_alarmas_NVS(const char *nameSpace, alarma_t *alarmas);
    esp_err_t horometro_actualizar_alarmas_NVS(const char *nameSpace, alarma_t *alarmas);

    esp_err_t horometro_extraer_segundos_NVS(const char *nameSpace, uint32_t *segundos);
    esp_err_t horometro_actualizar_segundos_NVS(const char *nameSpace, uint32_t add);
    esp_err_t horometro_limpiar_segundos_NVS(const char *nameSpace);

#ifdef __cplusplus
}
#endif

#endif