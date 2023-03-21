/**
 * @file dht11.c
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

#include "esp_timer.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dht11.h"

#include "config.h"

#define GPIO_HUMID          CONFIG_GPIO_HUMIDITY

// ~By Anacron-mb
static gpio_num_t dht_gpio;
static int64_t last_read_time = -2000000;
static struct dht11_reading last_read;
static uint8_t is_init = false;

/**
 * @brief 
 * ~By Anacron-mb, cosmetic changes by R2D2 team
 * @param microSeconds 
 * @param level 
 * @return uint32_t 
 */
static uint32_t _waitOrTimeout(uint16_t microSeconds, int8_t level)
{
    uint32_t micros_ticks = 0;
    while(gpio_get_level(dht_gpio) == level)
    { 
        if(micros_ticks++ > microSeconds)
        {
            return DHT11_TIMEOUT_ERROR;
        } 
            
        ets_delay_us(1);
    }
    return micros_ticks;
}

/**
 * @brief 
 * ~By Anacron-mb, cosmetic changes by R2D2 team
 * @param data 
 * @return int8_t 
 */
static int8_t _checkCRC(uint8_t data[])
{
    if(data[4] == (data[0] + data[1] + data[2] + data[3]))
    {
        return DHT11_OK;
    }
    else
    {
        return DHT11_CRC_ERROR;
    }
}

/**
 * @brief 
 * ~By Anacron-mb, cosmetic changes by R2D2 team
 */
static void _sendStartSignal()
{
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(dht_gpio, 0);
    ets_delay_us(20 * 1000);
    gpio_set_level(dht_gpio, 1);
    ets_delay_us(40);
    gpio_set_direction(dht_gpio, GPIO_MODE_INPUT);
}

/**
 * @brief 
 * ~By Anacron-mb, cosmetic changes by R2D2 team
 * @return int8_t 
 */
static int8_t _checkResponse()
{
    /* Wait for next step ~80us*/
    if(DHT11_TIMEOUT_ERROR == _waitOrTimeout(80, 0))
    {
        return DHT11_TIMEOUT_ERROR;
    }

    /* Wait for next step ~80us*/
    if(DHT11_TIMEOUT_ERROR == _waitOrTimeout(80, 1))
    {
        return DHT11_TIMEOUT_ERROR;
    } 

    return DHT11_OK;
}

/**
 * @brief 
 * ~By Anacron-mb, cosmetic changes by R2D2 team
 * @return struct dht11_reading 
 */
static struct dht11_reading _timeoutError()
{
    struct dht11_reading timeoutError = {DHT11_TIMEOUT_ERROR, -300, -1};
    return timeoutError;
}

/**
 * @brief 
 * ~By Anacron-mb, cosmetic changes by R2D2 team
 * @return struct dht11_reading 
 */
static struct dht11_reading _crcError()
{
    struct dht11_reading crcError = {DHT11_CRC_ERROR, -300, -1};
    return crcError;
}


void DHT11_init(void)
{
    /* Wait 1 seconds to make the device pass its initial unstable status */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    dht_gpio = DHT11_GPIO_PIN;
    is_init = true;
}

/**
 * @brief 
 * ~By Anacron-mb, modified by R2D2 team
 * @return struct dht11_reading 
 */
struct dht11_reading DHT11_read()
{
    /* Tried to sense too soon since last read (dht11 needs ~2 seconds to make a new read) */
    if(esp_timer_get_time() - 2000000 < last_read_time)
    {
        return last_read;
    }

    last_read_time = esp_timer_get_time();

    uint8_t data[5] = {0,0,0,0,0};

    noInterrupts();

    _sendStartSignal();

    // If device does not respond enable interrupts and return error value
    if(DHT11_TIMEOUT_ERROR == _checkResponse())
    {
        interrupts();
        return last_read = _timeoutError();
    }

    /* Read response */
    for(uint8_t i = 0; i < 40; ++i)
    {
        /* Initial data */
        // If timeouted while waiting for data enable interrupts and return error value
        if(DHT11_TIMEOUT_ERROR == _waitOrTimeout(50, 0))
        {
            interrupts();
            return last_read = _timeoutError();
        }
        if(_waitOrTimeout(70, 1) > 28)
        {
            /* Bit received was a 1 */
            data[i/8] |= (1 << (7-(i%8)));
        }
    }

    interrupts();                                              

    if(_checkCRC(data) != DHT11_CRC_ERROR)
    {
        last_read.status = DHT11_OK;
        last_read.temperature = data[2];
        last_read.humidity = data[0];
        return last_read;
    }
    else
    {
        return last_read = _crcError();
    }
}

/**
 * @brief Returns initialized state
 * ~By R2D2 team
 * @return uint8_t 
 */
uint8_t DHT11_is_initialized()
{
    return is_init;
}

/**
 * @brief Resets GPIO pin and changes initialized state to false
 * ~By R2D2 team
 */
void DHT11_delete()
{
    gpio_reset_pin(dht_gpio);
    is_init = false;
}


void DHT11_read_by_interface(void* out_buff)
{
    *( (struct dht11_reading*) out_buff ) = DHT11_read(); 
}