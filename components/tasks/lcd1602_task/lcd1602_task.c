#include "lcd1602_task.h"
#include "sync_time.h"
#include <string.h>
#include <time.h>
#include "esp_sntp.h"
#include <sys/time.h>


#define LCD_RS_PIN 16
#define LCD_E_PIN 17
#define LCD_D4_PIN 13
#define LCD_D5_PIN 14
#define LCD_D6_PIN 27
#define LCD_D7_PIN 26
#define SWITCH_PIN 22

enum display displayed_value = DATE;

static void IRAM_ATTR change_display_handler(void* args)
{
    if( MEASUREMENTS == displayed_value )
    {
        displayed_value = DATE;
    }
    else
    {
        displayed_value = MEASUREMENTS;
    }
}

void lcd16x2_task(void *args)
{
    const m_shd_mem_t *shd_memory = (const m_shd_mem_t *)args;
    m_shd_mem_t buff;

    gpio_pad_select_gpio(SWITCH_PIN);
    gpio_set_direction(SWITCH_PIN, GPIO_MODE_INPUT);
    gpio_pulldown_en(SWITCH_PIN);
    gpio_pullup_dis(SWITCH_PIN);
    gpio_set_intr_type(SWITCH_PIN, GPIO_INTR_POSEDGE);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(SWITCH_PIN, change_display_handler, NULL);

    lcd16x2_init_4bits(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);
    lcd16x2_clear();
    lcd16x2_cursorShow(false);

    while (1)
    {
        switch(displayed_value)
        {
            case MEASUREMENTS:
            {
                m_shd_mem_read(shd_memory, ALL_MEASUREMENTS, (void *)&buff);
                lcd16x2_update_measurement(&buff);
                vTaskDelay(400 / portTICK_PERIOD_MS);
                break;
            }
            case DATE:
            {
                lcd16x2_update_date();
                vTaskDelay(400 / portTICK_PERIOD_MS);
                break;
            }
        }
    }
}

void lcd16x2_update_measurement(const m_shd_mem_t *shd_ptr)
{
    lcd16x2_1stLine();
    lcd16x2_printf("DIST:%3d  WAT:%2s", (int)shd_ptr->shd_distance, hw038_return_string(shd_ptr->shd_water_level));
    lcd16x2_2ndLine();
    lcd16x2_printf("TEMP:%3d  HUM:%2d", shd_ptr->shd_temp_humid.temperature, shd_ptr->shd_temp_humid.humidity);
}


void lcd16x2_update_date()
{
    char strftime_buf[64];
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

    lcd16x2_1stLine();
    lcd16x2_printf("%.3s %d.%d.%d       ",strftime_buf,timeinfo.tm_mday,timeinfo.tm_mon+1,timeinfo.tm_year+1900);
    lcd16x2_2ndLine();
    lcd16x2_printf("    %.5s        ",strftime_buf+11);
}
