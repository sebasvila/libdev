#include <stdbool.h>
#include <stdlib.h>
#include "i2c.h"
#include "bcd.h"
#include "rtc1307.h"


#define RTC_ADDRESS (0x68)


/**
 * @brief Start RTC
 */
void rtc1307_start(void) {
  uint8_t tmp;
  volatile i2c_status_t st;
  
  i2c_receive_uint8(RTC_ADDRESS, &tmp, NULL);
  tmp |= 0x80;
  i2c_send_uint8(RTC_ADDRESS, tmp, &st);
  while(!st);
}


/**
 * @brief Get current time
 *
 * @return An object of type rtc1307_time_t corresponding to the current time
 */
rtc1307_time_t rtc1307_get_time(void) {
  uint8_t buf[3];
  volatile i2c_status_t st;

  i2c_sandr(RTC_ADDRESS,
	    (uint8_t[]){0}, 1,
	    buf, 3,
	    &st);
  while(!st);

  buf[0] = bcd2dec(buf[0]);
  buf[1] = bcd2dec(buf[1]);
  buf[2] = bcd2dec(buf[2]); // assume 24h mode
  
  return (rtc1307_time_t){buf[0], buf[1], buf[2]};
}
  
	    



/** 
 * @brief Set current time
 *
 * @param ct: A rtc1307_time_t value that will be set into the clock.
 *
 */
void  rtc1307_set_time(rtc1307_time_t ct) {
  uint8_t buf[4] = {0, dec2bcd(ct.s), dec2bcd(ct.m), dec2bcd(ct.h)};
  volatile i2c_status_t st;
  
  // note that will set hours in 24h mode
  i2c_send(RTC_ADDRESS, buf, 4, &st);
  while(!st);
}  



void rtc1307_setup(void) {
}
