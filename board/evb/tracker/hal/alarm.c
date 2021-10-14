
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
static void alarm_timer_callback(void *p_handle)
{
    uint32_t timer_id;
	int param;
    // Which timer expired?
    os_timer_id_get(&p_handle, &timer_id);

    if (timer_id >= ALARM_ID_START && timer_id<ALARM_ID_END)
    {
        //do something
		g_Alarm[timer_id-ALARM_ID_START].handle((void *)&param);
    }
}

int set_alarm(alarm_t  *alarm)
{
    int t = get_expiry_time(alarm);
	
    if(t <= 0) return -1;

	if (os_timer_create(&alarm->xTimer, alarm->name, CUSTOM_TIMER_ALARM,
                       t*60*1000, false, alarm_timer_callback) == true)
    {
        // Timer created successfully, start the timer.
        //os_timer_start(&alarm->xTimer);
		return 0;
    } else {
        // Timer failed to create.
        return -1;
    }
}

bool start_alarm(alarm_t  *alarm)
{
	return os_timer_start(&alarm->xTimer);
}

void stop_alarm(alarm_t  *alarm)
{
	os_timer_stop(&alarm->xTimer);
}

/**
 * @brief  Create alarm
 *
 * @param  tm    alarm time, hour[0~23], min[0~59], day_interval[0~31]
 * @param  handle   action handle
 * @return pointer to skipped white spaces' new buffer.
 */
int alarm_create(cron_tm_t tm, action_handle handle)
{
	uint8_t i;
	int ret = 0;
	
	for(i=0; i<ALARM_MAX_ITEM; i++)
	{
		if(g_Alarm[i].id == ALARM_ID_NO_USED)
		{
			g_Alarm[i].id = ALARM_ID_START+i;
			g_Alarm[i].cron_tm.hour = tm.hour;
			g_Alarm[i].cron_tm.min = tm.min;
			g_Alarm[i].cron_tm.day_interval = tm.day_interval;
			g_Alarm[i].handle = handle;
			sprintf(g_Alarm[i].name, "alarm%d", i);
			ret = set_alarm(&g_Alarm[i]);
			break;
		}
	}
	
	return ret;
}

int alarm_delete(uint8_t id)
{
	uint8_t i;
	int ret = 0;
	
	for(i=0; i<ALARM_MAX_ITEM; i++)
	{
		if(g_Alarm[i].id == id)
		{
			stop_alarm(&g_Alarm[i]);
			g_Alarm[i].id = ALARM_ID_NO_USED;
			g_Alarm[i].cron_tm.hour = 0;
			g_Alarm[i].cron_tm.min = 0;
			g_Alarm[i].cron_tm.day_interval = 0;
			g_Alarm[i].handle = NULL;
			memset(g_Alarm[i].name, 0, ALARM_NAME_MAX_LEN);
			break;
		}
	}
	
	return ret;
}

void alarm_init(void)
{
	uint8_t i;
	memset(g_Alarm, 0, sizeof(alarm_t)*ALARM_MAX_ITEM);
	for(i=0; i<ALARM_MAX_ITEM; i++)
	{
		g_Alarm[i].id = ALARM_ID_NO_USED;
	}
}

