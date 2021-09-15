/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <string.h>
#include <time.h>

#include "rtl876x_nvic.h"
#include "rtl876x_rcc.h"
#include "rtl876x_rtc.h"
#include "os_sync.h"
#include "os_sched.h"
#include "custom_log.h"
#include "utils_timer.h"
#include "rtc.h"

static size_t rtc_timestamp = 1631515431;//1630425600; //2021-09-01 00:00:00
static void *p_rtc_mutex = NULL;
static RTC_DATE_TIME rtc_data_time;

#define RTC_LOCK(x) {if(x) os_mutex_take(x, 0xFFFFFFFF);}
#define RTC_UNLOCK(x) {if(x) os_mutex_give(x);}

void rtc_init(void)
{
	if (os_mutex_create(&p_rtc_mutex) != true)
    {
		LOG_E("rtc mutex create faile!!!");
		return;
    }
	
	time(NULL);
}

size_t set_local_timestamp(size_t timestamp)
{
	RTC_LOCK(p_rtc_mutex);
	rtc_timestamp = (timestamp - os_sys_tick_get()/100);
	RTC_UNLOCK(p_rtc_mutex);
	return rtc_timestamp;
}

size_t get_local_timestamp(void)
{
	return (rtc_timestamp + os_sys_tick_get()/100);
}


time_t time (time_t *_timer)
{
    struct tm ts;
	time_t time;
	int8_t time_zone = 8;
	
	//need get local time
	timestamp_to_date(get_local_timestamp, &rtc_data_time, time_zone);
	
	ts.tm_hour = rtc_data_time.hour;
	ts.tm_min  = rtc_data_time.minute;
	ts.tm_sec  = rtc_data_time.sec;
	ts.tm_mon  = rtc_data_time.month-1;
	ts.tm_mday = rtc_data_time.day-1;
	ts.tm_year = rtc_data_time.year-1900;
	
	time = mktime(&ts);
	
	if(_timer != NULL)
    {
		*_timer = time;
    }
    return time;
}
