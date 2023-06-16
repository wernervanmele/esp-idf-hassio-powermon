#pragma once
/*
    ESP Stuff
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include <lwip/sys.h>
#include <lwip/api.h>

/*  Components  */
#include "pzem004tv3.h"

#include "mqtt_client.h"
#include <cJSON.h>



/*
    My Stuff
*/
#include "wifi/wifi_connect.h"
#include "mqtt/mqtt.h"
#include "mqtt/hassio_mqtt_autodisco.h"
#include "mqtt/mqtt_pub_results.h"
#include "display/oled_task.h"
#include "mdns/mdns_svc.h"
#include "config.h"
#include "credentials.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USE_MDNS    1           // put in comment to disable mDns

//extern const char *TAG;
#define ONESEC  pdMS_TO_TICKS(1000U)
#define ONEMIN  pdMS_TO_TICKS(1000U*60U)
extern uint32_t MQTT_CONNECTED;


extern TaskHandle_t    auto_discovery_task;
extern TaskHandle_t    mqtt_publish_values;
extern TaskHandle_t    oled_display_values;
extern pzem_setup_t    pzConf;
extern QueueHandle_t   oled_queue;
/*
https://sourceforge.net/p/freertos/code/HEAD/tree/trunk/FreeRTOS/Demo/Common/Minimal/StaticAllocation.c#l57
*/

#define staticCREATOR_TASK_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 3 )
#define staticCREATOR_TASK_STACK_SIZE_MQTT       ( configMINIMAL_STACK_SIZE * 4 )
#define staticCREATOR_TASK_STACK_SIZE_OLED       ( configMINIMAL_STACK_SIZE * 3 )
#define staticTASK_PRIORITY                      ( tskIDLE_PRIORITY + 2 )
#define staticDONT_BLOCK                         ( ( TickType_t ) 0 )


#ifdef __cplusplus
}
#endif