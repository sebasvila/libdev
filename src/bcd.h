#ifndef _BCD_H_
#define _BCD_H_

#include <inttypes.h>

inline uint8_t dec2bcd(uint8_t num)
{
  return ((num/10 * 16) + (num % 10));
}

inline uint8_t bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 10));
}


#endif
