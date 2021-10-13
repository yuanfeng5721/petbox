
#include <stdio.h>
#include <time.h>

#include "board.h"

#include "custom_msg.h"
#include "custom_log.h"
#include "os_timer.h"
#include "os_mem.h"
#include "alarm.h"

static alarm_t g_Alarm[ALARM_MAX_ITEM] = {0};
//alarm_t *p_alarm = NULL;

#ifdef USE_M_H_D_W_FMT
int get_expiry_time(alarm_t  *alarm)
{
	int i;
	time_t    t;
	struct tm now;

	time(&t);
	localtime_r(&t, &now);

	int ret = 0;

	if(alarm->cron_tm.min != 0xFFFFFFFF)
	{
		ret += alarm->cron_tm.min - now.tm_min;
	}

	if(alarm->cron_tm.hour != 0xFFFFFFFF)
	{
		int flag = 0;

		for(i=0; i<24; i++)
		{
			if(i == alarm->cron_tm.hour){
				flag=1;
				break;
			}
		}

		if(flag){
			ret += 60*(i-now.tm_hour);
		}
	}

	int t1 = ret<0?1:0;

//	int d_wday=0;
	int d_mday=0;

//	for(int i=0; i<7; i++)
//	{
//		int wday = now.tm_wday + t1 +i;
//		if(1<<(wday%7) & alarm->cron_tm.wday){
//			d_wday=i+t1;
//			break;
//		}
//	}

	for(int i=0; i<30; i++)
	{
		int mday = now.tm_mday + t1 +i;

		if(1<<(mday%30) & alarm->cron_tm.mday){
			d_mday = i+t1;
			break;
		}
	}

	//int d = d_mday<d_wday?d_mday:d_wday;

	//ret += d*24*60;
	ret += d_mday*24*60;
	
	return ret;
}
#else
int get_expiry_time(alarm_t  *alarm)
{
	int i;
	time_t    t;
	struct tm now;
	int total_min = 0;
	int flag = 0;
	
	time(&t);
	localtime_r(&t, &now);

	total_min += alarm->cron_tm.min - now.tm_min;

	for(i=0; i<24; i++)
	{
		if(i == alarm->cron_tm.hour){
			flag=1;
			break;
		}
	}

	if(flag){
		total_min += 60*(i-now.tm_hour);
	}

	int d_mday = now.tm_mday + (total_min<0?1:0) + (alarm->cron_tm.day_interval-1);
	
	total_min += d_mday*24*60;
	
	return total_min;
}
#endif

// Timer callback function.
void alarm_timer_callback(void *p_handle)
{
    uint32_t timer_id;

    // Which timer expired?
    os_timer_id_get(&p_handle, &timer_id);

    if (timer_id == CUSTOM_TIMER_ALARM)
    {
         //do something
		
    }
}

int set_timer(alarm_t  *alarm)
{
    int t = get_expiry_time(alarm);
	
    if(t <= 0) return -1;

	if (os_timer_create(&alarm->xTimer, "alarm", CUSTOM_TIMER_ALARM,
                       t*60*1000, false, alarm_timer_callback) == true)
    {
        // Timer created successfully, start the timer.
        os_timer_start(&alarm->xTimer);
    } else {
        // Timer failed to create.
        return -1;
    }

    return 0;
}

int alarm_create(cron_tm_t cron_tm)
{
	
}

int alarm_init(void)
{
	memset(g_Alarm, 0, sizeof(alarm_t)*ALARM_MAX_ITEM);
}

