#ifndef _RPM_H_
#define _RPM_H_

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_attr.h"
#include <sys/time.h>
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "esp_event_loop.h"
#include "esp_event.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define rpmPin GPIO_NUM_19

#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t iniciar_rpmPin(void (*fn)(void *));
    void medir_velocidad_ON();
    float obtener_rpm();

#ifdef __cplusplus
}
#endif

#endif