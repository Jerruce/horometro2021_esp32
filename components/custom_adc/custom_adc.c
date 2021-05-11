#include "custom_adc.h"

#define TAG "custom ADC lib"

static esp_adc_cal_characteristics_t *acelermtr_adc_chars;
static esp_adc_cal_characteristics_t *vbat_adc_chars;
static esp_adc_cal_characteristics_t *sCorriente_adc_chars;

//revisar tipo de calibracion permitido
static void check_efuse()
{
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
        printf("eFuse Two Point: Supported\n");
    else
        printf("eFuse Two Point: NOT supported\n");

    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
        printf("eFuse Vref: Supported\n");
    else
        printf("eFuse Vref: NOT supported\n");
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_DEFAULT_VREF) == ESP_OK)
        printf("default Vref: Supported\n");
    else
        printf("default Vref: NOT supported\n");
}

//imprimir tipo de calibracion
static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
    {
        printf("Characterized using Two Point Value\n");
    }
    else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
    {
        printf("Characterized using eFuse Vref\n");
    }
    else
    {
        printf("Characterized using Default Vref\n");
    }
}

esp_err_t horometro_iniciar_adc_acelerometro()
{
    //revisar opciones de calibracion
    check_efuse();

    //configuracion para el controlador del sar
    adc1_config_width(acelermtr_width);
    esp_err_t err = adc1_config_channel_atten(acelermtr_chnl, acelermtr_atten);

    //configuracion para el SAR(caracterizar)
    acelermtr_adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(acelermtr_unit, acelermtr_atten, acelermtr_width, DEFAULT_VREF, acelermtr_adc_chars);
    print_char_val_type(val_type);

    return err;
}

esp_err_t horometro_iniciar_adc_vbat()
{
    //revisar opciones de calibracion
    check_efuse();

    //configuracion para el controlador del sar
    adc1_config_width(vbat_width);
    esp_err_t err = adc1_config_channel_atten(vbat_chnl, vbat_atten);

    //configuracion para el SAR(caracterizar)
    vbat_adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(vbat_unit, vbat_atten, vbat_width, DEFAULT_VREF, vbat_adc_chars);
    print_char_val_type(val_type);

    return err;
}

esp_err_t horometro_iniciar_adc_sensorCorriente()
{
    //configuracion para el controlador del sar
    //sar 2 requiere "width" al leer valor
    esp_err_t err = adc2_config_channel_atten((adc2_channel_t)sCorriente_chnl, sCorriente_atten);

    //configuracion para el SAR(caracterizar)
    sCorriente_adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(sCorriente_unit, sCorriente_atten, sCorriente_width, DEFAULT_VREF, sCorriente_adc_chars);
    print_char_val_type(val_type);

    return err;
}

uint32_t horometro_leer_acelerometro()
{
    uint32_t adc_reading = 0;

    //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++)
    {
        adc_reading += adc1_get_raw(acelermtr_chnl);
    }
    adc_reading /= NO_OF_SAMPLES;

    //Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, acelermtr_adc_chars);
    printf("Raw: %d\tVoltage acelerometro: %dmV\n", adc_reading, voltage);
    return voltage;
}

uint32_t horometro_leer_vbat()
{
    uint32_t adc_reading = 0;

    //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++)
    {
        adc_reading += adc1_get_raw(vbat_chnl);
    }
    adc_reading /= NO_OF_SAMPLES;

    //Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, vbat_adc_chars);
    printf("Raw: %d\tVoltage vbat: %dmV\n", adc_reading, voltage);
    return voltage;
}

uint32_t horometro_leer_sensorCorriente()
{
    uint32_t ticks = xTaskGetTickCount();

    uint32_t adc_reading = 0;

    //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++)
    {
        int raw;
        adc2_get_raw(sCorriente_chnl, sCorriente_width, &raw);
        adc_reading += raw;
    }
    adc_reading /= NO_OF_SAMPLES;

    //Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, sCorriente_adc_chars);

    ticks -= xTaskGetTickCount();
    //ESP_LOGI(TAG, "ticks totales: %u", ticks);
    //printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
    return voltage;
}