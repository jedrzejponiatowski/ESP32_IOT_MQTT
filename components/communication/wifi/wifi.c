/**
 * @file wifi.c
 * @author R2D2 2022
 * @brief Wi-Fi managment
 */

#include "wifi.h"
#include "esp_event.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#define TAG_WIFI                    "WI-FI"
#define WIFI_CONTROL_RETRY_NUM      0

EventGroupHandle_t wifi_event_group;
static uint32_t wifi_retry_num = 1;
static esp_event_handler_instance_t instance_any_id;
static esp_event_handler_instance_t instance_got_ip;
static SemaphoreHandle_t connection_state_mutex;
static bool is_connected_to_wifi = false;

void nvs_init()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}


void gain_internet_connection()
{
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    wifi_init(WIFI_MODE_STA, &wifi_init_config);
    wifi_config_t wifi_config = 
    {
        .sta = 
        {
            .ssid               = WIFI_SSID,
            .password           = WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    wifi_init_connection(WIFI_IF_STA, &wifi_config);
    wifi_start();

    ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_connect());
    vTaskDelay( 5000 / portTICK_PERIOD_MS);
}



void set_wifi_connected(bool status)
{
    xSemaphoreTake(connection_state_mutex, portMAX_DELAY);
    is_connected_to_wifi = status;
    xSemaphoreGive(connection_state_mutex);
}

bool is_wifi_connected(void)
{
    bool is_connected_to_wifi_copy;
    xSemaphoreTake(connection_state_mutex, portMAX_DELAY);
    is_connected_to_wifi_copy = is_connected_to_wifi;
    xSemaphoreGive(connection_state_mutex);
    return is_connected_to_wifi_copy;
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if ((WIFI_EVENT == event_base) && (WIFI_EVENT_STA_DISCONNECTED == event_id))
    {
        ESP_LOGE(TAG_WIFI, "Connection to AP failed.");
        xEventGroupSetBits(wifi_event_group, WIFI_RETRYING_BIT);
        ESP_LOGI(TAG_WIFI, "%d. Retrying to connect to AP...", wifi_retry_num);

        ++wifi_retry_num;
        set_wifi_connected(false);
    }
    else if ((IP_EVENT == event_base) && (IP_EVENT_STA_GOT_IP == event_id))
    {
        ip_event_got_ip_t* ip_event_data = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_WIFI, "Got IP: " IPSTR, IP2STR(&ip_event_data->ip_info.ip));
        wifi_retry_num = 1;
        set_wifi_connected( true );
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init(wifi_mode_t wifi_mode, wifi_init_config_t* config)
{
    ESP_LOGI(TAG_WIFI, "Starting Wi-Fi initialization...");

    connection_state_mutex = xSemaphoreCreateMutex();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    if (NULL == sta_netif)
    {
        ESP_LOGE((const char*)pcTaskGetName(NULL), "Action Failure: Creating default WiFi interface station.");
    }

    ESP_ERROR_CHECK(esp_wifi_init(config));

    ESP_ERROR_CHECK(esp_wifi_set_mode(wifi_mode));

    ESP_LOGI(TAG_WIFI, "Wi-Fi initialization complete.");
}

void wifi_init_connection(wifi_interface_t interface, wifi_config_t* config)
{
    ESP_LOGI(TAG_WIFI, "Starting Wi-Fi connection initialization...");
    
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id)
    );
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip)
    );

    ESP_ERROR_CHECK(esp_wifi_set_config(interface, config));

    ESP_LOGI(TAG_WIFI, "Wi-Fi connection initialization complete.");
}

void wifi_start(void)
{
    ESP_ERROR_CHECK(esp_wifi_start());
}

void wifi_stop(void)
{
    ESP_ERROR_CHECK(esp_wifi_stop());
}

void wifi_deinit_connection(void)
{
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(wifi_event_group);
}