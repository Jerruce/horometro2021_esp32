#ifndef _RELAY_H_
#define _RELAY_H_

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"

#define relayPin GPIO_NUM_5

typedef enum
{
    relay_off,
    relay_on
} estado_relay_t;

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t iniciar_relay();
estado_relay_t relay_obtener_estado();
esp_err_t relay_modificar_estado(estado_relay_t estado);

#ifdef __cplusplus
}
#endif

#endif