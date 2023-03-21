#ifndef __LCD1602_DRIVER_H__
#define __LCD1602_DRIVER_H__

#include "stdint.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "stdint.h"
#include "driver/gpio.h"

/**
 * @brief Initialise LCD on 4-bits mode
 */
void lcd16x2_init_4bits(gpio_num_t rs_pin, gpio_num_t e_pin, 
                        gpio_num_t d4_pin, gpio_num_t d5_pin, gpio_num_t d6_pin, gpio_num_t d7_pin);

/**
 * @brief Set cursor position
 * @param[in] row - 0 or 1 for line1 or line2
 * @param[in] col - 0 - 15 (16 columns LCD)
 */
void lcd16x2_setCursor(uint8_t row, uint8_t col);
/**
 * @brief Move to beginning of 1st line
 */
void lcd16x2_1stLine(void);
/**
 * @brief Move to beginning of 2nd line
 */
void lcd16x2_2ndLine(void);

/**
 * @brief Select LCD Number of lines mode
 */
void lcd16x2_twoLines(void);
void lcd16x2_oneLine(void);

/**
 * @brief Cursor ON/OFF
 */
void lcd16x2_cursorShow(bool state);

/**
 * @brief Display clear
 */
void lcd16x2_clear(void);

/**
 * @brief Display ON/OFF, to hide all characters, but not clear
 */
void lcd16x2_display(bool state);

/**
 * @brief Shift content to right
 */
void lcd16x2_shiftRight(uint8_t offset);

/**
 * @brief Shift content to left
 */
void lcd16x2_shiftLeft(uint8_t offset);

/**
 * @brief Print to display any datatype (e.g. lcd16x2_printf("Value1 = %.1f", 123.45))
 */
void lcd16x2_printf(const char* str, ...);


#endif