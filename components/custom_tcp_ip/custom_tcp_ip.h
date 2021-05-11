#ifndef _CUSTOM_TCP_IP_H_
#define _CUSTOM_TCP_IP_H_

#include "tcpip_adapter.h"
#include "esp_log.h"
#include "esp_system.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#ifdef __cplusplus
extern "C" {
#endif

void horometro_init_tcpip_adapter(){
    tcpip_adapter_init();
}

#ifdef __cplusplus
}
#endif

#endif