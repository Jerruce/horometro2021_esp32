#ifndef _CUSTOM_SPIFF_H_
#define _CUSTOM_SPIFF_H_

#include "esp_spiffs.h"
#include "esp_log.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t horometro_init_spiff_fs(void);

#ifdef __cplusplus
}
#endif

#endif