/* Includes */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "wifi_st.h"

/* Includes - other modules */
#include "../MQTT_module/MQTT_module.h"

/* Definitions - debug */
#define WIFI_TAG                "WIFI"

/* Static variables */
static bool is_connected_to_wifi = false;

/* Local functions */
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static bool init_wifi_st(uint8_t * pt_ssid, uint8_t * pt_pass);

/* Function: return wi-fi network connection status
 * Params: none
 * Return: true: it's connected to wi-fi network
 *         false: it isn't connected to wi-fi network
*/
bool get_wifi_connection_status(void)
{
    return is_connected_to_wifi;
}

/* Function: wifi events callback
 * Params: events arguments
 * Return: none
*/
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        ESP_LOGI(WIFI_TAG, "Connecting to wi-fi network");
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        is_connected_to_wifi = false;

        ESP_LOGI(WIFI_TAG, "Wi-fi network has been disconented. Reconnecting...");
        esp_wifi_connect();
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        is_connected_to_wifi = true;

        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(WIFI_TAG, "Wi-fi connection has been established. IP:" IPSTR, IP2STR(&event->ip_info.ip));
        init_mqtt();
    }
    else if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(WIFI_TAG, "station "MACSTR" connected, AID=%d", MAC2STR(event->mac), event->aid);
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) 
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(WIFI_TAG, "station "MACSTR" disconnected, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

/* Function: init wifi
 * Params: none
 * Return: none
*/
void wifi_init_sta_st(void)
{
    uint8_t ssid_wifi_network[MAX_SIZE_SSID_WIFI_ST] = {0};
    uint8_t pass_wifi_network[MAX_SIZE_PASS_WIFI_ST] = {0};
    uint8_t gateway_IP[MAX_SIZE_GATEWAY_IP] = {0};
    uint8_t gateway_mask[MAX_SIZE_GATEWAY_MASK] = {0};
    bool init_wifi_st_status = true;
  
    /* Set wi-fi credentials and gateway parametrization */
    snprintf((char *)ssid_wifi_network, sizeof(ssid_wifi_network), "%s", WIFI_SSID_ST_DEFAULT);
    ESP_LOGI(WIFI_TAG, "SSID wifi: %s", ssid_wifi_network);
    
    snprintf((char *)pass_wifi_network, sizeof(pass_wifi_network), "%s", WIFI_PASS_ST_DEFAULT);
    ESP_LOGI(WIFI_TAG, "Pass wifi: %s", pass_wifi_network);
    
    snprintf((char *)gateway_IP, sizeof(gateway_IP), "%s", WIFI_ST_GATEWAY);
    ESP_LOGI(WIFI_TAG, "Gateway IP: %s", gateway_IP);
    
    snprintf((char *)gateway_mask, sizeof(gateway_mask), "%s", WIFI_ST_NETWORK_MASK);
    ESP_LOGI(WIFI_TAG, "Gateway mask: %s", gateway_mask);

    /* Init wifi in station mode */
    is_connected_to_wifi = false;
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* Register wi-fi events callback */
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    init_wifi_st_status = init_wifi_st(ssid_wifi_network, pass_wifi_network);

    if (init_wifi_st_status == true)
    {
        ESP_LOGI(WIFI_TAG, "Wi-fi OK (station)");   
    }
    else
    {
        ESP_LOGE(WIFI_TAG, "Failed to init Wi-fi (station mode)");   
    }
}

/* Function: init wifi (station mode)
 * Params: none
 * Return: true: init ok
 *         false: failed to init
*/
static bool init_wifi_st(uint8_t * pt_ssid, uint8_t * pt_pass)
{
    bool status_wifi_st = false;
    wifi_config_t wifi_config = {0};

    snprintf((char *)wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), "%s", (char *)pt_ssid);
    snprintf((char *)wifi_config.sta.password, sizeof(wifi_config.sta.password), "%s", (char *)pt_pass);
    
    esp_err_t ret_esp_set_mode = esp_wifi_set_mode(WIFI_MODE_STA);
    esp_err_t ret_esp_wifi_set_config = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_connect();
    
    if ( (ret_esp_set_mode == ESP_OK) && (ret_esp_wifi_set_config == ESP_OK) )
    {
        status_wifi_st = true;
    }
    else
    {
        status_wifi_st = false;
    }
      
    if (status_wifi_st == true)
    {
        ESP_LOGI(WIFI_TAG, "Wi-fi in station mode is OK");
    }
    else
    {
        ESP_LOGE(WIFI_TAG, "Failed to init Wi=-Fi in station mode");
    }

    return status_wifi_st;
}