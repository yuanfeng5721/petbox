


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os_sched.h"
#include "utils.h"

#define portTICK_PERIOD_MS 10
#define TIME_FORMAT_STR_LEN (20)

uint32_t HAL_GetTimeMs(void)
{
#if defined PLATFORM_HAS_TIME_FUNCS
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;

#elif defined PLATFORM_HAS_CMSIS
    return HAL_GetTick();
#else
	return os_sys_tick_get()*portTICK_PERIOD_MS;
#endif
}

/*Get timestamp*/
long HAL_Timer_current_sec(void)
{
    return HAL_GetTimeMs() / 1000;
}

char *HAL_Timer_current(char *time_str)
{
#if defined PLATFORM_HAS_TIME_FUNCS
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t    now_time = tv.tv_sec;
    struct tm tm_tmp   = *localtime(&now_time);
    strftime(time_str, TIME_FORMAT_STR_LEN, "%F %T", &tm_tmp);
    return time_str;
#else
    long time_sec;
    time_sec = HAL_Timer_current_sec();
    memset(time_str, 0, TIME_FORMAT_STR_LEN);
    snprintf(time_str, TIME_FORMAT_STR_LEN, "%ld", time_sec);
    return time_str;
#endif
}

static bool HAL_Timer_expired(Timer *timer)
{
    uint32_t now_ts;

    now_ts = HAL_GetTimeMs();

    return (now_ts > timer->end_time) ? true : false;
}

static void HAL_Timer_countdown_ms(Timer *timer, unsigned int timeout_ms)
{
    timer->end_time = HAL_GetTimeMs();
    timer->end_time += timeout_ms;
}

static void HAL_Timer_countdown(Timer *timer, unsigned int timeout)
{
    timer->end_time = HAL_GetTimeMs();
    timer->end_time += timeout * 1000;
}

static int HAL_Timer_remain(Timer *timer)
{
    return (int)(timer->end_time - HAL_GetTimeMs());
}

static void HAL_Timer_init(Timer *timer)
{
    timer->end_time = 0;
}

bool expired(Timer *timer)
{
    return HAL_Timer_expired(timer);
}

void countdown_ms(Timer *timer, unsigned int timeout_ms)
{
    HAL_Timer_countdown_ms(timer, timeout_ms);
}

void countdown(Timer *timer, unsigned int timeout)
{
    HAL_Timer_countdown(timer, timeout);
}

int left_ms(Timer *timer)
{
    return HAL_Timer_remain(timer);
}

void InitTimer(Timer *timer)
{
    HAL_Timer_init(timer);
}

char g_date_buf[25]={0};
void Get_Compile_Date_Base(uint16_t *Year, uint16_t *Month, uint16_t *Day)
{
	const char *pMonth[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	const char Date[12] = __DATE__;//取编译时间
	uint8_t i;
	for(i = 0; i < 12; i++)if(memcmp(Date, pMonth[i], 3) == 0)*Month = i + 1, i = 12;
	*Year = (uint16_t)atoi(Date + 7); //Date[9]为２位年份，Date[7]为完整年份
	*Day = (uint8_t)atoi(Date + 4);
}

char* Get_Compile_Date(void)
{
	uint16_t  Year, Month, Day;
	Get_Compile_Date_Base(&Year, &Month, &Day);//取编译时间
	sprintf(g_date_buf, "%04d-%02d-%02d_%s", Year, Month, Day, __TIME__);//任意格式化
	return g_date_buf;
}
		