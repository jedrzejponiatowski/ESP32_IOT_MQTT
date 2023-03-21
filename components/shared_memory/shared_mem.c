#include "shared_mem.h"


#define SHD_MEM_TAG "Shared mamory"

SemaphoreHandle_t xMutex = NULL;

m_shd_mem_t* m_shd_mem_create(void)
{
    m_shd_mem_t* shd_ptr = malloc( sizeof(m_shd_mem_t));
    if (NULL == shd_ptr)
    {
        ESP_LOGE(SHD_MEM_TAG, "Can not allocate shared memory!\n");
        return NULL;
    }

    xMutex = xSemaphoreCreateMutex();
    if (NULL == xMutex)
    {
        ESP_LOGE(SHD_MEM_TAG, "Can not create mutex!\n");
        return NULL;
    }

    return shd_ptr;
}


void m_shd_mem_write(m_shd_mem_t* shd_mem, measurement_type_t m_type, void* in_buff)
{
    switch (m_type)
    {
        case TEMP_HUMID:
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            shd_mem->shd_temp_humid = *( (temp_humid_t*) in_buff);
            xSemaphoreGive(xMutex); 
            break;
        }
        case WATER:
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            shd_mem->shd_water_level = *( (water_level_t*) in_buff);
            xSemaphoreGive(xMutex); 
            break;
        }
        case DISTANCE:
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            shd_mem->shd_distance = *( (distance_t*) in_buff);
            xSemaphoreGive(xMutex); 
            break;
        }
        case RGB_LED:
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            shd_mem->shd_led_value = *( (led_val_t*) in_buff);
            xSemaphoreGive(xMutex); 
            break;
        }
        case ALL_MEASUREMENTS:
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            *shd_mem = *( (m_shd_mem_t*) in_buff);
            xSemaphoreGive(xMutex); 
            break;
        }
        default:
            break;
    }

}



void m_shd_mem_read(const m_shd_mem_t* shd_mem, measurement_type_t m_type, void* out_buff)
{
    switch (m_type)
    {
        case TEMP_HUMID:
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            *( (temp_humid_t*) out_buff ) = shd_mem->shd_temp_humid;
            xSemaphoreGive(xMutex); 
            break;
        }
        case WATER:
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            *( (water_level_t*) out_buff ) = shd_mem->shd_water_level;
            xSemaphoreGive(xMutex); 
            break;
        }
        case DISTANCE:
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            *( (distance_t*) out_buff ) = shd_mem->shd_distance;
            xSemaphoreGive(xMutex); 
            break;
        }
        case RGB_LED:
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            *( (led_val_t*) out_buff ) = shd_mem->shd_led_value;
            xSemaphoreGive(xMutex); 
            break;
        }
        case ALL_MEASUREMENTS:
        {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            *( (m_shd_mem_t*) out_buff ) = *shd_mem;
            xSemaphoreGive(xMutex); 
            break;  
        }
        default:
            break;
    }
}