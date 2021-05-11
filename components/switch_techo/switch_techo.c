
#include "switch_techo.h"

#define GPIO_SWITCH GPIO_NUM_2
#define GPIO_OUTPUT_PIN_SEL (1ULL << GPIO_SWITCH)

#define GPIO_SWITCH_A GPIO_NUM_4
#define GPIO_OUTPUT_PIN_SEL_A (1ULL << GPIO_SWITCH_A)

void init_switch()
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}
void open_switch()
{
    gpio_set_level(GPIO_SWITCH, 1);
}

void close_switch()
{
    gpio_set_level(GPIO_SWITCH, 0);
}

void init_switch_A()
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_A;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}
void open_switch_A()
{
    gpio_set_level(GPIO_SWITCH_A, 1);
}

void close_switch_A()
{
    gpio_set_level(GPIO_SWITCH_A, 0);
}