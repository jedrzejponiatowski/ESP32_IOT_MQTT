#ifndef _MEASUREMENT_H_
#define _MEASUREMENT_H_

#include "GS_interface.h"
#include "shared_mem.h"

#define MEASUREMENT_PERIOD_MS 300

typedef struct measurement_args_t
{
    general_sensor_interface_t  m_interface;
    m_shd_mem_t*                m_shd_memory;
    measurement_type_t          m_type;
    void*                       m_buff;
} measurement_args_t;


void task_measurement(measurement_args_t* args);




#endif