/* Includes */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "LEDs_module.h"

/* Define - debug */
#define MOD_LEDS_TAG "MOD_LEDS"

/* Variáveis estáticas */
static char led_21_state = 0x00;
static char led_33_state = 0x00;

/* Function: inicializa modulo de LEDs
 * Params: none
 * Return: none
 */
void init_modulo_leds(void)
{
    gpio_config_t io_conf = {};

    /* Configure LEDs GPIOs and turn them off */
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_PINSEL_LEDS_21_33;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(GPIO_LED_21, 0);
    gpio_set_level(GPIO_LED_33, 0);
    led_21_state = 0x00;
    led_33_state = 0x00;

    ESP_LOGI(MOD_LEDS_TAG, "LEDs module has been initialized");
}

/* Function: toogle LED state
 * Params: LED id
 * Return: none
 */
void toogle_led(int id_led)
{
    switch (id_led)
    {
    case ID_LED_21:
    {
        if (led_21_state == 0x00)
        {
            led_21_state = 0x01;
        }
        else
        {
            led_21_state = 0x00;
        }

        gpio_set_level(GPIO_LED_21, (int)led_21_state);
        break;
    }

    case ID_LED_33:
    {
        if (led_33_state == 0x00)
        {
            led_33_state = 0x01;
        }
        else
        {
            led_33_state = 0x00;
        }

        gpio_set_level(GPIO_LED_33, (int)led_33_state);
        break;
    }

    default:
        break;
    }
}

/* Function: read LED state
 * Params: LED id
 * Return: 0: LED off
 *         1: LED on
 */
int read_led_state(int id_led)
{
    int estado_lido = 0;

    switch (id_led)
    {
    case ID_LED_21:
    {
        estado_lido = (int)led_21_state;
        break;
    }

    case ID_LED_33:
    {
        estado_lido = (int)led_33_state;
        break;
    }

    default:
        break;
    }

    return estado_lido;
}