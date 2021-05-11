#include "relay.h"

#define TAG "RELAY"

esp_err_t iniciar_relay()
{
    ESP_LOGD(TAG, "Configurar relay pin.");
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.pin_bit_mask = ((uint64_t)1 << relayPin);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    return gpio_config(&io_conf);
}

estado_relay_t relay_obtener_estado()
{
    return (estado_relay_t)gpio_get_level(relayPin);
}

esp_err_t relay_modificar_estado(estado_relay_t estado)
{
    return gpio_set_level(relayPin, estado);
}