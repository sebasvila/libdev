#include <avr/io.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <util/delay.h>
#include <i2c.h>
#include "lcd_i2c.h"



lcd_t lcd_constructor(const uint8_t i2c_address, const uint8_t rows){
  lcd_t l = {
    i2c_address,
    rows,
    0,            //Function
    0,            //Params
    0,            //Mode
  };
  return l;
}


static void lcd_write_nibble(const lcd_t *l, uint8_t nibble) {
  while (i2c_swamped());
  i2c_send_uint8(l->i2c_address,
                  nibble | LCD_EN_PIN, 
                  NULL);

  while (i2c_swamped());
  i2c_send_uint8(l->i2c_address,
                  nibble & ~LCD_EN_PIN,
                  NULL);
}


/**
 * Sends a DATA or COMMAND byte instruction to the LCD.  Internally,
 * as the display is connected using 4-bit data bus over I2c, the function
 * splits the byte into two nibble-sized messages.
 *
 * @param l The LCD object where the message must be adressed
 * @param rs_mode Message is a LCD_COMMAND LCD_DATA
 * @param message The byte to be sent
 */
void lcd_send(const lcd_t *l, const uint8_t rs_mode, uint8_t message) {
  uint8_t nibble = (rs_mode | LCD_BACKLIGHT_PIN) & 0x0F;
  
  //High nibble
  lcd_write_nibble(l, (message & 0xF0) | nibble);
  
  //Low nibble
  lcd_write_nibble(l, ((message << 4) & 0xF0) | nibble);
}


void lcd_init(lcd_t *l){
	l->function = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (l->rows > 1) {
		l->function |= LCD_2LINE;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	_delay_ms(50);

	// Now we pull both RS and R/W low to begin commands
	i2c_send_uint8(l->i2c_address, LCD_BACKLIGHT_PIN, NULL);
	_delay_ms(1000);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	lcd_write_nibble(l, 0x03 << 4);
	_delay_ms(5); // wait min 4.1ms

	// second try
	lcd_write_nibble(l, 0x03 << 4);
	_delay_ms(5); // wait min 4.1ms

	// third go!
	lcd_write_nibble(l, 0x03 << 4);
	_delay_us(150);

	// finally, set to 4-bit interface
	lcd_write_nibble(l, 0x02 << 4);

	// set # lines, font size, etc.
  lcd_send(l, LCD_COMMAND, LCD_FUNCTIONSET | l->function);

	// turn the display on with no cursor or blinking default
	l->params = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  lcd_send(l, LCD_COMMAND, LCD_DISPLAYCONTROL | l->params);

  lcd_clear(l);

	// Initialize to default text direction (for roman languages)
	l->mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	lcd_send(l, LCD_COMMAND, LCD_ENTRYMODESET | l->mode);

	lcd_return_home(l);
}


/*
 * Turns the LCD ON
 * @param l The LCD object to turn ON
 */
void lcd_on(lcd_t *l) {
  l->params |= LCD_DISPLAYON;
  lcd_send(l, LCD_COMMAND, LCD_DISPLAYCONTROL | l->params);
}


/*
 * Turns the LCD OFF
 * @param l The LCD object
 */
void lcd_off(lcd_t *l) {
  l->params &= ~LCD_DISPLAYON;
  lcd_send(l, LCD_COMMAND, LCD_DISPLAYCONTROL | l->params);
}


/*
 * Clears the LCD display. The function takes 2ms (blocking)
 * @param l The LCD object that must be cleared
 */
void lcd_clear(const lcd_t *l) {
  lcd_send(l, LCD_COMMAND, LCD_CLEARDISPLAY);
  _delay_ms(2);   //Atention!
}




/*
 * Returns both display and cursor to the original 
 * position (address 0). The function takes 2ms (blocking)
 * @param l The LCD object
 */
void lcd_return_home(const lcd_t *l) {
  lcd_send(l, LCD_COMMAND, LCD_RETURNHOME);
  _delay_ms(2); //Atention
}


/*
 * Enables the cursor blinking
 * @param l The LCD object
 */
void lcd_enable_blinking(lcd_t *l) {
  l->params |= LCD_BLINKON;
  lcd_send(l, LCD_COMMAND, LCD_DISPLAYCONTROL | l->params);
}


/*
 * Disables the cursor blinking
 * @param l The LCD object
 */
void lcd_disable_blinking(lcd_t *l) {
  l->params &= ~LCD_BLINKON;
  lcd_send(l,LCD_COMMAND, LCD_DISPLAYCONTROL | l->params);
}


/*
 * Enables the cursor visibility
 * @param l The LCD object
 */
void lcd_enable_cursor(lcd_t *l) {
  l->params |= LCD_CURSORON;
  lcd_send(l, LCD_COMMAND, LCD_DISPLAYCONTROL | l->params);
}


/*
 * Disables the cursor visibility
 * @param l The LCD object
 */
void lcd_disable_cursor(lcd_t *l) {
  l->params &= ~LCD_CURSORON;
  lcd_send(l, LCD_COMMAND, LCD_DISPLAYCONTROL | l->params);
}


/*
 * Sets the scroll direction to LEFT
 * @param l The LCD object
 */
void lcd_scroll_left(const lcd_t *l) {
  lcd_send(l, LCD_COMMAND,  LCD_CURSORSHIFT |
                            LCD_DISPLAYMOVE |
	                          LCD_MOVELEFT);
}


/*
 * Sets the scroll direction to RIGHT
 * @param l The LCD object
 */
void lcd_scroll_right(const lcd_t *l) {
  lcd_send(l, LCD_COMMAND,  LCD_CURSORSHIFT |
	                          LCD_DISPLAYMOVE | 
                            LCD_MOVERIGHT);
}


/*
 * Sets the entry mode to be left to right. 
 * Means the cursor will move RIGHT at each written character.
 * @param l The LCD object
 */
void lcd_set_left_to_right(lcd_t *l) {
  l->params |= LCD_ENTRYLEFT;
  lcd_send(l, LCD_COMMAND, LCD_ENTRYMODESET | l->params);
}


/*
 * Sets the entry mode to be right to left. Means the cursor 
 * will move LEFT at each written character.
 * @param l The LCD object
 */
void lcd_set_right_to_left(lcd_t *l) {
  l->params &= ~LCD_ENTRYLEFT;
  lcd_send(l, LCD_COMMAND, LCD_ENTRYMODESET | l->params);
}


/*
 * Enables the autoscroll mode.
 * @param l The LCD object
 */
void lcd_enable_autoscroll(lcd_t *l) {
  l->params |= LCD_ENTRYSHIFTINCREMENT;
  lcd_send(l, LCD_COMMAND, LCD_ENTRYMODESET | l->params);
}


/*
 * Disables the autoscroll mode.
 * @param l The LCD object
 */
void lcd_disable_autoscroll(lcd_t *l) {
  l->params &= ~LCD_ENTRYSHIFTINCREMENT;
  lcd_send(l, LCD_COMMAND, LCD_ENTRYMODESET | l->params);
}


/*
 * Creates a new character. The LCD can save up to 8 
 * user-defined characters (5x8px). 
 * The lcd_move_cursor() must be called after the char 
 * creation, otherwise the cursor is pointing somewhere else.
 *
 * @param l The LCD object
 * @param location The address to save the new character.
 * @param charmap The charmap containing the new char information
 */
void lcd_create_char(const lcd_t *l, uint8_t location, uint8_t *charmap) {
  lcd_send(l, LCD_COMMAND, (LCD_SETCGRAMADDR | ((location & 0x07) << 3)));
  for (int i = 0; i < 8; i++) {
    lcd_send(l, LCD_DATA, *charmap);
    charmap++;
  }
}


/*
 * Move the LCD cursor to the desired position
 * @param l The LCD object
 * @param col The column
 * @param row The row
 */
void lcd_move_cursor(const lcd_t *l, uint8_t col, uint8_t row) {
  static uint8_t offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  lcd_send(l, LCD_COMMAND, LCD_SETDDRAMADDR | (col + offsets[row]));
}


/*
 * Sends a string to the LCD
 * @param l The LCD object to send the string
 * @param string The string object to be sent
 */
void lcd_print(const lcd_t *l, char *string) {
  for (char *it = string; *it; it++) {
    lcd_send(l, LCD_DATA, *it);
  }
}


/*
 * Prints a formated string on the LCD
 * @param l The LCD object
 * @param format The
 */
void lcd_printf(const lcd_t *l, char *format, ...) {
  va_list args;
  char lcd_buffer[LCD_COL_COUNT + 1];

  va_start(args, format);
  vsnprintf(lcd_buffer, LCD_COL_COUNT + 1, format, args);
  va_end(args);

  lcd_print(l, lcd_buffer);
}
