#ifndef _DIGITAL_EPD_H_
#define _DIGITAL_EPD_H_

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1

#define digitalWrite(gpio_num, valor) gpio_set_level(gpio_num, valor)
#define pinMode(gpio_num, modo)                                              \
    {                                                                        \
        gpio_config_t io_conf;                                               \
        io_conf.intr_type = GPIO_INTR_DISABLE;                               \
        io_conf.pin_bit_mask = ((uint64_t)1 << gpio_num);                    \
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;                        \
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;                            \
        io_conf.mode = (modo == INPUT ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT); \
        gpio_config(&io_conf);                                               \
    }

#define delay(t) vTaskDelay(pdMS_TO_TICKS(t))
#define digitalRead(gpio_num) gpio_get_level(gpio_num)

#endif