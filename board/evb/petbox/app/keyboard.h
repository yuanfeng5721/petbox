/**
*********************************************************************************************************
*               Copyright(c) 2016, Liyi Corporation. All rights reserved.
**********************************************************************************************************
* @file     keyboard.h
* @brief    
* @details  
* @author   wangkun
* @date     2021-11-24
* @version  v0.1
*********************************************************************************************************
*/
#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

//#define KEY_NUM			  2
//#define KEY_NONE		0xFF

typedef enum
{
	KEY_UP			= 0,
	KEY_DOWN		= 1,
	KEY_NONE	    = 0xFF,
}T_KEY_STATUS;

typedef enum
{
	KEY_X_DOWN		= 0,
	KEY_X_UP		= 1,
	KEY_X_DOUBLE	= 2,
	KEY_X_DOWNLONG	= 3,
}T_KEY_X_STATUS;

typedef enum
{
	KEY_CONFIG 		= 0,
	KEY_FEED		= 1,
	KEY_NUM			= 2,
}T_KEY_VALUE;

#define KEYDOWN_LONG_TIME		40		//计算长按时长。目前keyboard函数每50ms调用一次。


extern uint8_t key_event[KEY_NUM][4];

typedef struct ST_Key
{
	uint8_t keycode;
	uint8_t pin;
	char name[10];
	T_KEY_X_STATUS event;
} ST_Key;



void KEY_Init(void);
uint8_t KEY_Scan(uint32_t num);
uint8_t Keyboard(void);
void KeyBoard_Task_Init(void);



#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif      /* _KEYBOARD_H_*/
