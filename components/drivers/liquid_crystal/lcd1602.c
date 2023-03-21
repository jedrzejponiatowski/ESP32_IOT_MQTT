#include "lcd1602.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

//Milisecond function
#define LCD_MS_DELAY(X) vTaskDelay( (X) / portTICK_PERIOD_MS);

/* List of COMMANDS */
#define LCD_CLEARDISPLAY      0x01
#define LCD_RETURNHOME        0x02
#define LCD_ENTRYMODESET      0x04
#define LCD_DISPLAYCONTROL    0x08
#define LCD_CURSORSHIFT       0x10
#define LCD_FUNCTIONSET       0x20
#define LCD_SETCGRAMADDR      0x40
#define LCD_SETDDRAMADDR      0x80

/* List of commands Bitfields */
//1) Entry mode Bitfields
#define LCD_ENTRY_SH          0x01
#define LCD_ENTRY_ID          0x02
//2) Entry mode Bitfields
#define LCD_ENTRY_SH          0x01
#define LCD_ENTRY_ID          0x02
//3) Display control
#define LCD_DISPLAY_B         0x01
#define LCD_DISPLAY_C         0x02
#define LCD_DISPLAY_D         0x04
//4) Shift control
#define LCD_SHIFT_RL          0x04
#define LCD_SHIFT_SC          0x08
//5) Function set control
#define LCD_FUNCTION_F        0x04
#define LCD_FUNCTION_N        0x08
#define LCD_FUNCTION_DL       0x10

/* LCD Library Variables */

static gpio_num_t PIN_RS, PIN_E;                    // RS and E pins
static gpio_num_t D4_PIN, D5_PIN, D6_PIN, D7_PIN;   // MSBs D5, D6, D7 and D8 pins
#define T_CONST   20
static uint8_t DisplayControl = 0x0F;
static uint8_t FunctionSet = 0x38;



/**
 * @brief Enable Pulse function
 */
static void lcd16x2_enablePulse(void)
{
    //HAL_GPIO_WritePin(PORT_RS_and_E, PIN_E, GPIO_PIN_SET);
    gpio_set_level(PIN_E, 1);
    //DWT_Delay_us(T_CONST);
    LCD_MS_DELAY(10);
  //HAL_GPIO_WritePin(PORT_RS_and_E, PIN_E, GPIO_PIN_RESET);
  gpio_set_level(PIN_E, 0);
  //DWT_Delay_us(60);
  //LCD_MS_DELAY(1);
}

/**
 * @brief RS control
 */
static void lcd16x2_rs(bool state)
{
  //HAL_GPIO_WritePin(PORT_RS_and_E, PIN_RS, (GPIO_PinState)state);
  gpio_set_level(PIN_RS, (uint32_t) state);
}

/**
 * @brief Write parallel signal to lcd
 */
static void lcd16x2_write(uint8_t wbyte)
{
  uint8_t LSB_nibble = wbyte&0xF, MSB_nibble = (wbyte>>4)&0xF;

    gpio_set_level(D4_PIN, (uint32_t)(MSB_nibble&0x1));
    gpio_set_level(D5_PIN, (uint32_t)(MSB_nibble&0x2));
    gpio_set_level(D6_PIN, (uint32_t)(MSB_nibble&0x4));
    gpio_set_level(D7_PIN, (uint32_t)(MSB_nibble&0x8));
    lcd16x2_enablePulse();
    //LSB data
    gpio_set_level(D4_PIN, (uint32_t)(LSB_nibble&0x1));
    gpio_set_level(D5_PIN, (uint32_t)(LSB_nibble&0x2));
    gpio_set_level(D6_PIN, (uint32_t)(LSB_nibble&0x4));
    gpio_set_level(D7_PIN, (uint32_t)(LSB_nibble&0x8));
    lcd16x2_enablePulse();
}

/**
 * @brief Write command
 */
static void lcd16x2_writeCommand(uint8_t cmd)
{
  lcd16x2_rs(false);
  lcd16x2_write(cmd);
}

/**
 * @brief Write data
 */
static void lcd16x2_writeData(uint8_t data)
{
  lcd16x2_rs(true);
  lcd16x2_write(data);
}

/**
 * @brief 4-bits write
 */
static void lcd16x2_write4(uint8_t nib)
{
  nib &= 0xF;
  lcd16x2_rs(false);
  //LSB data
    gpio_set_level(D4_PIN, (uint32_t)(nib&0x1));
    gpio_set_level(D5_PIN, (uint32_t)(nib&0x2));
    gpio_set_level(D6_PIN, (uint32_t)(nib&0x4));
    gpio_set_level(D7_PIN, (uint32_t)(nib&0x8));
  lcd16x2_enablePulse();
}


void lcd16x2_init_4bits(gpio_num_t rs_pin, gpio_num_t e_pin, 
                        gpio_num_t d4_pin, gpio_num_t d5_pin, gpio_num_t d6_pin, gpio_num_t d7_pin)
{
  //DWT_Delay_Init();
  //Set GPIO Ports and Pins data
  PIN_RS = rs_pin;
  PIN_E = e_pin;
  D4_PIN = d4_pin;
  D5_PIN = d5_pin;
  D6_PIN = d6_pin;
  D7_PIN = d7_pin;
  FunctionSet = 0x28;

  
  gpio_reset_pin(rs_pin);
  gpio_reset_pin(e_pin);
  gpio_reset_pin(d4_pin);
  gpio_reset_pin(d5_pin);
  gpio_reset_pin(d6_pin);
    gpio_reset_pin(d7_pin);
    gpio_set_direction(rs_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(e_pin,  GPIO_MODE_OUTPUT);
    gpio_set_direction(d4_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(d5_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(d6_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(d7_pin, GPIO_MODE_OUTPUT);

  

  //Initialise LCD
  //1. Wait at least 15ms
  LCD_MS_DELAY(20);
  //2. Attentions sequence
  lcd16x2_write4(0x3);
  LCD_MS_DELAY(5);
  lcd16x2_write4(0x3);
  LCD_MS_DELAY(1);
  lcd16x2_write4(0x3);
  LCD_MS_DELAY(1);
  lcd16x2_write4(0x2);  //4 bit mode
  LCD_MS_DELAY(1);
  //4. Function set; Enable 2 lines, Data length to 4 bits
  lcd16x2_writeCommand(LCD_FUNCTIONSET | LCD_FUNCTION_N);
  //3. Display control (Display ON, Cursor ON, blink cursor)
  lcd16x2_writeCommand(LCD_DISPLAYCONTROL | LCD_DISPLAY_B | LCD_DISPLAY_C | LCD_DISPLAY_D);
  //4. Clear LCD and return home
  lcd16x2_writeCommand(LCD_CLEARDISPLAY);
  LCD_MS_DELAY(3);
}

/**
 * @brief Set cursor position
 * @param[in] row - 0 or 1 for line1 or line2
 * @param[in] col - 0 - 15 (16 columns LCD)
 */
void lcd16x2_setCursor(uint8_t row, uint8_t col)
{
  uint8_t maskData;
  maskData = (col)&0x0F;
  if(row==0)
  {
    maskData |= (0x80);
    lcd16x2_writeCommand(maskData);
  }
  else
  {
    maskData |= (0xc0);
    lcd16x2_writeCommand(maskData);
  }
}
/**
 * @brief Move to beginning of 1st line
 */
void lcd16x2_1stLine(void)
{
  lcd16x2_setCursor(0,0);
}
/**
 * @brief Move to beginning of 2nd line
 */
void lcd16x2_2ndLine(void)
{
  lcd16x2_setCursor(1,0);
}

/**
 * @brief Select LCD Number of lines mode
 */
void lcd16x2_twoLines(void)
{
  FunctionSet |= (0x08);
  lcd16x2_writeCommand(FunctionSet);
}
void lcd16x2_oneLine(void)
{
  FunctionSet &= ~(0x08);
  lcd16x2_writeCommand(FunctionSet);
}

/**
 * @brief Cursor ON/OFF
 */
void lcd16x2_cursorShow(bool state)
{
  if(state)
  {
    DisplayControl |= (0x03);
    lcd16x2_writeCommand(DisplayControl);
  }
  else
  {
    DisplayControl &= ~(0x03);
    lcd16x2_writeCommand(DisplayControl);
  }
}

/**
 * @brief Display clear
 */
void lcd16x2_clear(void)
{
  lcd16x2_writeCommand(LCD_CLEARDISPLAY);
  LCD_MS_DELAY(3);
}

/**
 * @brief Display ON/OFF, to hide all characters, but not clear
 */
void lcd16x2_display(bool state)
{
  if(state)
  {
    DisplayControl |= (0x04);
    lcd16x2_writeCommand(DisplayControl);
  }
  else
  {
    DisplayControl &= ~(0x04);
    lcd16x2_writeCommand(DisplayControl);
  }
}

/**
 * @brief Shift content to right
 */
void lcd16x2_shiftRight(uint8_t offset)
{
  for(uint8_t i=0; i<offset;i++)
  {
    lcd16x2_writeCommand(0x1c);
  }
}

/**
 * @brief Shift content to left
 */
void lcd16x2_shiftLeft(uint8_t offset)
{
  for(uint8_t i=0; i<offset;i++)
  {
    lcd16x2_writeCommand(0x18);
  }
}

/**
 * @brief Print to display any datatype (e.g. lcd16x2_printf("Value1 = %.1f", 123.45))
 */
void lcd16x2_printf(const char* str, ...)
{
  char stringArray[20];
  va_list args;
  va_start(args, str);
  vsprintf(stringArray, str, args);
  va_end(args);
  for(uint8_t i=0;  i<strlen(stringArray) && i<16; i++)
  {
    lcd16x2_writeData((uint8_t)stringArray[i]);
  }
}
