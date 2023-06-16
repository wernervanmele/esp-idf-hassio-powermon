#pragma once
#include "../main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPRINTF_BUF 200U
#define TOPIC_BUF 70U

void mqtt_pub_sensor_values(void *pv);


#ifdef __cplusplus
}
#endif
