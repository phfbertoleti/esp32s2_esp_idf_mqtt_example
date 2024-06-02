#ifndef HEADER_MQTT_MODULE
#define HEADER_MQTT_MODULE

/* Definitions - tópicos MQTT */
#define MQTT_TOPIC_PUB     "/led_controll/pub_topic"
#define MQTT_TOPIC_SUB     "/led_controll/sub_topic"

/* Definitions - MQTT communication */
#define QOS_LEVEL                   0
#define CALC_MSG_SIZE               0
#define FLAG_RETAIN                 0

/* Definitions - sucesso ou falha */
#define FALHA_ENVIO_MSG_MQTT       -1

/* Definition - max MQTT mesage size */
#define MQTT_MSG_MAX_SIZE      100

#endif

/* Protótipos */
void init_mqtt(void);
int publish_mqtt_message(char *pt_str_to_publish);
bool verify_MQTT_broker_connection(void);