#ifndef _RGB_TASK_H_
#define _RGB_TASK_H_

#include <stdint.h>
#include "shared_mem.h"

#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (19)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO       (18)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1
#define LEDC_HS_CH2_GPIO       (21)
#define LEDC_HS_CH2_CHANNEL    LEDC_CHANNEL_2
#define RGB_TASK_PERIOD_MS     200


#define RGB_BUTTON_CONTROL     GPIO_NUM_23
#define RGB_POTEN_CHANNEL      ADC1_CHANNEL_0
#define RGB_SWITCH_PIN         GPIO_NUM_2




void rgb_task(void* args);

void rgb_init();

void rgb_update(const m_shd_mem_t* shd_ptr);

void rgb_convert_string_and_write(const char* out_buff, m_shd_mem_t* shd_ptr);

void rgb_convert_map_and_write(const uint32_t value, m_shd_mem_t* shd_ptr);

bool rgb_button_is_presed();

uint32_t rgb_read_value_pot();

void rgb_on_off(const char* buff);

char* rgb_is_on();

#endif