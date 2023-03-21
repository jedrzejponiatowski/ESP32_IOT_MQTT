/**
 * Datasheet: https://pdf1.alldatasheet.com/datasheet-pdf/view/1132203/ETC2/HC-SR04.html
 * 
 * Formula: uS / 58 = centimeters or uS / 148 =inch; or: 
 * the range = high level time * velocity (340M/S) / 2; 
 * we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal. 
 */

#ifndef HCSR04_H
#define HCSR04_H

#include "driver/gpio.h"

#define HCSR04_TRIGGER_PIN    GPIO_NUM_32
#define HCSR04_ECHO_PIN       GPIO_NUM_33

/**
 * @brief Check if sensor is initialized.
 * 
 * @return uint8_t - TRUE / FALSE
 */
uint8_t hcsr04_is_initialized(void);



void hcsr04_init(void);


/**
 * @brief Checks if the waiting time has not been exceeded.
 * 
 * @param timed_while_break Start of the waiting period.
 * @return uint8_t - TRUE / FALSE
 */
uint8_t check_distance_timeout(const uint64_t *timed_while_break);


/**
 * @brief Sensor reading.
 * 
 * Is entering the critical.
 * Trigger measurement and is waiting for echo.
 * Before return range is devided by 58.8 (return value in cm)
 * 
 * @return float - Range (cm) between sensor and obstacle. If 0 - out of range
 */
float hcsr04_get_range(void);


/**
 * @brief Reset GPIO pins used by driver and change initialized state to false.
 */
void hcsr04_delete();

void hcsr04_read_by_interface(void* out_buff);

#endif