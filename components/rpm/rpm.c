#include "rpm.h"
#define TAG "RPM"

#define TIMER_DIVIDER 16                             //  Hardware timer clock divider
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER) // convert counter value to seconds
#define TIMER_INTERVAL0_SEC (0.001)                  // sample test interval for the first timer
#define TEST_WITH_RELOAD 1                           // testing will be done without auto reload

static volatile float velocidad_rpm = 0.0;
volatile int64_t t1 = 0;
volatile int64_t numero_de_datos = 0;
volatile int64_t rpm_timer = 0;
volatile uint32_t timer_counter = 0;

static void IRAM_ATTR rpm_isr(void *arg)
{
    if (timer_counter - rpm_timer >= 3)
    {
        rpm_timer = timer_counter;
        numero_de_datos++;
        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);
        if (numero_de_datos == 1)
        {
            t1 = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
        }
        if (numero_de_datos >= 10)
        {
            t1 = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec - t1;
            velocidad_rpm = (numero_de_datos - 1) * (1000000.0 / t1) * 0.457268; // hz
            numero_de_datos = 0;
            t1 = 0;
            timer_counter = 0;
            rpm_timer = 0;
            timer_pause(TIMER_GROUP_0, TIMER_0);
            gpio_intr_disable(rpmPin);
            esp_event_isr_post("RPM_EVENT", 1, NULL, 0, NULL); //sensores.1: calcular velocidad
        }
    }
}

static void IRAM_ATTR timer_group0_isr(void *para)
{
    int timer_idx = (int)para;
    uint32_t intr_status = TIMERG0.int_st_timers.val;
    TIMERG0.hw_timer[timer_idx].update = 1;

    if ((intr_status & BIT(timer_idx)) && timer_idx == TIMER_0)
    {
        TIMERG0.int_clr_timers.t0 = 1;
    }
    TIMERG0.hw_timer[timer_idx].config.alarm_en = TIMER_ALARM_EN;

    // ACCION DE ISR
    timer_counter++;

    if (timer_counter >= 5000)
    {
        velocidad_rpm = -1.0;
        timer_pause(TIMER_GROUP_0, TIMER_0);
        gpio_intr_disable(rpmPin);
        esp_event_isr_post("RPM_EVENT", 1, NULL, 0, NULL); //sensores.1: calcular velocidad        
    }
}

static void tg0_timer_init(int timer_idx, bool auto_reload, double timer_interval_sec)
{
    /* Select and initialize basic parameters of the timer */
    timer_config_t config;
    config.divider = TIMER_DIVIDER;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en = TIMER_PAUSE;
    config.alarm_en = TIMER_ALARM_EN;
    config.intr_type = TIMER_INTR_LEVEL;
    config.auto_reload = auto_reload;
    timer_init(TIMER_GROUP_0, timer_idx, &config);

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(TIMER_GROUP_0, timer_idx, 0x00000000ULL);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(TIMER_GROUP_0, timer_idx, timer_interval_sec * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, timer_idx);
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer_group0_isr,
                       (void *)timer_idx, ESP_INTR_FLAG_IRAM, NULL);
    timer_start(TIMER_GROUP_0, timer_idx);
}

esp_err_t iniciar_rpmPin(void (*fn)(void *))
{
    ESP_LOGD(TAG, "Configurar rpmPin.");
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << rpmPin);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    if (gpio_config(&io_conf) != ESP_OK)
    {
        return ESP_FAIL;
    }
    //gpio_install_isr_service(0);
    return gpio_isr_handler_add(rpmPin, fn, NULL);
}
void medir_velocidad_ON()
{
    iniciar_rpmPin(rpm_isr);
    tg0_timer_init(TIMER_0, TEST_WITH_RELOAD, TIMER_INTERVAL0_SEC);
}
float obtener_rpm()
{
    return velocidad_rpm;
}