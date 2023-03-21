idf_component_register(SRC_DIRS "." "temp_humid_task" "water_level_task" "distance_task" "mqtt_task" "led_rgb_task" "lcd1602_task"
                       INCLUDE_DIRS "." "temp_humid_task" "water_level_task" "distance_task" "mqtt_task" "led_rgb_task" "lcd1602_task"
                       REQUIRES "drivers" "communication" "measurement" "shared_memory" "utilities"
)
