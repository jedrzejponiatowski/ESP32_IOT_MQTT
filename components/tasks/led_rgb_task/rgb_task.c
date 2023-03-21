#include "rgb_task.h"
#include "driver/ledc.h"
#include <string.h>
#include "driver/adc.h"
#include <time.h>
#include "esp_log.h"

#define NUMBER_OF_MEASUREMENTS 15
static bool is_on = false;

static SemaphoreHandle_t xMutex_RGB = NULL;

static uint32_t map_value(uint32_t value, uint32_t from_low, uint32_t from_high, uint32_t to_low, uint32_t to_high);

ledc_timer_config_t ledc_timer = 
    {
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz = 5000,
        .speed_mode = LEDC_HS_MODE,
        .timer_num = LEDC_HS_TIMER,
        .clk_cfg = LEDC_AUTO_CLK,
    };

ledc_channel_config_t ledc_channel[3] =
{
        {
            .channel    = LEDC_HS_CH0_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_HS_CH0_GPIO,
            .speed_mode = LEDC_HS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_HS_TIMER,
            .flags.output_invert = 0
        },
        {
            .channel    = LEDC_HS_CH1_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_HS_CH1_GPIO,
            .speed_mode = LEDC_HS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_HS_TIMER,
            .flags.output_invert = 0
        },
                {
            .channel    = LEDC_HS_CH2_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_HS_CH2_GPIO,
            .speed_mode = LEDC_HS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_HS_TIMER,
            .flags.output_invert = 0
        },
};


void rgb_task(void* args)
{
    clock_t start_push;
    clock_t end_push;
    int time_taken;
    rgb_init();

    m_shd_mem_t * shd_mem_p = (m_shd_mem_t*) args;

    while(true)
    {
        if( true == rgb_button_is_presed() )
        {
            start_push= clock();
            while(true == rgb_button_is_presed())
            {
                end_push = clock() - start_push;
                time_taken = ((int) end_push / (CLOCKS_PER_SEC / 1000));

                if(time_taken >= 1500)
                {
                    rgb_convert_map_and_write(rgb_read_value_pot(), shd_mem_p);
                    rgb_update(shd_mem_p);
                    continue;
                }
            }

            end_push = clock() - start_push;
            time_taken = ((int) end_push / (CLOCKS_PER_SEC / 1000));

            if(time_taken < 1000)
            {
                rgb_on_off( is_on ? "off" : "on");
            }
        }
        rgb_update(shd_mem_p);
        vTaskDelay(RGB_TASK_PERIOD_MS / portTICK_PERIOD_MS);
    }
}

void rgb_init()
{
    xMutex_RGB = xSemaphoreCreateMutex();
    if (NULL == xMutex_RGB)
    {
        ESP_LOGE("RGB", "Can not create mutex_rgb!\n");
        return;
    }

    ledc_timer_config(&ledc_timer);

    for (int ch = 0; ch < 3; ch++) {
        ledc_channel_config(&ledc_channel[ch]);
    }

    gpio_set_direction(RGB_BUTTON_CONTROL, GPIO_MODE_INPUT);

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(RGB_POTEN_CHANNEL, ADC_ATTEN_11db));

    gpio_set_direction(RGB_SWITCH_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(RGB_SWITCH_PIN,0);
}


void rgb_update(const m_shd_mem_t* shd_ptr)
{
    static led_val_t led_colors;
    m_shd_mem_read(shd_ptr, RGB_LED, &led_colors);

    ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, led_colors.red);
    ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);

    ledc_set_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel, led_colors.green);
    ledc_update_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel);

    ledc_set_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel, led_colors.blue);
    ledc_update_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel);

    //printf("Current colors: %d, %d, %d\n", led_colors.red, led_colors.green, led_colors.blue);
}


void rgb_convert_string_and_write(const char* out_buff, m_shd_mem_t* shd_ptr)
{
    uint8_t start_message = 5;
    char* tmp;

    static led_val_t led_buff;

    led_buff.red=atoi(out_buff+start_message);
    tmp = strchr(out_buff, ',');
    led_buff.green=atoi(tmp+1);
    tmp = strchr(tmp+1, ',');
    led_buff.blue=atoi(tmp+1);

    m_shd_mem_write(shd_ptr, RGB_LED, &led_buff);
}

void rgb_convert_map_and_write(const uint32_t value, m_shd_mem_t* shd_ptr)
{
    uint32_t rgb_val = map_value(value, 0, 4095, 0, 1535);

    static led_val_t rgb;

    if( rgb_val < 256 )
    {
        rgb.red   = 255;
        rgb.green = rgb_val;
        rgb.blue  = 0;
    }
    else if ( rgb_val < 512 )
    {
        rgb.red   = 511 - rgb_val;
        rgb.green = 255;
        rgb.blue  = 0;
    }
    else if ( rgb_val < 768 )
    {
        rgb.red   = 0;
        rgb.green = 255;
        rgb.blue  = rgb_val - 512;
    }
    else if ( rgb_val < 1024 )
    {
        rgb.red   = 0;
        rgb.green = 1023 - rgb_val;
        rgb.blue  = 255;
    }
    else if ( rgb_val < 1280 )
    {
        rgb.red   = rgb_val - 1024;
        rgb.green = 0;
        rgb.blue  = 255;
    }
    else 
    {
        rgb.red   = 255;
        rgb.green = 0;
        rgb.blue  = 1535 - rgb_val;        
    }


    m_shd_mem_write(shd_ptr, RGB_LED, &rgb);
}


bool rgb_button_is_presed()
{
    if(true == gpio_get_level(RGB_BUTTON_CONTROL))
        return true;
    else return false;
}

uint32_t rgb_read_value_pot()
{
    int sum = 0;
    for (uint8_t i = 0; i < NUMBER_OF_MEASUREMENTS; i++)
    {
        sum += adc1_get_raw(RGB_POTEN_CHANNEL);
    }
                
    return (uint32_t) sum / NUMBER_OF_MEASUREMENTS;
}


static uint32_t map_value(uint32_t value, uint32_t from_low, uint32_t from_high, 
                                          uint32_t to_low, uint32_t to_high)
{
    return (value - from_low) * (to_high - to_low) / (from_high - from_low) + to_low;
}


void rgb_on_off(const char* buff)
{
    if(0 == strcmp(buff, "on"))
    {
        gpio_set_level(RGB_SWITCH_PIN, 1);
        xSemaphoreTake(xMutex_RGB, portMAX_DELAY);
        is_on = true;
        xSemaphoreGive(xMutex_RGB);
    }
    else if (0 == strcmp(buff, "off"))
    {
        gpio_set_level(RGB_SWITCH_PIN, 0);   
        xSemaphoreTake(xMutex_RGB, portMAX_DELAY);
        is_on = false; 
        xSemaphoreGive(xMutex_RGB);    
    }
}

char* rgb_is_on()
{
    bool on_off = false;
    xSemaphoreTake(xMutex_RGB, portMAX_DELAY);
    if(is_on == true)
    {
        on_off = true;
    }
    xSemaphoreGive(xMutex_RGB);
    return (on_off ? "on" : "off");
}
