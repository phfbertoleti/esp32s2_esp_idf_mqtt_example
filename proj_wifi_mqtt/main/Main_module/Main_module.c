/* Includes */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "Main_module.h"
#include "../prio_tasks.h"
#include "../stacks_sizes.h"

/* Includes - other modules */
#include "../MQTT_module/MQTT_module.h"
#include "../LEDs_module/LEDs_module.h"

/* Definitions - debug */
#define MAIN_MODULE_TAG "MAIN_MODULE"

/* Definitions - task parametrization */
#define MAIN_MODULE_TASK_ARGUMENTS   NULL
#define MAIN_MODULE_TASK_HANDLER     NULL

/* Definition - time between seding two MQTT messages */
#define TIME_BETWEEN_2_MQTT_MESSAGES_MS           5000

/* Task function prototype */
static void main_module_task(void *arg);

/* Function: init main module
 * Params: none
 * Return: none
 */
void init_modulo_principal(void)
{
    xTaskCreate(main_module_task, "main_module",
                            MAIN_MODULE_TASK_STACK_SIZE,
                            MAIN_MODULE_TASK_ARGUMENTS,
                            PRIO_TASK_MAIN_MODULE,
                            MAIN_MODULE_TASK_HANDLER);   

    ESP_LOGI(MAIN_MODULE_TAG, "Main module has been initialized");
}

/* Function: main module task implementation
 * Params: main module task arguments
 * Return: none
 */
static void main_module_task(void *arg)
{
    char mqtt_msg_to_send[MQTT_MSG_MAX_SIZE] = {0};
    int LED_21_state = 0;
    int LED_33_state = 0;

    while(1)
    {
        if (verify_MQTT_broker_connection() == true)
        {
            /* Read LEDs states */
            LED_21_state = read_led_state(ID_LED_21);
            LED_33_state = read_led_state(ID_LED_33);

            /* Format MQTT message and send it */
            memset(mqtt_msg_to_send, 0x00, MQTT_MSG_MAX_SIZE);
            snprintf(mqtt_msg_to_send, MQTT_MSG_MAX_SIZE, "LED 21: %d  |  LED 33: %d", LED_21_state, LED_33_state);
            publish_mqtt_message(mqtt_msg_to_send);     
        }

        vTaskDelay(TIME_BETWEEN_2_MQTT_MESSAGES_MS / portTICK_PERIOD_MS);
    }
}