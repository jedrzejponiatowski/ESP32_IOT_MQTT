#ifndef _MQTT_H_
#define _MQTT_H_

#include "esp_event.h"

#define MQTT_URI "mqtt://test.mosquitto.org"

#define TOPIC_TEMP   "topic_temp"
#define TOPIC_HUMID  "topic_humid"
#define TOPIC_DIST   "topic_dist"
#define TOPIC_WATER  "topic_water"
#define TOPIC_RGB_RX "topic_rgb_rx"
#define TOPIC_RGB_TX "topic_rgb_tx"
#define TOPIC_RGB_SW "topic_rgb_sw"
#define TOPIC_RELAY  "topic_relay"

typedef void (*sub_data_handler_t)(const int topic_len, const char* topic, const int data_len, const char* data);


/**
 * @brief Set URI and init client
 */
void mqtt_app_start(sub_data_handler_t);


/**
 * @brief Subscribe to topic via URI
 */
void mqtt_subscribe(const char* topic);


/**
 * @brief Publish data through topic via URI
 */
void mqtt_publish(const char* topic, const char* data);




#endif