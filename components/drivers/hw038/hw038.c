#include "hw038.h"

#define LOG_HW038 "Sensor log: HW038 -"
#define HW038_error UINT32_MAX
#define NUMBER_OF_MEASUREMENTS 15

const char* com_empty  = "EMPTY";
const char* com_low    = "LOW";
const char* com_medium = "MEDIUM";
const char* com_high   = "HIGH";
const char* com_error  = "ERROR";


static bool is_init=0;
static water_filled_level_t value_to_level(uint32_t value);

void hw038_init(void)
{
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(WATER_SENSOR_CHANNEL, ADC_ATTEN_11db));
    is_init = true;
}

bool hw038_is_init(void)
{
    return is_init;
}

uint32_t hw038_read(void)
{
    if (true == hw038_is_init())
    {
        int sum = 0;
        for (uint8_t i = 0; i < NUMBER_OF_MEASUREMENTS; i++)
        {
            sum += adc1_get_raw(WATER_SENSOR_CHANNEL);
        }
                
        return (uint32_t) sum / NUMBER_OF_MEASUREMENTS;  
    }
    else
    {
        ESP_LOGE(LOG_HW038, "Sensor is not initialized!\n");
    }
    return HW038_error;
}


water_filled_level_t hw038_read_filled_level(void)
{
    if (false == hw038_is_init())
    {
        return ERROR;
    }
    
    return value_to_level( hw038_read() ); 
}


void hw038_read_by_interface(void* out_buff)
{
    *( (water_filled_level_t*) out_buff) = hw038_read_filled_level();
}



static water_filled_level_t value_to_level(uint32_t value)
{
    if (HW038_error == value)
    {
        return ERROR;
    }
    else
    {
        if (value < 1000)
        {
            return EMPTY;
        }
        else if (value < 1700)
        {
            return LOW;
        }
        else if (value < 1940)
        {
            return MEDIUM;
        }
        else
        {
            return HIGH;
        }
    }
}


const char* hw038_return_string(water_filled_level_t level)
{
    switch(level)
    {
        case EMPTY:
        {
            return com_empty;
        }
        case LOW:
        {
            return com_low;
        }
        case MEDIUM:
        {
            return com_medium;
        }
        case HIGH:
        {
            return com_high;
        }
        case ERROR:
        {
            return com_error;
        }
    }
        return com_error;
}

