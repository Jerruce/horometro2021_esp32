#ifndef _CUSTOM_INPUT_CAPTURE_H_
#define _CUSTOM_INPUT_CAPTURE_H_

#include "esp_attr.h"
#include "soc/rtc.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

#define RPM_GPIO 19         // gpio19 entrada de onda cuadrada
#define CAP0_INT_EN BIT(27) // habilitar interrupcion en CAP0

#define horometro_captura_en_ticks() mcpwm_capture_signal_get_value(MCPWM_UNIT_0, MCPWM_SELECT_CAP0)
#define horometro_captura_en_hz() ({                   \
    uint32_t ticks = horometro_captura_en_ticks();     \
    float hz = (rtc_clk_apb_freq_get() * 1.0) / ticks; \
    hz;                                                \
})

typedef struct
{
    uint32_t capture_signal;
    mcpwm_capture_signal_t sel_cap_signal;
} capture;

#ifdef __cplusplus
extern "C"
{
#endif

    void inputCapture_iniciar(void (*isr)(void *));

#ifdef __cplusplus
}
#endif

#endif