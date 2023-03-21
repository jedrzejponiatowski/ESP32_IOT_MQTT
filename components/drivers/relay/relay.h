#ifndef __RELAY_H__
#define __RELAY_H__

#include "stdint.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "stdint.h"
#include "driver/gpio.h"
#include "stdbool.h"

#define RELAY_GPIO  GPIO_NUM_5

/**

    @brief Initializes the relay module.
    This function resets the relay GPIO pin, sets its direction as output,
    sets its initial state to low and saves the GPIO pin number for later use.
    @param gpio The GPIO pin number connected to the relay module.
*/
void relay_init(gpio_num_t gpio);

/**
    @brief Gets the current status of the relay module.
    @return True if the relay is currently turned on, false if it is turned off.
*/
bool relay_status();

/**
    @brief Switches the relay module on or off.
    This function takes a status string as an argument, and turns the relay on or off
    depending on its value.
    @param status A string containing "on" or "off" to turn the relay on or off, respectively.
*/
void switch_relay(char* status);

#endif