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
	int             id;         // 任务ID
    cron_tm_t       cron_tm;    // cron格式时间
	int             action;     // 响应操作，对于灯控产品来说，action可以表示开关、颜色、场景等等
    void			*xTimer;    // 定时器句柄
    //ListNode        node;       // 节点，用于将一系列定时任务组织成list
} alarm_t;

#define ALARM_MAX_ITEM    5

#ifdef __cplusplus
}
#endif

#endif /* _ALARM_H_ */