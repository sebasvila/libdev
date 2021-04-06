#include <stdbool.h>
#include <stdio.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "lcd_i2c.h"


//I2C Expander pins
#define LCD_RS_PIN          0x01
#define LCD_RW_PIN          0x02
#define LCD_EN_PIN          0x04
#define LCD_BACKLIGHT_PIN   0x08

//Main instructions (page 24)
#define LCD_CLEARDISPLAY   0x01

#define LCD_RETURNHOME     0x02

#define LCD_ENTRYMODESET   0x04 
  #define LCD_ENTRYRIGHT          0x00
  #define LCD_ENTRYLEFT           0x02
  #define LCD_AUTOSCROLL_ON       0x01
  #define LCD_AUTOSCROLL_OFF      0x00  

#define LCD_DISPLAYCONTROL 0x08
  #define LCD_DISPLAYON  0x04
  #define LCD_DISPLAYOFF 0x00
  #define LCD_CURSORON   0x02
  #define LCD_CURSOROFF  0x00
  #define LCD_BLINKON    0x01
  #define LCD_BLINKOFF   0x00

#define LCD_CURSORSHIFT    0x10
  #define LCD_DISPLAYMOVE 0x08
  #define LCD_CURSORMOVE  0x00
  #define LCD_MOVERIGHT   0x04
  #define LCD_MOVELEFT    0x00

#define LCD_FUNCTIONSET    0x20
  #define LCD_8BITMODE  0x10
  #define LCD_4BITMODE  0x00
  #define LCD_2LINE     0x08
  #define LCD_1LINE     0x00
  #define LCD_5x10DOTS  0x04
  #define LCD_5x8DOTS   0x00

#define LCD_SETCGRAMADDR   0x40
#define LCD_SETDDRAMADDR   0x80

/**
 * @brief Defines the two different types of messages to the display
 */
typedef enum {
  COMMAND = 0x00,
  DATA = 0x01
} rs_mode_t;


lcd_t lcd_constructor(const uint8_t i2c_address, const uint8_t rows){
  lcd_t l = {
    i2c_address,
    rows,
    0,            //Function
    0,            //Params
    0,            //Mode
    Success,      //Last I2C request status
    0xFF,         //Recieve buffer
  };
  return l;
}


static void lcd_write_nibble(lcd_t *l, uint8_t nibble, volatile i2c_status_t * st) {
  while (i2c_swamped());
  i2c_send_uint8(l->i2c_address,
                  nibble | LCD_EN_PIN, 
                  NULL);
  while (i2c_swamped());
  i2c_send_uint8(l->i2c_address,
                  nibble & ~LCD_EN_PIN,
                  st);
}


/**
 * @brief Sends a DATA or COMMAND byte instruction to the LCD.
 * Internally, as the display is connected using 4-bit data bus over I2c,
 * the function splits the byte into two nibble-sized messages.
 *
 * @param l Pointer to the LCD object where the message must be adressed
 * @param rs Message is a `COMMAND` or `DATA`
 * @param message The byte to be sent
 */
static void lcd_send(lcd_t *l, const rs_mode_t rs, const uint8_t message) {
  while(l->i2c_comm == Running);  //If LCD is still pending on the last i2c command, function will block.
  
  uint8_t nibble = (rs | LCD_BACKLIGHT_PIN) & 0x0F;
  
  //High nibble
  lcd_write_nibble(l, (message & 0xF0) | nibble, NULL);
  
  //Low nibble
  lcd_write_nibble(l, ((message << 4) & 0xF0) | nibble, &l->i2c_comm);
}


void lcd_busy_flag_request(lcd_t *l){
  if (l->i2c_comm == Running){
    return;
  }  //If LCD is still pending on the last i2c command, function returns.

  //High nibble on HIGH to be able to read D7-D4 pins
  const uint8_t s_buffer = 0xF0 | COMMAND | LCD_BACKLIGHT_PIN | LCD_RW_PIN;

  i2c_send_uint8(l->i2c_address, s_buffer, NULL);                 //High nibble (D7-D4) on HIGH to be able to read it (PCF8574 requirement)
  i2c_send_uint8(l->i2c_address, s_buffer | LCD_EN_PIN, NULL);    //Rising edge ENABLE pulse
  i2c_receive_uint8(l->i2c_address, &l->r_buffer, &l->i2c_comm);  //Receive byte I2C request. Busy flag will be stored at the MSB on `r_buffer`
  i2c_send_uint8(l->i2c_address, s_buffer, NULL);                 //Falling edge ENABLE pulse

  //2nd ENABLE pulse is needed as LCD is working in 4-bit mode.
  i2c_send_uint8(l->i2c_address, s_buffer | LCD_EN_PIN, NULL);    //Rising edge ENABLE pulse
  i2c_send_uint8(l->i2c_address, s_buffer, NULL);                 //Falling edge ENABLE pulse
}


bool lcd_is_busy_flag_set(lcd_t *l){
  bool busy_flag = true;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    if (l->i2c_comm == Success && !(l->r_buffer >> 7)) {                      //BF is cleared
      //Received buffer to 0xFF to reset the internal BF bit
      l->r_buffer = 0xFF;
      busy_flag = false;
    }
  }
  return busy_flag;
}


void lcd_init(lcd_t *l){
	//The following seqüence is according to the Hitachi HD44780 datasheet (page 46)
	//to init the LCD into 4 bit mode

	lcd_write_nibble(l, 0x03 << 4, NULL);
	_delay_ms(5); // Wait min 4.1ms

	lcd_write_nibble(l, 0x03 << 4, NULL);
	_delay_ms(5); // Wait min 4.1ms

	lcd_write_nibble(l, 0x03 << 4, NULL);
	_delay_us(150);

	//Finally, set to 4-bit interface
	lcd_write_nibble(l, 0x02 << 4, NULL);

	//Set # lines, font size, etc.
  l->function = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	if (l->rows > 1) {
		l->function |= LCD_2LINE;
	}
  lcd_send(l, COMMAND, LCD_FUNCTIONSET | l->function);

	//Turn the display on with no cursor or blinking default
	l->params = LCD_DISPLAYOFF | LCD_CURSOROFF | LCD_BLINKOFF;
  lcd_send(l, COMMAND, LCD_DISPLAYCONTROL | l->params);

  lcd_clear(l, true);

	//Set the entry mode. Initialize to default text direction (for roman languages)
	l->mode = LCD_ENTRYLEFT | LCD_AUTOSCROLL_OFF;
	lcd_send(l, COMMAND, LCD_ENTRYMODESET | l->mode);

  lcd_on(l);
}


void lcd_return_home(lcd_t *l, const bool blocking) {
  lcd_send(l, COMMAND, LCD_RETURNHOME);
  if (blocking){
    _delay_ms(2);
  }
}


void lcd_on(lcd_t *l) {
  l->params |= LCD_DISPLAYON;
  lcd_send(l, COMMAND, LCD_DISPLAYCONTROL | l->params);
}


void lcd_off(lcd_t *l) {
  l->params &= ~LCD_DISPLAYON;
  lcd_send(l, COMMAND, LCD_DISPLAYCONTROL | l->params);
}


void lcd_clear(lcd_t *l, const bool blocking) {
  lcd_send(l, COMMAND, LCD_CLEARDISPLAY);
  if (blocking){
    _delay_ms(2);
  }
}


void lcd_enable_blinking(lcd_t *l) {
  l->params |= LCD_BLINKON;
  lcd_send(l, COMMAND, LCD_DISPLAYCONTROL | l->params);
}


void lcd_disable_blinking(lcd_t *l) {
  l->params &= ~LCD_BLINKON;
  lcd_send(l,COMMAND, LCD_DISPLAYCONTROL | l->params);
}


void lcd_enable_cursor(lcd_t *l) {
  l->params |= LCD_CURSORON;
  lcd_send(l, COMMAND, LCD_DISPLAYCONTROL | l->params);
}


void lcd_disable_cursor(lcd_t *l) {
  l->params &= ~LCD_CURSORON;
  lcd_send(l, COMMAND, LCD_DISPLAYCONTROL | l->params);
}


void lcd_scroll_left(lcd_t *l) {
  lcd_send(l, COMMAND,  LCD_CURSORSHIFT |
                            LCD_DISPLAYMOVE |
	                          LCD_MOVELEFT);
}


void lcd_scroll_right(lcd_t *l) {
  lcd_send(l, COMMAND,  LCD_CURSORSHIFT |
	                          LCD_DISPLAYMOVE | 
                            LCD_MOVERIGHT);
}


void lcd_set_left_to_right(lcd_t *l) {
  l->params |= LCD_ENTRYLEFT;
  lcd_send(l, COMMAND, LCD_ENTRYMODESET | l->params);
}


void lcd_set_right_to_left(lcd_t *l) {
  l->params &= ~LCD_ENTRYLEFT;
  lcd_send(l, COMMAND, LCD_ENTRYMODESET | l->params);
}


void lcd_enable_autoscroll(lcd_t *l) {
  l->params |= LCD_AUTOSCROLL_ON;
  lcd_send(l, COMMAND, LCD_ENTRYMODESET | l->params);
}


void lcd_disable_autoscroll(lcd_t *l) {
  l->params &= ~LCD_AUTOSCROLL_ON;
  lcd_send(l, COMMAND, LCD_ENTRYMODESET | l->params);
}


void lcd_create_char(lcd_t *l, uint8_t location, uint8_t *charmap) {
  lcd_send(l, COMMAND, (LCD_SETCGRAMADDR | ((location & 0x07) << 3)));
  for (int i = 0; i < 8; i++) {
    lcd_send(l, DATA, *charmap);
    charmap++;
  }
}


void lcd_move_cursor(lcd_t *l, uint8_t col, uint8_t row) {
  static uint8_t offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  lcd_send(l, COMMAND, LCD_SETDDRAMADDR | (col + offsets[row]));
}

void lcd_print_ch(lcd_t *l, const char ch){
  lcd_send(l, DATA, ch);
}

void lcd_print(lcd_t *l, char *string) {
  for (char *it = string; *it; it++) {
    lcd_send(l, DATA, *it);
  }
}
