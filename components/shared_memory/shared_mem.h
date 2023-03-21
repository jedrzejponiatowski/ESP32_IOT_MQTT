#ifndef _SHARED_MEMORY_H_
#define _SHARED_MEMORY_H_

#include "utilities.h"

typedef struct m_shd_mem_t
{
    temp_humid_t    shd_temp_humid;
    distance_t      shd_distance;
    water_level_t   shd_water_level;
    led_val_t       shd_led_value;
} m_shd_mem_t;

/**
 * @brief This function creates a shared memory object for measurements and initializes a mutex for synchronizing access to the shared memory.
 * 
 * @return m_shd_mem_t* : A pointer to the shared memory object, or NULL if the memory allocation or mutex creation fails.
 */
m_shd_mem_t* m_shd_mem_create(void);

/**
 * @brief This function writes a measurement value to the shared memory.
 * 
 * @param shd_mem : A pointer to the shared memory object.
 * @param m_type : The type of measurement to be written.
 * @param in_buff : A pointer to the measurement value to be written.
 */
void m_shd_mem_write(m_shd_mem_t* shd_mem, measurement_type_t m_type, void* in_buff);

/**
 * @brief This function reads a measurement value from the shared memory.
 * 
 * @param shd_mem : A pointer to the shared memory object.
 * @param m_type : The type of measurement to be read.
 * @param out_buff : A pointer to the buffer where the measurement value will be stored.
 */
void m_shd_mem_read(const m_shd_mem_t* shd_mem, measurement_type_t m_type, void* out_buff);


#endif