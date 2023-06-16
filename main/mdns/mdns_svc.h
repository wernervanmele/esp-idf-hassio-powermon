#pragma once
#include "../main.h"
#include "mdns.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_LWIP_CHECK_THREAD_SAFETY
#error "Run menuconfig and enable CONFIG_LWIP_CHECK_THREAD_SAFETY bro"
#endif

#ifndef CONFIG_LWIP_DNS_SUPPORT_MDNS_QUERIES
#error "Run menuconfig and enable CONFIG_LWIP_DNS_SUPPORT_MDNS_QUERIES Rudy"
#endif

void pzem_mdns_init();
void pzem_mdns_query_mqtt(esp_mqtt_client_config_t *mqtt_conf);     // query local network for a mqqt service




#ifdef __cplusplus
}
#endif