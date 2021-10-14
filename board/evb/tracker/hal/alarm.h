/*********************************************************************************************************
*/
/*============================================================================*
  *                     Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _ALARM_H_
#define _ALARM_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdint.h>
#include "utils_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*action_handle)(void *param);  

#define ALARM_NAME_MAX_LEN              8
//#define USE_M_H_D_W_FMT   
/* cron 格式时间表示 */
#ifdef USE_M_H_D_W_FMT
typedef struct
{
    int     min;    // minute,          0xFFFFFFFF表示*
    int     hour;   // hour，           0xFFFFFFFF表示*
    //int     wday;   // day of week，    0xFFFFFFFF表示*，bit[0~6]  表示周一到周日,如周一到周五每天响铃，则0x1F
    int     mday;   // day of month，   0xFFFFFFFF表示*, bit[0~31] 表示1~31日,每月1,3,5号响铃，则0x15
    int     mon;    // month，          0xFFFFFFFF表示*，bit[0~11] 表示1~12月
} cron_tm_t;
#else
typedef struct
{
    uint8_t     min;           // minute,          0xFF表示*
    uint8_t     hour;          // hour，           0xFF表示*
    uint8_t		day_interval;  //day 间隔
} cron_tm_t;
#endif

/* 闹钟任务 */
typedef struct
{
	int           id;                       // 任务ID
    cron_tm_t     cron_tm;                  // cron格式时间
	action_handle handle;					// 响应handle
    void	      *xTimer;                  // 定时器句柄
	char 		  name[ALARM_NAME_MAX_LEN];
    //ListNode        node;                 // 节点，用于将一系列定时任务组织成list
} alarm_t;

#define ALARM_MAX_ITEM    5
#define ALARM_ID_NO_USED     0xFFFFFFFF
#define ALARM_ID_START       CUSTOM_TIMER_ALARM
#define ALARM_ID_1			 (CUSTOM_TIMER_ALARM+0)
#define ALARM_ID_2			 (CUSTOM_TIMER_ALARM+1)
#define ALARM_ID_3           (CUSTOM_TIMER_ALARM+2)
#define ALARM_ID_4           (CUSTOM_TIMER_ALARM+3)
#define ALARM_ID_5           (CUSTOM_TIMER_ALARM+4)
#define ALARM_ID_END		 (CUSTOM_TIMER_ALARM+ALARM_MAX_ITEM)



void alarm_init(void);
int alarm_create(cron_tm_t tm, action_handle handle);
int alarm_delete(uint8_t id);
bool start_alarm(alarm_t  *alarm);
void stop_alarm(alarm_t  *alarm);

#ifdef __cplusplus
}
#endif

#endif /* _ALARM_H_ */
