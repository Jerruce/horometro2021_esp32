#ifndef _CUSTOM_EVENT_LOOP_H_
#define _CUSTOM_EVENT_LOOP_H_

#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_event_loop.h"
#include "esp_event.h"

#ifdef __cplusplus
extern "C" {
#endif

void horometro_init_event_loop();

#ifdef __cplusplus
}
#endif

#endif