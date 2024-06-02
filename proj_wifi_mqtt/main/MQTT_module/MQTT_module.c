/* Includes */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_err.h"
#include "mqtt_client.h"
#include "MQTT_module.h"

/* Includes - other modules */
#include "../LEDs_module/LEDs_module.h"

/* Definition - debug */
#define MOD_MQTT_TAG "MOD_MQTT"

/* Static variables */
static bool is_connected_to_mqtt_broker = false;
static esp_mqtt_client_handle_t client = NULL;

/* Function: MQTT events handler
 * Params: args regarding MQTT event
 * Return: none
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(MOD_MQTT_TAG, "Event: MQTT_EVENT_CONNECTED");
        is_connected_to_mqtt_broker = true;

        /* Subscribes to MQTT topic, for LEDs controll */
        msg_id = esp_mqtt_client_subscribe(client, MQTT_TOPIC_SUB, 0);
        ESP_LOGI(MOD_MQTT_TAG, "Successfully subscribed to %s MQTT topic, msg_id=%d", MQTT_TOPIC_SUB, msg_id);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(MOD_MQTT_TAG, "Event: MQTT_EVENT_DISCONNECTED");
        is_connected_to_mqtt_broker = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(MOD_MQTT_TAG, "Event: MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(MOD_MQTT_TAG, "Event: MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(MOD_MQTT_TAG, "Event: MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(MOD_MQTT_TAG, "Event: MQTT_EVENT_DATA. A mesage has been received through MQTT.");
        ESP_LOGI(MOD_MQTT_TAG, "---------");
        ESP_LOGI(MOD_MQTT_TAG, "Topic: %.*s\r", event->topic_len, event->topic);
        ESP_LOGI(MOD_MQTT_TAG, "Payload: %.*s\r", event->data_len, event->data);
        ESP_LOGI(MOD_MQTT_TAG, "---------");

        /* Controlls LED 21 or LED 33 */
        toogle_led(event->data[0]);       

        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(MOD_MQTT_TAG, "Event: MQTT_EVENT_ERROR");
        break;

    default:
        ESP_LOGI(MOD_MQTT_TAG, "Unknown event. ID: %d", event->event_id);
        break;
    }
}

/* Function: init MQTT
 * Params: none
 * Return: none
 */
void init_mqtt(void)
{
    /* Connect to MQTT broker */
    is_connected_to_mqtt_broker = 0;
    esp_mqtt_client_config_t config_mqtt = {
         .uri = "mqtt://broker.hivemq.com:1883"
    };

    client = esp_mqtt_client_init(&config_mqtt);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

    ESP_LOGI(MOD_MQTT_TAG, "MQTT module has been initialized");
}

/* Function: publishes MQTT message
 * Params: pointer to MQTT message to be published
 * Return: none
 */
int publish_mqtt_message(char *pt_str_to_publish)
{
    char str_publish[MQTT_MSG_MAX_SIZE] = {0};
    int status_mqtt_msg_send = 0;

    /* Publish MQTT message */
    snprintf(str_publish, MQTT_MSG_MAX_SIZE, "%s", pt_str_to_publish);
    status_mqtt_msg_send = esp_mqtt_client_publish(client, MQTT_TOPIC_PUB, 
                                                           pt_str_to_publish, 
                                                           CALC_MSG_SIZE,
                                                           QOS_LEVEL,
                                                           FLAG_RETAIN);

    if (status_mqtt_msg_send == -1)
    {
        ESP_LOGE(MOD_MQTT_TAG, "Fail to send MQTT message");
    }
    else
    {
        ESP_LOGI(MOD_MQTT_TAG, "MQTT message successfully sent!");
    }

    return status_mqtt_msg_send;
}

/* Function: returns MQTT broker connection status
 * Params: none
 * Return: true: connected to MQTT broker
 *         false: not connected to MQTT broker
 */
bool verify_MQTT_broker_connection(void)
{
    return is_connected_to_mqtt_broker;
}