#ifndef __LCD1602_H__
#define __LCD1602_H__

#include "shared_mem.h"
#include "lcd1602.h"
#include "sync_time.h"

enum display
{
    MEASUREMENTS = 0,
    DATE = 1,
};


void lcd16x2_task(void* args);

void lcd16x2_update_measurement(const m_shd_mem_t* shd_ptr);

void lcd16x2_update_date();



#endif