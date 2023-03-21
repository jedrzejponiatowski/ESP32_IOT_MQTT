#include "temp_humid_task.h"
#include "measurement.h"

void sensor_task_humid_temp(void* args)
{

    static temp_humid_t buff;

    measurement_args_t m_args = 
    {
        .m_interface = 
        {
            .sensor_init   = DHT11_init,
            .sensor_delete = NULL,
            .sensor_read   = DHT11_read_by_interface,
        },
        .m_type = TEMP_HUMID,
        .m_shd_memory = (m_shd_mem_t*) args,
        .m_buff = (void*)&buff,
    };

    task_measurement(&m_args);
}