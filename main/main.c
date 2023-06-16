#include "main.h"

/**
 * @brief ESP32 Uart configuration
 */
pzem_setup_t pzConf = {
    .pzem_uart   = UART_NUM_2,              /*  <== Specify the UART you want to use, UART_NUM_0, UART_NUM_1, UART_NUM_2 (ESP32 specific) */
    .pzem_rx_pin = GPIO_NUM_16,             /*  <== GPIO for RX */
    .pzem_tx_pin = GPIO_NUM_17,             /*  <== GPIO for TX */
    .pzem_addr   = PZ_DEFAULT_ADDRESS,      /*  If your module has a different address, specify here or update the variable in pzem004tv3.h */
};

/**
 * @brief MQTT Connection Parameters
 */
esp_mqtt_client_config_t mqttConfig = {
    .broker.address.uri = MQTT_URL,
    .credentials.username = MQTT_USER,
    .credentials.authentication.password = MQTT_PASS,
    .credentials.client_id = DEVICE_NAME,
};


/**
 * @brief FreeRTOS Parameters / settings
 */
static StaticTask_t xAutoDiscoveryTaskBuffer;
static StackType_t uxAutoDiscoveryStackTaskBuffer[ staticCREATOR_TASK_STACK_SIZE ];
static StaticTask_t xMqttPublishTaskBuffer;;
static StackType_t uxMqttPublishStackTaskBuffer[ staticCREATOR_TASK_STACK_SIZE_MQTT ];
static StaticTask_t xOledDispTaskBuffer;;
static StackType_t uxOledDispStackTaskBuffer[ staticCREATOR_TASK_STACK_SIZE_OLED ];
/* static const BaseType_t pro_cpu = 0;
static const BaseType_t app_cpu = 1; */
// --
TaskHandle_t    auto_discovery_task = NULL;
TaskHandle_t    mqtt_publish_values = NULL;
TaskHandle_t    oled_display_values = NULL;
// -- Static Queue config
QueueHandle_t   oled_queue = NULL;
StaticQueue_t   xOledTaskQueueBuffer;
#define QUEUE_LENGTH 1
#define ITEM_SIZE sizeof( _current_values_t )
uint8_t ucQueueOledTaskStorage[ QUEUE_LENGTH * ITEM_SIZE ];
/* ----------------------------------------------------- */


void app_main(void)
{
    static char *TAG = "APP_MAIN";

    PzemInit( &pzConf );            // Initialize Sensor/Uart Combo

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

    (void)connect_wifi();               // it does what it says.

    ESP_LOGI(TAG, "STARTING MQTT");

// when mdns is enabled an mDNS query will be launched to check for any mqtt services on the network
#ifdef USE_MDNS
    (void)pzem_mdns_query_mqtt(&mqttConfig);
#endif

    client = esp_mqtt_client_init(&mqttConfig);
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, client));
    ESP_ERROR_CHECK(esp_mqtt_client_start(client));

    // Create the oled display queue w static mem definition
    oled_queue = xQueueCreateStatic( QUEUE_LENGTH, ITEM_SIZE, &( ucQueueOledTaskStorage[ 0 ] ), &xOledTaskQueueBuffer );
    configASSERT( oled_queue );

    auto_discovery_task = xTaskCreateStatic(
                              AutoDiscovery_Task,
                              "Auto_Discovery",
                              staticCREATOR_TASK_STACK_SIZE,
                              NULL,
                              tskIDLE_PRIORITY,
                              &(uxAutoDiscoveryStackTaskBuffer[0]),
                              &xAutoDiscoveryTaskBuffer
                          );
    configASSERT(auto_discovery_task);

    // Pin sewrial communication Core 1 (no wifi interrupts)
    mqtt_publish_values = xTaskCreateStatic(
                              mqtt_pub_sensor_values,
                              "Publish_Values",
                              staticCREATOR_TASK_STACK_SIZE_MQTT,
                              NULL,
                              tskIDLE_PRIORITY,
                              &(uxMqttPublishStackTaskBuffer[0]),
                              &xMqttPublishTaskBuffer
                          );
    configASSERT(mqtt_publish_values);

    // Task to manage oled display
    oled_display_values = xTaskCreateStatic(
                              oled_task,
                              "Oled_Display_Task",
                              staticCREATOR_TASK_STACK_SIZE_OLED,
                              NULL,
                              tskIDLE_PRIORITY,
                              &(uxOledDispStackTaskBuffer[0]),
                              &xOledDispTaskBuffer
                          );
    configASSERT(oled_display_values);


    ESP_LOGI(TAG, "MQTT Publisher_Task is up and running\n");

    vTaskDelete(NULL);

}


