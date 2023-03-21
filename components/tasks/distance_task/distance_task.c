#include "distance_task.h"
#include "measurement.h"


void sensor_task_distance(void* args)
{
    static distance_t buff;

    measurement_args_t m_args = 
    {
        .m_interface = 
        {
            .sensor_init   = hcsr04_init,
            .sensor_delete = hcsr04_delete,
            .sensor_read   = hcsr04_read_by_interface,
        },
        .m_type =       DISTANCE,
        .m_shd_memory = (m_shd_mem_t*) args,
        .m_buff =       (void*)&buff,
    };

    task_measurement(&m_args);
}