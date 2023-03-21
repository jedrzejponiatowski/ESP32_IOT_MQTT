#ifndef __SYNC_TIME_H__
#define __SYNC_TIME_H__

#include "stdbool.h"
#include "esp_sntp.h"
#define MS_IN_HOUR  3600000

/**
 * @brief Obtain current time, UTC and also local time
 */
void obtain_time(void);

/**
 * @brief Set the current time object.
 */
void set_current_time();

/**
 * @brief Log current time 
 */
void get_current_time();


bool is_time_synchronized();

#endif