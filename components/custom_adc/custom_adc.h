#ifndef _CUSTOM_ADC_H_
#define _CUSTOM_ADC_H_

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DEFAULT_VREF 1142 //Use adc2_vref_to_gpio() o via espefuse.py
#define NO_OF_SAMPLES 64  //Multisampling

#define sCorriente_unit ADC_UNIT_2
#define sCorriente_atten ADC_ATTEN_DB_11
#define sCorriente_width ADC_WIDTH_BIT_12
#define sCorriente_chnl ADC2_CHANNEL_7

#define acelermtr_unit ADC_UNIT_1
#define acelermtr_atten ADC_ATTEN_DB_11
#define acelermtr_width ADC_WIDTH_BIT_12
#define acelermtr_chnl ADC1_CHANNEL_6

#define vbat_unit ADC_UNIT_1
#define vbat_atten ADC_ATTEN_DB_11
#define vbat_width ADC_WIDTH_BIT_12
#define vbat_chnl ADC1_CHANNEL_0

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t horometro_iniciar_adc_acelerometro();
esp_err_t horometro_iniciar_adc_vbat();
esp_err_t horometro_iniciar_adc_sensorCorriente();
uint32_t horometro_leer_acelerometro();
uint32_t horometro_leer_vbat();
uint32_t horometro_leer_sensorCorriente();

#ifdef __cplusplus
}
#endif

#endif