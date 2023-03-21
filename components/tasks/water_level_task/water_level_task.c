#include "water_level_task.h"
#include "measurement.h"



void sensor_task_water_level(void* args)
{

    static water_level_t buff;

    measurement_args_t m_args = 
    {
        .m_interface = 
        {
            .sensor_init   = hw038_init,
            .sensor_delete = NULL,
            .sensor_read   = hw038_read_by_interface,
        },
        .m_type = WATER,
        .m_shd_memory = (m_shd_mem_t*) args,
        .m_buff = (void*)&buff,
    };

    task_measurement(&m_args);
}