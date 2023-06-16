#pragma once
#include "../main.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int wifi_connect_status;

void connect_wifi(void);
void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);



#ifdef __cplusplus
}
#endif