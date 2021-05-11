#ifndef _SWITCH_TECHO_H_
#define _SWITCH_TECHO_H_

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void init_switch();
    void open_switch();
    void close_switch();
    
    void init_switch_A();
    void open_switch_A();
    void close_switch_A();

#ifdef __cplusplus
}
#endif

#endif