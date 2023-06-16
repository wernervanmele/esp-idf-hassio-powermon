#include "hassio_mqtt_autodisco.h"

/*
 * https://github.com/DaveGamble/cJSON
 * https://cpp.hotexamples.com/examples/-/-/cJSON_CreateObject/cpp-cjson_createobject-function-examples.html
 */

/* Assemble an Hassio MQTT AutoDiscovery message */

static mqtt_auto_discovery_parameters_t auto_discovery_configs[] = {
    { "Voltage", "voltage", "V", "{{ value_json.voltage | round(1) }}" },
    { "Current", "current", "A", "{{ value_json.current | round(3) }}" },
    { "Power", "power", "W", "{{ value_json.power | round(1) }}" },
    { "Apparent Power", "apparent_power", "VA", "{{ value_json.apparent_power | round(2) }}" },
    { "Energy", "energy", "kWh", "{{ value_json.energy | round(2) }}" },
    { "Frequency", "frequency", "Hz", "{{ value_json.frequency | round(1) }}" },
    { "Power Factor", "power_factor", "", "{{ value_json.power_factor | round(2) }}" },
    { "Reactive Power", "reactive_power", "var", "{{ value_json.reactive_power | round(2) }}" },
};



void AutoDiscovery_Task( void *params )
{
    static bool wakeup_publish_task;
    static char publishToTopicBuff[ topic_buf_len ];
    static char stateTopicBuff[ topic_buf_len ];
    static uint8_t mac[ mac_addr_len ];
    static char mac_buff[31];
    static char macuniqid_buff[50];
    static mqtt_auto_discovery_parameters_t auto_discovery_config;

    wakeup_publish_task = false;

    memset( stateTopicBuff, 0, topic_buf_len );
    memset( publishToTopicBuff, 0, topic_buf_len );
    memset( macuniqid_buff, 0, sizeof( macuniqid_buff ) );

    // check how many autoDisco message we have
    size_t cfg_len = sizeof(auto_discovery_configs) / sizeof(auto_discovery_configs[0]);

    snprintf(stateTopicBuff, sizeof( stateTopicBuff ), "homeassistant/sensor/%s/state", DEVICE_NAME);

    /*  Get wifi mac address to have a unique id as identifier */
    ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA, mac));

    snprintf(mac_buff, sizeof(mac_buff), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    for (;;) {               /* @non-terminating@ */
        for (int x = 0 ; x < cfg_len ; x++) {       // Auto Disovery messages loop

            auto_discovery_config = auto_discovery_configs[x];          // loop over all autoDisco settings

            snprintf( publishToTopicBuff, sizeof( publishToTopicBuff ), "homeassistant/sensor/%s/%s/config", DEVICE_NAME, auto_discovery_config.dev_class);
            snprintf(macuniqid_buff, sizeof(macuniqid_buff), "%02x-%02x-%02x-%02x-%02x-%02x-%s", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], auto_discovery_config.dev_class);  // gen

            cJSON *autoDisco, *fmt;
            autoDisco = cJSON_CreateObject();

            cJSON_AddStringToObject( autoDisco, "name", auto_discovery_config.name );
            cJSON_AddStringToObject( autoDisco, "device_class", auto_discovery_config.dev_class );
            cJSON_AddStringToObject( autoDisco, "unit_of_measurement", auto_discovery_config.uom );
            cJSON_AddStringToObject( autoDisco, "state_topic", stateTopicBuff );
            cJSON_AddStringToObject( autoDisco, "value_template", auto_discovery_config.tpl );
            cJSON_AddStringToObject( autoDisco, "icon", "mdi:sine-wave" );
            cJSON_AddStringToObject( autoDisco, "unique_id", macuniqid_buff );

            cJSON_AddItemToObject( autoDisco, "device", fmt = cJSON_CreateObject() );
            cJSON_AddStringToObject( fmt, "name", DEVICE_NAME );
            cJSON_AddStringToObject( fmt, "manufacturer", "Wespressif" );
            cJSON_AddStringToObject( fmt, "identifiers", mac_buff );

            char *my_json_string = cJSON_Print( autoDisco );

            if ( MQTT_CONNECTED ) {

                printf("[MQTT] publish to: %s\n", publishToTopicBuff);
                ESP_LOGI( "[JSON]", "Auto DIscovery msg:\n %s\n", my_json_string );

                if (esp_mqtt_client_publish( client, publishToTopicBuff, my_json_string, 0, 0, 1 ) == -1) {
                    ESP_LOGE("[MQTT]", "Unable to publish message to MQTT Broker");
                }
            } else {
                ESP_LOGE( "[JSON]", "MQTT Not connected" );
            }

            cJSON_Delete( autoDisco );      // delete auto disco object
            cJSON_free( my_json_string );
            ESP_LOGI( "[MQTT]", "Stack High Water Mark: %ld Bytes free, running on Core: %d", ( unsigned long int ) uxTaskGetStackHighWaterMark( NULL ), xPortGetCoreID() );

            vTaskDelay(ONESEC / 2);

        }   // End Auto Discovery msgs Loop

        // Fire once and forget
        if (wakeup_publish_task == false) {
            vTaskResume(mqtt_publish_values);           // Wake-up the mqtt publish values task after all auto discoery msgs are send
            vTaskDelay(50 / portTICK_PERIOD_MS);
            vTaskResume(oled_display_values);           // WAke-up Oled display task
            wakeup_publish_task = true;
        }

        vTaskDelay(AUTODISCOVERY_INTERVAL);
    }   //  Task Loop
    vTaskDelete(NULL);
}


/* Result JSON Message

{
    "name": "Apparent Power",
    "device_class": "apparent_power",
    "unit_of_measurement":  "VA",
    "state_topic":  "homeassistant/sensor/esp32_powermon_new02/state",
    "value_template":   "{{ value_json.apparent_power | round(2) }}",
    "icon": "mdi:sine-wave",
    "device":   {
        "name": "esp32_powermon_new02",
        "manufacturer": "Wespressif",
        "identifiers":  "54:43:b2:46:17:d8"
    }
}

*/