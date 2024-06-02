#include <stdio.h>
#include <string.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

/* Includes dos módulos */
#include "wifi_st/wifi_st.h"
#include "MQTT_module/MQTT_module.h"
#include "Main_module/Main_module.h"
#include "LEDs_module/LEDs_module.h"

void app_main(void)
{   
    /* Inicializa NVS (requerido para o wifi funcionar) */
    nvs_flash_init();

    /* Inicializa LEDs da Franzininho wifi*/
    init_modulo_leds();

    /* Inicializa wifi */
    wifi_init_sta_st();

    /* Inicializa módulo principal */
    init_modulo_principal();
}
