#ifndef _GENERAL_SENSOR_INTERFACE_H
#define _GENERAL_SENSOR_INTERFACE_H

typedef struct general_sensor_interface_t
{
    void (*sensor_init)(void);
    void (*sensor_delete)(void);
    void (*sensor_read)(void* out_buff);
} general_sensor_interface_t;


#endif