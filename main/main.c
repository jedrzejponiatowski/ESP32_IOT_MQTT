#include "relay.h"
#include "wifi.h"
#include "utilities.h"

#include "water_level_task.h"
#include "temp_humid_task.h"
#include "distance_task.h"
#include "lcd1602_task.h"
#include "shared_mem.h"
#include "mqtt_task.h"
#include "rgb_task.h"

/**
    @brief Main function of the application.

This function creates a shared memory object, and creates tasks for handling sensor data, RGB LEDs, and an LCD display. It also initializes a relay, and sets up an internet connection.

The function continuously checks if the internet connection is established and tries to connect if it is not. It also sets the current time and syncs it with an SNTP server.

@note This function runs indefinitely, and does not return. 
 */
void app_main(void)
{
    m_shd_mem_t* shared_memory = m_shd_mem_create();

    xTaskCreate(sensor_task_humid_temp,  "TASK1", 2048, (void*)shared_memory , 1, NULL);
    xTaskCreate(sensor_task_distance,    "TASK2", 2048, (void*)shared_memory , 1, NULL);
    xTaskCreate(sensor_task_water_level, "TASK3", 2048, (void*)shared_memory , 1, NULL);
    xTaskCreate(rgb_task,                "TASK4", 2048, (void*)shared_memory , 2, NULL);
    xTaskCreate(lcd16x2_task,            "TASK5", 2048, (void*)shared_memory , 2, NULL);
    relay_init(RELAY_GPIO);
    
    nvs_init();
    gain_internet_connection();

    while(false == is_wifi_connected())
    {
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_connect());
        vTaskDelay( 5000 / portTICK_PERIOD_MS);
    }
    ESP_ERROR_CHECK(esp_netif_init());
    
    xTaskCreate(mqtt_task, "MQTT_TASK", 2048, (void*)shared_memory, 3, NULL);

    while(false == is_time_synchronized())
    {
        set_current_time();
    }
    sntp_set_sync_interval(MS_IN_HOUR);
    ESP_LOGI("SNTP", "Set sync interval for : %d ms", sntp_get_sync_interval());

    


    while(1)
    {
        if(false == is_wifi_connected())
        {
            ESP_ERROR_CHECK(esp_wifi_disconnect());
            ESP_ERROR_CHECK(esp_wifi_connect());
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        continue;
    }

}
