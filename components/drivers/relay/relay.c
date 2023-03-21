#include "relay.h"
#include "string.h"

static bool relay_stat;
static gpio_num_t relay_gpio;


void relay_init(gpio_num_t gpio)
{
    gpio_reset_pin(gpio);
    gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
    relay_gpio = gpio;
    gpio_set_level(relay_gpio,0);
    relay_stat = false;
}

bool relay_status()
{
    return relay_stat;
}

void switch_relay(char* status)
{
    if(strcmp(status, "on") == 0)
    {
        relay_stat = true;
    }
    else if(strcmp(status, "off") == 0)
    {
        relay_stat = false;
    }
    
    gpio_set_level(relay_gpio, (uint32_t) relay_stat);
}