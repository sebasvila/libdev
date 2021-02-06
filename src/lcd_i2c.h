#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <stdbool.h>
#include <i2c.h>

/* A 'lcd_t' is an abstraction of an LCD display using the HD44780
 * controller through a PCF8574 I2C expander.
 */

typedef struct {
  const uint8_t i2c_address;
  const uint8_t rows;
  uint8_t function;
  uint8_t params;
  uint8_t mode;
  volatile i2c_status_t i2c_comm;
  uint8_t r_buffer;
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
 * @param rows Number of rows of the display.
 * @return lcd_t The LCD I2C object.
 */
lcd_t lcd_constructor(const uint8_t i2c_address, const uint8_t rows);


/**
 * @brief Initializes the LCD.
 * Must be called after the 'i2c' module has been initialized.
 * According to page 45/46 from datasheet, initialitzation should be started
 * at least 40ms after Vcc rises 2.7V. A 50ms delay is recommended between 
 * power on and LCD init.
 * 
 * @param l The LCD struct variable.
 */
void lcd_init(lcd_t *l);


/**
 * @brief Clears the LCD display.
 * @param l Pointer to the LCD object
 * @param blocking `true`: the function blocks during 2ms. `false`: Just sends the command
 */
void lcd_clear(lcd_t *l, const bool blocking);


/**
 * @brief Returns both display and cursor to the first 
 * position (address 0).
 * @param l Pointer to the LCD object
 * @param blocking `true`: the function will block during 2ms. `false`: Just sends the command.
 */
void lcd_return_home(lcd_t *l, const bool blocking);


/**
 * @brief Request the LCD the busy flag status.
 * 
 * @param l Pointer to the LCD object
 */
void lcd_busy_flag_request(lcd_t *l);


/**
 * @brief Check the requested busy flag status.
 * 
 * @param l Pointer to the LCD object
 * @return true if busy flag is still set
 * @return false if busy flag is cleared 
 */
bool lcd_is_busy_flag_set(lcd_t *l);


/**
 * @brief Turns the LCD ON
 * @param l Pointer to the LCD object
 */
void lcd_on(lcd_t *l);


/**
 * @brief Turns the LCD OFF
 * @param l Pointer to the LCD object
 */
void lcd_off(lcd_t *l);


/**
 * @brief Enables the cursor blinking
 * @param l Pointer to the LCD object
 */
void lcd_enable_blinking(lcd_t *l);


/**
 * @brief Disables the cursor blinking
 * @param l Pointer to the LCD object
 */
void lcd_disable_blinking(lcd_t *l);


/**
 * @brief Enables the cursor visibility
 * @param l The LCD object
 */
void lcd_enable_cursor(lcd_t *l);


/**
 * @brief Disables the cursor visibility
 * @param l Pointer to the LCD object
 */
void lcd_disable_cursor(lcd_t *l);


/**
 * @brief Sets the scroll direction to LEFT
 * @param l Pointer to the LCD object
 */
void lcd_scroll_left(lcd_t *l);


/**
 * @brief Sets the scroll direction to RIGHT
 * @param l Pointer to the LCD object
 */
void lcd_scroll_right(lcd_t *l);


/**
 * @brief Sets the entry mode to be left to right. 
 * Means the cursor will move RIGHT at each written character.
 * @param l Pointer to the LCD object
 */
void lcd_set_left_to_right(lcd_t *l);


/**
 * @brief Sets the entry mode to be right to left. Means the cursor 
 * will move LEFT at each written character.
 * @param l Pointer to the LCD object
 */
void lcd_set_right_to_left(lcd_t *l);


/**
 * @brief Enables the autoscroll mode.
 * @param l Pointer to the LCD object
 */
void lcd_enable_autoscroll(lcd_t *l);


/**
 * @brief Disables the autoscroll mode.
 * @param l Pointer to the LCD object
 */
void lcd_disable_autoscroll(lcd_t *l);


/**
 * @brief Creates a new character. The LCD can save up to 8 
 * user-defined characters (5x8px). 
 * The lcd_move_cursor() must be called after the char 
 * creation, otherwise the cursor is pointing somewhere else.
 *
 * @param l Pointer to the LCD object
 * @param location The address to save the new character.
 * @param charmap The charmap containing the new char information
 */
void lcd_create_char(lcd_t *l, uint8_t location, uint8_t charmap[8]);


/**
 * @brief Move the LCD cursor to the desired position
 * @param l Pointer to the LCD object
 * @param col The column
 * @param row The row
 */
void lcd_move_cursor(lcd_t *l, const uint8_t col, const uint8_t row);

/**
 * @brief Prints a character to the LCD asyncronously
 * 
 * @param l Pointer to the LCD object
 * @param ch The character to be printed
 */
void lcd_print_ch(lcd_t *l, const char ch);

/**
 * @brief Prints a string to the LCD
 * The function will be blocking as soon as the i2c queue will be full.
 * Better to use the `lcd_print_ch` in a prothothreads environment or
 * increase the i2c queue array size.
 * @param l Pointer to the LCD object to send the string
 * @param string The string object to be sent
 */
void lcd_print(lcd_t *l, char *string);

#endif
