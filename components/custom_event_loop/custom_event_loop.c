#include "custom_event_loop.h"

#define TAG "custom event loop"

static void ip_event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data)
{

    if (event_id == IP_EVENT_AP_STAIPASSIGNED)
    {
        ESP_LOGI("IP EVENT", "ip asignado a estacion");
    }
    else if (event_id == IP_EVENT_GOT_IP6)
    {
        ESP_LOGI("IP EVENT", "ip event got ip6 ");
    }
    else
    {
        ESP_LOGI("IP EVENT", "otro ip event");
    }
}

void horometro_init_event_loop()
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler, NULL));
}