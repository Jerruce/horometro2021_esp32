#ifndef _CUSTOM_ACCESS_POINT_
#define _CUSTOM_ACCESS_POINT_

#define ESP_WIFI_SSID "Eplimin_AP"
#define ESP_WIFI_PASS "123456789"
#define MAX_STA_CONN 4

#ifdef __cplusplus
extern "C" {
#endif

void horometro_init_softap();
void horometro_stop_softap();

#ifdef __cplusplus
}
#endif

#endif