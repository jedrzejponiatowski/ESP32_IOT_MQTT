/**
 * @file hcsr04.c
 * @author R2D2 2022
 * @brief Driver for HCSR04 Ultrasonic Ranging Module, please refer to datasheet https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
 */

#define LOW 0
#define HIGH 1

#include "hcsr04.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp32/rom/ets_sys.h"

#include "config.h"

#define TRIGGER_SIGNAL_US               10
#define RANGE_MEASUREMENT_TIMEOUT       60000 // [us]
#define MEASURE_DIVIDER_CM              58.8f // magic number from datasheet

#define DISTANCE_TIMEOUT_ERROR          0
#define MEASURMENT_PASSED               1
#define MEASURMENT_DONE                 0
#define MEASURMENT_ERROR                1

#define GPIO_DIST_TRIG                  CONFIG_GPIO_DISTANCE_TRIGGER
#define GPIO_DIST_ECHO                  CONFIG_GPIO_DISTANCE_ECHO

static gpio_num_t GPIO_HCSR04_TRIG;
static gpio_num_t GPIO_HCSR04_ECHO;
static uint8_t initialized = false;

static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

uint8_t hcsr04_is_initialized(void)
{
    return initialized;
}

void hcsr04_init(void)
{
    GPIO_HCSR04_TRIG = HCSR04_TRIGGER_PIN;
    GPIO_HCSR04_ECHO = HCSR04_ECHO_PIN;
    gpio_reset_pin(GPIO_HCSR04_TRIG);
    gpio_reset_pin(GPIO_HCSR04_ECHO);
    gpio_set_direction(GPIO_HCSR04_TRIG, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_HCSR04_ECHO, GPIO_MODE_INPUT);
    
    initialized = true;
}

static void hcsr04_trigger_signal(void)
{
    taskENTER_CRITICAL(&spinlock);
    
    gpio_set_level(GPIO_HCSR04_TRIG, LOW);
    ets_delay_us(2);//set LOW for 2 us just to make sure it's in low state, as sensor detects rising edge
    gpio_set_level(GPIO_HCSR04_TRIG, HIGH);
    ets_delay_us(TRIGGER_SIGNAL_US);
    gpio_set_level(GPIO_HCSR04_TRIG, LOW);

    taskEXIT_CRITICAL(&spinlock);
}

uint8_t check_distance_timeout(const uint64_t *timed_while_break)
{
    /**
     * @brief (esp_timer_get_time() < *timed_while_break) If new time is less than old time, it means that time counter overflowed and started counting from 0 again
     */
    if(((esp_timer_get_time() - *timed_while_break) > RANGE_MEASUREMENT_TIMEOUT) || (esp_timer_get_time() < *timed_while_break))
    {
        return DISTANCE_TIMEOUT_ERROR;
    }
    else
    {
        return MEASURMENT_PASSED;
    }
}

float hcsr04_get_range(void)
{
    uint8_t status = MEASURMENT_ERROR;
    uint8_t ready_to_read_rising_edge = true;
    
    uint64_t start_read_time_us = 0;
    uint64_t end_read_time_us = 0;
    uint64_t high_level_time_us = 0;
    uint64_t timed_while_break = 0;

    taskENTER_CRITICAL(&spinlock);
    
    hcsr04_trigger_signal();

    // To not wait for return signal indefinitely, function will return after 60ms with error, please refer to datasheet
    timed_while_break = esp_timer_get_time();
    while(((DISTANCE_TIMEOUT_ERROR != check_distance_timeout(&timed_while_break)) && (MEASURMENT_DONE != status)))
    {
        if((HIGH == gpio_get_level(GPIO_HCSR04_ECHO)) && (true == ready_to_read_rising_edge)) 
        {
            start_read_time_us = esp_timer_get_time();
            ready_to_read_rising_edge = false;
        }
        else if ((LOW == gpio_get_level(GPIO_HCSR04_ECHO)) && (false == ready_to_read_rising_edge))
        {
            end_read_time_us = esp_timer_get_time();
            status = MEASURMENT_DONE;
        }
    }
    // If timeouted set those two variables to 0 to return 0 as a distance.
    if(DISTANCE_TIMEOUT_ERROR == check_distance_timeout(&timed_while_break))
    {
        end_read_time_us = 0;
        start_read_time_us = 0;
    }

    taskEXIT_CRITICAL(&spinlock);

    high_level_time_us = end_read_time_us - start_read_time_us; // Calculate the width of returning pulse

    return (float)high_level_time_us / MEASURE_DIVIDER_CM; // Convert pulse witdh in us to distance in cm
}

void hcsr04_delete()
{
    gpio_reset_pin(GPIO_HCSR04_TRIG);
    gpio_reset_pin(GPIO_HCSR04_ECHO);
    initialized = false;
}

void hcsr04_read_by_interface(void* out_buff)
{
    *( (float*) out_buff ) = hcsr04_get_range();
}