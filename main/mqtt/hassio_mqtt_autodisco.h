#pragma once
#include "../main.h"

#ifdef __cplusplus
extern "C" {
#endif

void AutoDiscovery_Task( void *params );

#define topic_buf_len 85U
#define mac_addr_len  6U

typedef struct mqtt_auto_discovery_parameters {
    char *name;
    char *dev_class;
    char *uom;
    char *tpl;
} mqtt_auto_discovery_parameters_t;




#ifdef __cplusplus
}
#endif