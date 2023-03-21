/**
 * @file dht11.h
 * @author R2D2 2022
 * @brief Humidity & Temperature sensor
 */


/*
 *~R2D2  - driver was partially modified by R2D2 team, also please refer to DHT11 datasheet https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf
 * Driver code source: https://github.com/Anacron-mb/esp32-DHT11
 * MIT License
 * 
 * Copyright (c) 2018 Michele Biondi
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

/**
 * datasheet: https://components101.com/sensors/dht11-temperature-sensor
 * 
 */

#ifndef DHT11_H_
#define DHT11_H_

#include "driver/gpio.h"


#define noInterrupts() portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;taskENTER_CRITICAL(&mux)
#define interrupts() taskEXIT_CRITICAL(&mux)                                                    

#define DHT11_GPIO_PIN GPIO_NUM_25

enum dht11_status {
    DHT11_CRC_ERROR = -2,
    DHT11_TIMEOUT_ERROR,
    DHT11_OK
};

/**
 * @brief Structure of read values
 */
struct dht11_reading {
    int32_t status;
    int32_t temperature;
    uint32_t humidity;
};

/**
 * @brief Initialization the GPIO pin of sensor.
 * 
 * @param gpio_num Data GPIO pin 
 */
void DHT11_init(void);

/**
 * @brief Reads values of sensor.
 * 
 * Humidity range:      20-90 %RH.
 * Temperature range:   0-50  *C.
 * If temperature is -300 OR humidity is -1 : error occurred.
 * 
 * @return struct dht11_reading - Structure with readed data
 */
struct dht11_reading DHT11_read();


/**
 * @brief Check if sensor is initialized.
 * 
 * @return uint8_t true/false.
 */
uint8_t DHT11_is_initialized();


/**
 * @brief Reset GPIO pins used by driver
 */
void DHT11_delete();


void DHT11_read_by_interface(void* out_buff);


#endif