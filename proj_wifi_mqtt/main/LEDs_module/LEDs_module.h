#ifndef HEADER_LEDS_MODULE
#define HEADER_LEDS_MODULE

/* Definitions - LEDs GPIOs */
#define GPIO_LED_21      21
#define GPIO_LED_33      33

/* Definitions - LEDs Ids */
#define ID_LED_21        '0'
#define ID_LED_33        '1'

/* Definition - Pinsel mask */
#define GPIO_PINSEL_LEDS_21_33    ( (1ULL<<GPIO_LED_21) | (1ULL<<GPIO_LED_33) )

#endif

/* Prototypes */
void init_modulo_leds(void);
void toogle_led(int id_led);
int read_led_state(int id_led);
