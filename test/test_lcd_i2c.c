#include <avr/interrupt.h>
#include <util/delay.h>
#include "i2c.h"
#include "lcd_i2c.h"

/**
 * @brief This is an example that shows the main functions
 * of an LCD based on the HD44780 driver using a PCF8574 I2C expander.
 * 
 * The functions shown are:
 *  - Print strings
 *  - Print single characters
 *  - Display clear
 *  - Return home
 *  - Move cursor to a position
 *  - Display on & off
 *  - Enable / disable cursor visibility
 *  - Enable / disable cursor blinking
 *  - Manual scroll
 *  - Auto scroll
 *  - Left to right writing
 *  - Right to left writing
 *  - Custom characters
 */


//Insert your I2C device address here:
#define LCD_I2C_ADDRESS 0x3F

//There are some different-sized LCD's based on the same HD44780 driver.
//Insert how many rows it has:
#define LCD_ROWS        4

int main(){
  lcd_t lcd = lcd_constructor(LCD_I2C_ADDRESS, LCD_ROWS);
  i2c_setup();

  sei();

  i2c_open();

  _delay_ms(50);
  lcd_init(&lcd);

  
  for(;;) {    
    //Printed strings
    lcd_clear(&lcd, true);
    lcd_print(&lcd, "Hi! This is a");
    lcd_move_cursor(&lcd, 0, 1);
    lcd_print(&lcd, "string!");

    _delay_ms(2000);


    //Clear display
    lcd_clear(&lcd, true);
    lcd_print(&lcd, "Now, I cleared");
    lcd_move_cursor(&lcd, 0, 1);
    lcd_print(&lcd, "the display");

    _delay_ms(2000);


    //Move cursor
    lcd_move_cursor(&lcd, 7, 0);
    lcd_print_ch(&lcd, 'm');
    _delay_ms(200);
    lcd_print_ch(&lcd, 'o');
    _delay_ms(200);
    lcd_print_ch(&lcd, 'v');
    _delay_ms(200);
    lcd_print_ch(&lcd, 'e');
    _delay_ms(200);
    lcd_print_ch(&lcd, 'd');
    _delay_ms(200);    
    lcd_print_ch(&lcd, ' ');
    _delay_ms(200);
    lcd_print_ch(&lcd, ' ');
    _delay_ms(200);    
    lcd_move_cursor(&lcd, 0, 1);
    lcd_print(&lcd, "the cursor.");

    _delay_ms(2000);
    

    //Return home
    lcd_return_home(&lcd, true);
    lcd_print(&lcd, "I just ");
    _delay_ms(500);
    lcd_print(&lcd, "returned");
    lcd_move_cursor(&lcd, 0, 1);
    _delay_ms(500);
    lcd_print(&lcd, "home ");    
    _delay_ms(500);
    lcd_print(&lcd, "sweet ");    
    _delay_ms(500);
    lcd_print(&lcd, "home");   

    _delay_ms(2000);


    //Right to left writing
    lcd_clear(&lcd, true);
    lcd_move_cursor(&lcd, 8, 0);
    lcd_set_right_to_left(&lcd);
    char bw[10] = "SDROWKCAB"; //BACKWORDS
    for (uint8_t i=0; bw[i] != '\0'; i++){
      lcd_print_ch(&lcd, bw[i]);
      _delay_ms(250);
    }
    lcd_set_left_to_right(&lcd);

    _delay_ms(2000);


    //Cursor display
    lcd_clear(&lcd, true);
    lcd_print(&lcd, "Let's see");
    lcd_move_cursor(&lcd, 0, 1);
    lcd_print(&lcd, "the cursor");
    _delay_ms(1000);
    lcd_enable_cursor(&lcd);

    _delay_ms(2000);


    //Cursor blinking
    lcd_clear(&lcd, true);
    lcd_print(&lcd, "Let's blink");
    lcd_move_cursor(&lcd, 0, 1);
    lcd_print(&lcd, "the cursor");
    lcd_enable_blinking(&lcd);

    _delay_ms(4000);
    lcd_disable_blinking(&lcd);
    lcd_disable_cursor(&lcd);


    //Display on & off
    lcd_clear(&lcd, true);
    lcd_print(&lcd, "Let's shut off");
    lcd_move_cursor(&lcd, 0, 1);
    lcd_print(&lcd, "the display 2s");
    _delay_ms(2000);
    lcd_off(&lcd);
    _delay_ms(2000);
    lcd_on(&lcd);

    _delay_ms(2000);


    //Manual scroll
    lcd_clear(&lcd, true);
    lcd_print(&lcd, "Manual scrolling");
    lcd_move_cursor(&lcd, 0, 1);
    lcd_print(&lcd, "LEFT  <- 5 TIMES");
    for(uint8_t i = 0; i<5; i++){
      _delay_ms(1000);
      lcd_scroll_left(&lcd);
    }
    _delay_ms(2000);
    lcd_move_cursor(&lcd, 0, 1);
    lcd_print(&lcd, "RIGHT -> 5 TIMES");
    for(uint8_t i = 0; i<5; i++){
      _delay_ms(1000);
      lcd_scroll_right(&lcd);
    }
    _delay_ms(2000);


    //Autoscrolling
    lcd_clear(&lcd, true);
    lcd_print(&lcd, " Scrolling test");
    lcd_enable_cursor(&lcd);            //In autoscroll mode, cursor should be enabled. Otherwise the autoscroll moves backwords.
    lcd_enable_autoscroll(&lcd);
    lcd_move_cursor(&lcd, 15, 1);
    char st[40] = "Moves as it types";
    for(uint8_t i = 0; st[i] != '\0'; i++) {
      lcd_print_ch(&lcd, st[i]);
      _delay_ms(500);
    }
    lcd_disable_autoscroll(&lcd);
    lcd_disable_cursor(&lcd);

    _delay_ms(2000);


    //Create char
    lcd_clear(&lcd, true);
    lcd_print(&lcd, "Let's create");
    lcd_move_cursor(&lcd, 0, 1);
    lcd_print(&lcd, "a char!");
    _delay_ms(2000);
    uint8_t U_char[8] = {
      0b11011,
      0b11011,
      0b11011,
      0b11011,
      0b11011,
      0b11011,
      0b11111,
      0b01110
    };
    uint8_t mem_index = 0x01;
    lcd_create_char(&lcd, mem_index, U_char);
    _delay_ms(1000);
    lcd_clear(&lcd, true);
    lcd_print(&lcd, "That's a bold");
    lcd_move_cursor(&lcd, 0, 1);
    lcd_print(&lcd, "custom one: ");
    lcd_print_ch(&lcd, mem_index);

    _delay_ms(4000);
  }

  i2c_close();
  
  return 0;
}
