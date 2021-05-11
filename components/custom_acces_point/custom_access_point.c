#include "custom_access_point.h"
#include "esp_wifi.h"
#include "string.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_private/wifi.h"
#include "custom_nvs.h"

#define TAG "custom_access_point"

void horometro_init_softap()
{
    char wifi_ssid[32];
    char wifi_pswd[32];

    if (horometro_extraer_credenciales_wifi_from_nvs(wifi_nameSpace, wifi_ssid, wifi_pswd) != ESP_OK)
    {
        sprintf(wifi_ssid, "%s", ESP_WIFI_SSID);
        sprintf(wifi_pswd, "%s", ESP_WIFI_PASS);
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            //.ssid = wifi_ssid,
            //.ssid_len = strlen((char *)wifi_ssid),
            //.password = wifi_pswd,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .max_connection = (uint8_t)1},
    };

    memcpy((void *)wifi_config.ap.ssid, wifi_ssid, strlen(wifi_ssid) + 1); // +1 for null terminated
    wifi_config.ap.ssid_len = strlen(wifi_ssid);
    memcpy((void *)wifi_config.ap.password, wifi_pswd, strlen(wifi_pswd) + 1); // +1 for null terminated

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    // esp_wifi_set_ps(WIFI_PS_NONE);
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    wifi_interface_t ifx = WIFI_IF_AP;
    ESP_ERROR_CHECK(esp_wifi_internal_set_fix_rate(ifx, true, WIFI_PHY_RATE_MCS7_SGI));

    ESP_LOGI(TAG, "SSID:%s password:%s",
             wifi_ssid, wifi_pswd);
}

void horometro_stop_softap()
{
    esp_wifi_stop();
    esp_wifi_deinit();
}