#ifndef _UTILITIES_
#define _UTILITIES_

#include "dht11.h"
#include "hcsr04.h"
#include "hw038.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "stdint.h"

typedef struct dht11_reading     temp_humid_t;
typedef float                    distance_t;
typedef water_filled_level_t     water_level_t;

typedef struct led_val_t
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} led_val_t;


typedef enum measurement_type_t
{
    TEMP_HUMID = 1,
    WATER,
    DISTANCE,
    RGB_LED,
    ALL_MEASUREMENTS,
} measurement_type_t;



#endif