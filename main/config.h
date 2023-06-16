#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME             "esp32_powermon_new02"           // set the device name
#define DEVICE_HOSTNAME         "esp32-powermon-new02"           // set the device name
#define WIFI_COUNTRY            "BE"                             // set your wifi country code
#define ESP_MAXIMUM_RETRY       5
#define AUTODISCOVERY_INTERVAL  pdMS_TO_TICKS(15000U * 60U)

#ifdef USE_MDNS
#define MDNS_SERVICE (_mqtt)
#else
#define MQTT_URL    "mqtt://192.168.39.39:9999"         // MQTT Server connection string, in my case Home Assistant, mqtt credentials see credentials.h
#endif

#ifdef __cplusplus
}
#endif