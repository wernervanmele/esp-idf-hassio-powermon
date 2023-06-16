#include "mqtt.h"
#include "hassio_mqtt_autodisco.h"

esp_mqtt_client_handle_t client = NULL;
uint32_t MQTT_CONNECTED = 0;

const char *MTAG = "MQTT";

void log_error_if_nonzero( const char *message,
                           int error_code )
{
    if ( error_code != 0 ) {
        ESP_LOGE( MTAG, "Last error %s: 0x%x", message, error_code );
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
void mqtt_event_handler( void *handler_args,
                         esp_event_base_t base,
                         int32_t event_id,
                         void *event_data )
{
    ESP_LOGD( MTAG, "Event dispatched from event loop base=%s, event_id=%ld", base, event_id );
    esp_mqtt_event_handle_t event = event_data;

    switch ( ( esp_mqtt_event_id_t ) event_id ) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI( "[MQTT]", "MQTT_EVENT_CONNECTED" );
        MQTT_CONNECTED = 1;

        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI( "[MQTT]", "MQTT_EVENT_DISCONNECTED" );
        MQTT_CONNECTED = 0;
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI( "[MQTT]", "MQTT_EVENT_ERROR" );
        break;

    default:
        ESP_LOGI( "[MQTT]", "Other event id:%d", event->event_id );
        break;
    }
}


