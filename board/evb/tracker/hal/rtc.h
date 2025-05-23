/*********************************************************************************************************
*/
/*============================================================================*
  *                     Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _RTC_H_
#define _RTC_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

void rtc_init(void);
size_t set_local_timestamp(size_t timestamp);
size_t get_local_timestamp(void);
time_t time (time_t *_timer);

#ifdef __cplusplus
}
#endif

#endif /* _RTC_H_ */
