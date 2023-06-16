#pragma once
#include "../main.h"

#ifdef __cplusplus
extern "C" {
#endif

extern esp_mqtt_client_handle_t client;

void log_error_if_nonzero(const char *message, int error_code);
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);


#ifdef __cplusplus
}
#endif