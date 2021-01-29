#ifndef LCD_I2C_H
#define LCD_I2C_H

#pragma once

#include <avr/io.h>
#include <stdbool.h>

/* A 'lcd_t' is an abstraction of an LCD display using the HD44780
 * controller through a PCF8574 I2C expander.
 */

#define LCD_COL_COUNT 20
#define LCD_ROW_COUNT 4   //Not used

// The rest should be left alone
#define LCD_CLEARDISPLAY   0x01
#define LCD_RETURNHOME     0x02
#define LCD_ENTRYMODESET   0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT    0x10
#define LCD_FUNCTIONSET    0x20
#define LCD_SETCGRAMADDR   0x40
#define LCD_SETDDRAMADDR   0x80

#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00  

#define LCD_DISPLAYON  0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON   0x02
#define LCD_CURSOROFF  0x00
#define LCD_BLINKON    0x01
#define LCD_BLINKOFF   0x00

#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE  0x00
#define LCD_MOVERIGHT   0x04
#define LCD_MOVELEFT    0x00

#define LCD_8BITMODE  0x10
#define LCD_4BITMODE  0x00
#define LCD_2LINE     0x08
#define LCD_1LINE     0x00
#define LCD_5x10DOTS  0x04
#define LCD_5x8DOTS   0x00

#define LCD_COMMAND   0x00
#define LCD_DATA      0x01

//I2C Expander pins
#define LCD_RS_PIN          0x01
#define LCD_RW_PIN          0x02
#define LCD_EN_PIN          0x04
#define LCD_BACKLIGHT_PIN   0x08

typedef struct {
  const uint8_t i2c_address;
  const uint8_t rows;
  uint8_t function;
  uint8_t params;
  uint8_t mode;
} lcd_t;

/**
 * @brief LCD I2C Object constructor.
 * 
 * @details The I2C node address depends on the PCF8574 version and its jumpers configuration.
 * The following table resumes the different combinations:
 * | A2 | A1 | A0 | PCF8574 | PCF8574A |
 * -------------------------------------
 * | L  | L  | L  |   0x20  |   0x38   |
 * | L  | L  | H  |   0x21  |   0x39   |
 * | L  | H  | L  |   0x22  |   0x3A   |
 * | L  | H  | H  |   0x23  |   0x3B   |
 * | H  | L  | L  |   0x24  |   0x3C   |
 * | H  | L  | H  |   0x25  |   0x3D   |
 * | H  | H  | L  |   0x26  |   0x3E   |
 * | H  | H  | H  |   0x27  |   0x3F   |
 * 
 * @param i2c_address The I2C node address. 
 * @return lcd_t The LCD I2C object.
 */
lcd_t lcd_constructor(const uint8_t i2c_address, const uint8_t rows);


/**
 * @brief Initializes the LCD. Must be called after the 'i2c' module has been initialized.
 * 
 * @param l The LCD struct variable.
 */
void lcd_init(lcd_t *l);

void lcd_send(const lcd_t *l, const uint8_t rs_mode, const uint8_t value);


void lcd_on(lcd_t *l);
void lcd_off(lcd_t *l);

void lcd_clear(const lcd_t *l);
void lcd_return_home(const lcd_t *l);

void lcd_enable_blinking(lcd_t *l);
void lcd_disable_blinking(lcd_t *l);

void lcd_enable_cursor(lcd_t *l);
void lcd_disable_cursor(lcd_t *l);

void lcd_scroll_left(const lcd_t *l);
void lcd_scroll_right(const lcd_t *l);

void lcd_set_left_to_right(lcd_t *l);
void lcd_set_right_to_left(lcd_t *l);

void lcd_enable_autoscroll(lcd_t *l);
void lcd_disable_autoscroll(lcd_t *l);

void lcd_create_char(const lcd_t *l, uint8_t location, uint8_t charmap[8]);

void lcd_move_cursor(const lcd_t *l, const uint8_t col, const uint8_t row);

void lcd_print(const lcd_t *l, char *string);
void lcd_printf(const lcd_t *l, char *format, ...);

#endif
