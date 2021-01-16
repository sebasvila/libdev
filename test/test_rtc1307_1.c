#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "serial.h"
#include "i2c.h"
#include "rtc1307.h"


#define RTC_ADDRESS (0x68)


// setup stdout
static int write(char s, FILE *stream) {
  if (s == '\n'){
    serial_write('\r');
    serial_write('\n');
  } else serial_write(s);
  return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(write, NULL,
                                         _FDEV_SETUP_WRITE);



int main(){
  rtc1307_time_t t;
  
  serial_setup();
  i2c_setup();
  rtc1307_setup();
  sei();
  
  stdout = &mystdout;
  serial_open();
  i2c_open();
  
  puts("== begin test");
  
  // reset RTC and start it (use a literal buffer)
  rtc1307_start();
  rtc1307_set_time((rtc1307_time_t){3,5,7});

  puts("== time updated");

  for(;;) {
    t = rtc1307_get_time();
    
    // show registers in a nice way
    printf("%02d:%02d:%02d\n", t.h, t.m, t.s);
    _delay_ms(500);
  }
    
  i2c_close();
  serial_close();
  
  return 0;
}
