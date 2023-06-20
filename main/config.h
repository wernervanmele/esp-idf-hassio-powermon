#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME             "esp32_powermon_new02"           // set the device name
#define DEVICE_HOSTNAME         "esp32-powermon-new02"           // set the device name

/*  WiFi Related Settings   */
#define WIFI_COUNTRY            "BE"                             // set your wifi country code
#define WIFI_PMF_ENABLED        false                            // required Protected Management Frames [if supported by your AP]
#define WIFI_TRESHOLD_AUTHMODE  WIFI_AUTH_WPA2_WPA3_PSK          // check esp_wifi_types.h, common settings [WIFI_AUTH_WPA2_PSK, WIFI_AUTH_WPA_WPA2_PSK, WIFI_AUTH_WPA3_PSK, WIFI_AUTH_WPA2_WPA3_PSK]
#define WIFI_SCAN_METHOD        WIFI_ALL_CHANNEL_SCAN            // WIFI_FAST_SCAN,  WIFI_ALL_CHANNEL_SCAN
#define WIFI_AP_SORT_METHOD     WIFI_CONNECT_AP_BY_SIGNAL        // [WIFI_CONNECT_AP_BY_SIGNAL, WIFI_CONNECT_AP_BY_SECURITY]
#define WIFI_CHANNEL            0                                // if the channel is unknown set it to 0 otherwise 1~13

#define ESP_MAXIMUM_RETRY       5                                // try 5 connection attempts
#define AUTODISCOVERY_INTERVAL  pdMS_TO_TICKS(15000U * 60U)

#ifdef USE_MDNS
#define MDNS_SERVICE (_mqtt_whatever_this_is_not_used_anyway)
#else
#define MQTT_URL    "mqtt://192.168.39.39:9999"         // MQTT Server connection string, in my case Home Assistant, mqtt credentials see credentials.h
#endif

#ifdef __cplusplus
}
#endif