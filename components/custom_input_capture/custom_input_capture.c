#include "custom_input_capture.h"

void inputCapture_iniciar(void (*isr)(void *))
{
    mcpwm_pin_config_t pin_config = {
        .mcpwm0a_out_num = -1, //Not used
        .mcpwm0b_out_num = -1, //Not used
        .mcpwm1a_out_num = -1, //Not used
        .mcpwm1b_out_num = -1, //Not used
        .mcpwm2a_out_num = -1, //Not used
        .mcpwm2b_out_num = -1, //Not used
        .mcpwm_cap0_in_num = RPM_GPIO,
        .mcpwm_cap1_in_num = -1,   //Not used,
        .mcpwm_cap2_in_num = -1,   //Not used,
        .mcpwm_sync0_in_num = -1,  //Not used
        .mcpwm_sync1_in_num = -1,  //Not used
        .mcpwm_sync2_in_num = -1,  //Not used
        .mcpwm_fault0_in_num = -1, //Not used
        .mcpwm_fault1_in_num = -1, //Not used
        .mcpwm_fault2_in_num = -1  //Not used
    };
    mcpwm_set_pin(MCPWM_UNIT_0, &pin_config);
    mcpwm_capture_enable(MCPWM_UNIT_0, MCPWM_SELECT_CAP0, MCPWM_POS_EDGE, 0); //capture signal on rising edge, pulse num = 0 i.e. 800,000,000 counts is equal to one second
    MCPWM0.int_ena.val = (CAP0_INT_EN);                                       //Enable interrupt on  CAP0 signal
    mcpwm_isr_register(MCPWM_UNIT_0, isr, NULL, ESP_INTR_FLAG_IRAM, NULL);    //Set ISR Handler
}


/*
QueueHandle_t capture_queue;
static void IRAM_ATTR input_capture_isr_handler(void *arg)
{
    uint32_t mcpwm_intr_status;
    capture evt;
    mcpwm_intr_status = MCPWM0.int_st.val; // leer interrupt status total
    if (mcpwm_intr_status & BIT(27))       // detectar flag
    {
        evt.capture_signal = horometro_captura_en_ticks();
        evt.sel_cap_signal = MCPWM_SELECT_CAP0;
        xQueueSendFromISR(capture_queue, &evt, NULL);
    }
    MCPWM0.int_clr.val = mcpwm_intr_status; // limpiar flag
}

*/