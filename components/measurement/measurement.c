#include "measurement.h"



void task_measurement(measurement_args_t* args)
{
    args->m_interface.sensor_init();
    
    while(1)
    {
        args->m_interface.sensor_read( args->m_buff );
        m_shd_mem_write( args->m_shd_memory,
                         args->m_type,
                         args->m_buff
        );
        vTaskDelay(MEASUREMENT_PERIOD_MS / portTICK_PERIOD_MS);
    }
}