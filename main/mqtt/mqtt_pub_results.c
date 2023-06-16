#include "mqtt_pub_results.h"
#include <math.h>



void mqtt_pub_sensor_values(void *pv)
{

    vTaskSuspend(NULL);         // suspend task, wake-up by auto discovery
    static const char *PTAG = "[PUBL]";
    static _current_values_t pzValues;            /* Measured values */
    static char state_topic[TOPIC_BUF] = {0};
    static char bulkbuff[SPRINTF_BUF] = {0};

    // Assemble mqtt stateTopic for this device
    snprintf(state_topic, sizeof( state_topic ), "homeassistant/sensor/%s/state", DEVICE_NAME);
    static unsigned long loop_time = 0;

    for ( ;; ) {             /* @non-terminating@ */

        cJSON *payload;
        payload = cJSON_CreateObject();                 // Prepare the Json object to publish the values

        // Request sensor values
        if (!PzemGetValues( &pzConf, &pzValues )) {
            ESP_LOGE(PTAG, "Unable to retreive sensor values");
        }

        memset(bulkbuff, 0, sizeof(bulkbuff));

        if (isnan(pzValues.voltage)) {
            pzValues.voltage = 0.0f;
        } else if (isnan(pzValues.current)) {
            pzValues.current = 0.0f;
        } else if (isnan(pzValues.power)) {
            pzValues.power = 0.0f;
        } else if (isnan(pzValues.frequency)) {
            pzValues.frequency = 0.0f;
        } else if (isnan(pzValues.pf)) {
            pzValues.pf = 0.0f;
        } else {

            // assemble array with reduced floats and use as template
            snprintf(bulkbuff, sizeof(bulkbuff), "{\"voltage\":%.4f,\"current\":%.4f,\"power\":%.4f,\"frequency\":%.4f,\"power_factor\":%.4f,\"energy\":%.4f,\"apparent_power\":%.4f, \"reactive_power\":%.4f}", pzValues.voltage, pzValues.current, pzValues.power, pzValues.frequency, pzValues.pf, pzValues.energy, pzValues.apparent_power, pzValues.reactive_power);
            payload = cJSON_Parse(bulkbuff);        // JSONify the array
        }

        char *payload_msg = cJSON_Print( payload );         // Make a usable json for transmit or printing

        if (xQueueOverwrite(oled_queue, (void *) &pzValues) != pdTRUE) {
            ESP_LOGE(PTAG, "xQueueOverwrite failed to send data to queue");
        }

        if ( MQTT_CONNECTED ) {

            if ( millis() - loop_time > 60000) {
                ESP_LOGI(PTAG, "Publish every minute:\n%s\n", payload_msg);
                if (esp_mqtt_client_publish( client, state_topic, payload_msg, 0, 0, 0 ) == -1) {
                    ESP_LOGE(PTAG, "Unable to publish values to mqttbroker");
                }
                loop_time = millis();
            }

        } else {
            ESP_LOGE( PTAG, "MQTT Not connected" );
        }

        cJSON_Delete( payload );
        cJSON_free( payload_msg );

        ESP_LOGI( PTAG, "Stack High Water Mark: %ld Bytes free, running Core: %d", ( unsigned long int ) uxTaskGetStackHighWaterMark( NULL ), xPortGetCoreID() );     /* Show's what's left of the specified stacksize */
        //   esp_dump_per_task_heap_info();

        vTaskDelay( ONESEC * 3 );
    }

    vTaskDelete( NULL );
}



