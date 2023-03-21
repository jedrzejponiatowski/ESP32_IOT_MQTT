#ifndef _WIFI_H_
#define _WIFI_H_

#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#define WIFI_SSID                           "WIFI_SSID"
#define WIFI_PASSWORD                       "WIFI_PASS"

#define WIFI_CONNECTION_TIMEOUT_MS          5
#define WIFI_SLEEP_TIME_AFTER_MAX_RETRIES   30000

#define WIFI_CONNECTED_BIT                  BIT0
#define WIFI_FAIL_BIT                       BIT1
#define WIFI_RETRYING_BIT                   BIT2


void nvs_init();

void gain_internet_connection();

/**
 * @brief Wi-Fi Initialization. Create default wifi station.
 * 
 * @param wifi_mode WiFi operating mode
 * @param config Pointer to WiFi initialized configuration structure; can point to a temporary variable.
 */
void wifi_init(wifi_mode_t wifi_mode, wifi_init_config_t* config);


/**
 * @brief Wi-Fi connection initialization
 * 
 * @param interface interface (STA or AP)
 * @param config station or soft-AP configuration
 */
void wifi_init_connection(wifi_interface_t interface, wifi_config_t* config);


/**
 * @brief Start WiFi according to current configuration. 
 * If mode is WIFI_MODE_STA, it create station control block and start station.
 * If mode is WIFI_MODE_AP, it create soft-AP control block and start soft-AP.
 * If mode is WIFI_MODE_APSTA, it create soft-AP and station control block and start soft-AP and station.
 */
void wifi_start(void);


/**
 * @brief Stop WiFi If mode is WIFI_MODE_STA, it stop station and free station control block. 
 * If mode is WIFI_MODE_AP, it stop soft-AP and free soft-AP control block.
 * If mode is WIFI_MODE_APSTA, it stop station/soft-AP and free station/soft-AP control block
 */
void wifi_stop(void);


/**
 * @brief Unregister handlers and delete event groups 
 */
void wifi_deinit_connection(void);


/**
 * @brief Check if there is a Wi-Fi connection
 * 
 * @return true /
 * @return false 
 */
bool is_wifi_connected(void);


/**
 * @brief Set the Wi-Fi connected object
 * 
 * @param status true / false
 */
void set_wifi_connected(bool status);



#endif