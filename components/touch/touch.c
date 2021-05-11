#include "touch.h"

static const char *TAG = "TOUCH_LIB";
#define TOUCH_NO_THRESHOLD (0)

static void calibrate_touch_pad(touch_pad_t pad)
{
    int avg = 0;
    const size_t calibration_count = 128;
    for (int i = 0; i < calibration_count; ++i)
    {
        uint16_t val;
        touch_pad_read(pad, &val);
        avg += val;
    }
    avg /= calibration_count;
    const int min_reading = 300;
    if (avg < min_reading)
    {
        printf("Touch pad #%d average reading is too low: %d (expecting at least %d). "
               "Not using for deep sleep wakeup.\n",
               pad, avg, min_reading);
        touch_pad_config(pad, 0);
    }
    else
    {
        int threshold = avg - 100;
        printf("Touch pad #%d average: %d, wakeup threshold set to %d.\n", pad, avg, threshold);
        touch_pad_config(pad, threshold);
    }
}

void iniciar_touch_pooling()
{
    //Usar threshold para interrupciones
    //Touch iniciado por SW en modo pooling
    touch_pad_init();
    touch_pad_set_meas_time(TOUCH_PAD_SLEEP_CYCLE_DEFAULT, TOUCH_PAD_MEASURE_CYCLE_DEFAULT);
    touch_pad_set_voltage(TOUCH_THRESHOLD_ALTO, TOUCH_THRESHOLD_BAJO, TOUCH_ATENUACION);
    touch_pad_config(TOUCH_PIN, TOUCH_NO_THRESHOLD);
    touch_pad_filter_start(TOUCHPAD_PERIODO_DE_FILTRADO);
}
void iniciar_touch_con_interrupcion_con_isr(void touch_isr(void *))
{
    iniciar_touch_con_interrupcion_sin_isr();

    // Register touch interrupt ISR
    touch_pad_isr_register(touch_isr, NULL);
}
void iniciar_touch_con_interrupcion_sin_isr()
{
    //Touch iniciado por TIMER en modo interrupcion
    touch_pad_init();
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_set_voltage(TOUCH_THRESHOLD_ALTO, TOUCH_THRESHOLD_BAJO, TOUCH_ATENUACION);
    touch_pad_config(TOUCH_PIN, TOUCH_NO_THRESHOLD); // threshold 0->0xffff(menor -> mayor sensibilidad)
    touch_pad_filter_start(TOUCHPAD_PERIODO_DE_FILTRADO);
    calibrate_touch_pad(TOUCH_PIN);

    //set trigger mode
    touch_pad_set_trigger_mode(TOUCH_TRIGGER_MODE);
}
void iniciar_touch_con_interrupcion_wakeup_con_isr(void touch_isr(void *))
{
    //Touch iniciado por TIMER en modo interrupcion
    iniciar_touch_con_interrupcion_con_isr(touch_isr);

    //CONFIGURAR LA INTERRUPCION WAKE UP   
    touch_pad_set_trigger_source(TOUCH_TRIGGER_SOURCE); //set source

    //HABILITAR COMO FUENTE DE WAKE-UP
    esp_sleep_enable_touchpad_wakeup();
}
void iniciar_touch_con_interrupcion_wakeup_sin_isr()
{
    //Touch iniciado por TIMER en modo interrupcion
    iniciar_touch_con_interrupcion_sin_isr();

    //CONFIGURAR LA INTERRUPCION WAKE UP  
    touch_pad_set_trigger_source(TOUCH_TRIGGER_SOURCE); //set source

    //HABILITAR COMO FUENTE DE WAKE-UP
    esp_sleep_enable_touchpad_wakeup();
}