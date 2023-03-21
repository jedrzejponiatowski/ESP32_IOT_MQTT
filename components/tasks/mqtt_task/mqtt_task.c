#include "mqtt_task.h"
#include "utilities.h"
#include "shared_mem.h"
#include "mqtt.h"
#include <string.h>
#include "rgb_task.h"
#include "relay.h"

#define BUFF_SIZE 50

static m_shd_mem_t* shared_memory_p = NULL;

static void publish_shd_memory(const m_shd_mem_t* shared_memory_p);

static void received_data_handler(const int topic_len, const char* topic, const int data_len, const char* data);

static void create_rgb_mqtt_message(char* buff, led_val_t rgb);


void mqtt_task(void* args)
{ 
    shared_memory_p = (m_shd_mem_t*) args;
    char last_rgb_info[10];

    mqtt_app_start(received_data_handler);

    while(1)
    {
        vTaskDelay(PUBLISH_PERIOD_MS / portTICK_PERIOD_MS);
        publish_shd_memory(shared_memory_p);
        if(strcmp(last_rgb_info,rgb_is_on()) != 0)
        {
            strcpy(last_rgb_info, rgb_is_on());
            mqtt_publish(TOPIC_RGB_SW, rgb_is_on() );
        }
       
    }
}

static void publish_shd_memory(const m_shd_mem_t* shared_memory_p)
{
    static char out_buff[50];
    m_shd_mem_t out;
    m_shd_mem_read(shared_memory_p, ALL_MEASUREMENTS, &out);

    snprintf(out_buff, sizeof(out_buff), "%d", out.shd_temp_humid.temperature);
    mqtt_publish(TOPIC_TEMP, out_buff);

    snprintf(out_buff, sizeof(out_buff), "%d", out.shd_temp_humid.humidity);
    mqtt_publish(TOPIC_HUMID, out_buff);

    snprintf(out_buff, sizeof(out_buff), "%d", (int) out.shd_distance);
    mqtt_publish(TOPIC_DIST, out_buff);

    snprintf(out_buff, sizeof(out_buff), "%d", out.shd_water_level);
    mqtt_publish(TOPIC_WATER, out_buff);

    create_rgb_mqtt_message(out_buff, out.shd_led_value);
    mqtt_publish(TOPIC_RGB_TX, out_buff);
}

static void received_data_handler(const int topic_len, const char* topic, const int data_len, const char* data)
{
    static char buff_topic[BUFF_SIZE];
    static char buff_data[BUFF_SIZE];

    snprintf(buff_topic, sizeof(buff_topic), "%.*s", topic_len, topic);
    snprintf(buff_data,  sizeof(buff_data),  "%.*s", data_len,  data);

    if(0 == strcmp(buff_topic, TOPIC_RGB_RX))
    {
        rgb_convert_string_and_write(buff_data, shared_memory_p);
    }
    if(0 == strcmp(buff_topic, TOPIC_RGB_SW))
    {
        rgb_on_off(buff_data);
    }
    if(0 == strcmp(buff_topic, TOPIC_RELAY))
    {
        switch_relay(buff_data);
    }
}



static void create_rgb_mqtt_message(char* buff, led_val_t rgb)
{
    char* tmp;
    strcpy(buff, "RGBA(");
    snprintf(buff+5, sizeof(int), "%d", rgb.red);
    strcat(buff,",");
    tmp = strchr(buff, ',');
    snprintf(tmp+1, sizeof(int), "%d", rgb.green);
    strcat(buff,",");
    tmp = strchr(tmp+1, ',');
    snprintf(tmp+1, sizeof(int), "%d", rgb.blue);
    strcat(buff,",255)");
}
