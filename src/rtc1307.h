#ifndef _RTC1307
#define _RTC1307

#include <stdbool.h>

/** 
 * @brief Time object.
 * Values are usual integers. Hours are always [0..23].
 */
typedef struct {
  uint8_t s,m,h;
} rtc1307_time_t;


/**
 * @brief Start  RTC
 *
 */
void rtc1307_start(void);


/**
 * @brief Get current time
 *
 * @return An object of type rtc1307_time_t corresponding to the current time
 */
rtc1307_time_t rtc1307_get_time(void);



/** 
 * @brief Set current time
 *
 * @param ct: A rtc1307_time_t value that will be set into the clock.
 *
 */
void rtc1307_set_time(rtc1307_time_t ct);



/** 
 * @brief Setup the module
 *
 * Must be called before any other module operation.
 */
void rtc1307_setup(void);

#endif
