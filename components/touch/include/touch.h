#ifndef _TOUCH_H_
#define _TOUCH_H_

//APLICACION USA SOLO UN TOUCH PIN (TP3-GPIO15)

#include "driver/touch_pad.h"
#include "esp_log.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"
#include "esp_sleep.h"

//definir en pooling, interrupcion e interrupcion wake-up
#define TOUCHPAD_PERIODO_DE_FILTRADO (10)
#define TOUCH_PIN TOUCH_PAD_NUM3 //gpio15
#define TOUCH_THRESHOLD_ALTO TOUCH_HVOLT_2V7
#define TOUCH_THRESHOLD_BAJO TOUCH_LVOLT_0V5
#define TOUCH_ATENUACION TOUCH_HVOLT_ATTEN_1V

//definir en interrupcion e interrupcion wake-up
#define TOUCH_TRIGGER_MODE TOUCH_TRIGGER_BELOW

//definir en interrupcion wake-up
#define TOUCH_TRIGGER_SOURCE TOUCH_TRIGGER_SOURCE_SET1

/*
typedef enum
{

} touch_err_t;
*/
//Usar en pooling e interrupcion
#define TOUCH_LEER_DATO_FILTRADO(p_value) touch_pad_read_filtered(TOUCH_PIN, p_value)
#define TOUCH_LEER_DATO_SIN_FILTRAR(p_value) touch_pad_read_raw_data(TOUCH_PIN, p_value)
//Usar en interrupcion
#define TOUCH_APAGAR_INTERRUPCION() touch_pad_intr_disable()
#define TOUCH_ENCENDER_INTERRUPCION() touch_pad_intr_enable()
#define TOUCH_TP3_ESTADO_ACTIVADO() ({             \
    uint32_t estado_pads = touch_pad_get_status(); \
    bool estado_miPad = estado_pads >> TOUCH_PIN;  \
    estado_miPad;                                  \
})
#define TOUCH_BORRAR_ESTADOS() touch_pad_clear_status()
#define TOUCH_INTERRUPCION_ENCENDIDA() ({         \
    bool int_apagada = RTCCNTL.int_ena.rtc_touch; \
    int_apagada;                                  \
})

#ifdef __cplusplus
extern "C"
{
#endif

    void iniciar_touch_pooling();
    void iniciar_touch_con_interrupcion_con_isr(void touch_isr(void *));
    void iniciar_touch_con_interrupcion_sin_isr();
    void iniciar_touch_con_interrupcion_wakeup_con_isr(void touch_isr(void *));
    void iniciar_touch_con_interrupcion_wakeup_sin_isr();

#ifdef __cplusplus
}
#endif

#endif