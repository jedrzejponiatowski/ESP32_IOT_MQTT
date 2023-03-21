#ifndef _HW_038_H_
#define _HW_038_H_

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_log.h"

#define WATER_SENSOR_CHANNEL ADC1_CHANNEL_6

typedef enum{
    EMPTY = 0,
    LOW,
    MEDIUM,
    HIGH,
    ERROR = -1,
} water_filled_level_t;

/**
 * @brief function that initializes the water sensor by configuring the ADC channel and setting the is_init flag to true.
 */
void hw038_init(void);

/**
 * @brief function that returns the value of the is_init flag indicating if the sensor is initialized or not.
 */
bool hw038_is_init(void);

/**
 * @brief function that reads the raw value of the water sensor by taking a specified number of measurements and averaging them. If the sensor is not initialized, an error message is logged.
 */
uint32_t hw038_read(void);

/**
 * @brief function that returns the water level based on the raw value read by hw038_read(). If the sensor is not initialized, it returns an error.
 */
water_filled_level_t hw038_read_filled_level(void);

/**
 * @brief function that fills the given buffer with the water level value read by hw038_read_filled_level().
 */
void hw038_read_by_interface(void* out_buff);

/**
 * @brief function that takes the water level as input and returns a corresponding string representation. It uses predefined strings for each level, as well as an error string.
 */
const char* hw038_return_string(water_filled_level_t level);




#endif


