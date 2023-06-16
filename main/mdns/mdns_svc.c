#include "mdns_svc.h"
#include "esp_netif_ip_addr.h"
#include "esp_netif.h"
#include "netdb.h"

/*
    Abraham und mustard: https://docs.espressif.com/projects/esp-protocols/mdns/docs/latest/en/index.html
*/

/**
 * @brief MDNS Initializiation, invoked via wifi callback (if mdns is enabled duh)
 */
void pzem_mdns_init()
{
    static const char *TAG = "MDNS_INIT";
#ifndef DEVICE_HOSTNAME
    char *hostname = CONFIG_LWIP_LOCAL_HOSTNAME;
#else
    char *hostname = DEVICE_HOSTNAME;
#endif

    // initialize mDNS
    ESP_ERROR_CHECK( mdns_init() );
    // set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK( mdns_hostname_set(hostname) );
    ESP_LOGI(TAG, "mdns hostname set to: [%s]", hostname);
    // set default mDNS instance name
    ESP_ERROR_CHECK( mdns_instance_name_set(DEVICE_NAME) );

}


/**
 * @brief Multicast a request on th elocal net to find a local mqtt server, otherwise used the preconfigured one
 * @param mqtt_conf
 */
void pzem_mdns_query_mqtt(esp_mqtt_client_config_t *mqtt_conf)
{
    static const char *TAG = "MDNS_QUERY";
    char pzem_mdns_mqtt_broker_url[32];

    mdns_result_t *mqtt_query;           // define response struct
    mdns_ip_addr_t *addrip = NULL;

    esp_err_t m_err = mdns_query_ptr("_mqtt", "_tcp", 3000, 1, &mqtt_query);       // fire the multicast query reuqtes for a _mqtt service on da network, we want only one result

    if (m_err) {
        ESP_LOGE(TAG, "mDNS query failed, no results found");
    } else {
        addrip = mqtt_query->addr;
        ESP_LOGI(TAG, "MQTT Service found on the network: host [%s], port: [%d], ip-address: [%d.%d.%d.%d]", mqtt_query->hostname, mqtt_query->port, IP2STR(&(addrip->addr.u_addr.ip4)));

        snprintf(pzem_mdns_mqtt_broker_url, sizeof(pzem_mdns_mqtt_broker_url), "mqtt://%d.%d.%d.%d:%d", IP2STR(&(addrip->addr.u_addr.ip4)), mqtt_query->port);
        ESP_LOGI(TAG, "Et voila le mqtt broker url: %s", pzem_mdns_mqtt_broker_url);
        // Override pre-configured url in case a mqtt service is found in the network (if mdns is enabled obviously duh).
        mqtt_conf->broker.address.uri = pzem_mdns_mqtt_broker_url;
    }
}
/*

Et voila le resultat A A a a a,

I (9233) MDNS_INIT: mdns hostname set to: [esp32-powermon-new02]
I (9233) APP_MAIN: STARTING MQTT
I (10303) wifi:<ba-add>idx:0 (ifx:0, 78:45:58:60:7e:9d), tid:6, ssn:2, winSize:64
I (10613) MDNS_QUERY: MQTT Service found on the network: host [homeassistant], port: [1883], ip-address: [192.168.28.16]
I (10613) MDNS_QUERY: Et voila le mqtt broker url: mqtt://192.168.28.16:1883

*/