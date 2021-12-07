/**
*********************************************************************************************************
*               Copyright(c) 2016, Liyi Corporation. All rights reserved.
**********************************************************************************************************
* @file     custom_msg.h
* @brief    
* @details  
* @author   wangkun
* @date     2021-11-24
* @version  v0.1
*********************************************************************************************************
*/
#ifndef _CUSTOM_MSG_H_
#define _CUSTOM_MSG_H_

#include <stdint.h>
#include <app_msg.h>

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */


/**  @brief custom type definitions for message, may extend as requested */
typedef enum
{
    CUSTOM_MSG_START = IO_MSG_TYPE_I2C,             /**< subtype to be defined */
	CUSTOM_MSG_WIFI,
	CUSTOM_MSG_CONTROL,
	CUSTOM_MSG_KEYBOARD,
} T_CUSTOM_MSG_TYPE;

/**  @brief custom subtype definitions for @ref T_CUSTOM_MSG_TYPE_WIFI type */
typedef enum
{
    CUSTOM_MSG_WIFI_RESET,
	CUSTOM_MSG_WIFI_STATE,
	CUSTOM_MSG_WIFI_SEND,
} T_CUSTOM_MSG_TYPE_WIFI;

typedef enum
{
	CONTROL_MSG_INIT,
	CONTROL_MSG_FEEDFOOD,
	CONTROL_MSG_FEEDWATER,
	CONTROL_MSG_FOODSTUCK,
	CONTROL_MSG_WATER_LOW,
	CONTROL_MSG_FOOD_LITTLE,
} T_CONTROL_MSG_SUBTYPE;

typedef enum
{
    CUSTOM_MSG_KEY_DOWN,
	CUSTOM_MSG_KEY_LONG,
	CUSTOM_MSG_KEY_UP,
} T_CUSTOM_MSG_TYPE_KEYBOARD;

typedef enum
{
	CUSTOM_KEYBOARD_TIMER_ID      = 0x100,
	CUSTOM_FEED_WATER_TIMER_ID    = 0x200,
	CUSTOM_FEED_FOOD_TIMER_ID    = 0x300,
	CUSTOM_DEVICE_STATUS_TIMER_ID = 0x400,
} T_CUSTOM_TIMER_ID;

typedef T_IO_MSG T_CUSTOM_MSG;

#define MAX_NUMBER_OF_IO_MESSAGE      0x20      //!<  IO message queue size


#define MAKE_CUSTOM_MSG(msg, t, st) \
	T_CUSTOM_MSG msg = {.type=(t), .subtype=(st)}
	
#define MAKE_CUSTOM_MSG_PARAM(msg, t, st, data) \
	T_CUSTOM_MSG msg = {.type=(t), .subtype=(st), .u.param=(data)}
	
#define MAKE_CUSTOM_MSG_BUFFER(msg, t, st, buf) \
	T_CUSTOM_MSG msg = {.type=(t), .subtype=(st), .u.buf=(buf)}

#define ASSIGN_CUSTOM_MSG(msg, t, st) {msg.type=(t); msg.subtype=(st);}

#define ASSIGN_CUSTOM_MSG_PARAM(msg, t, st, data) \
	{msg.type=(t), msg.subtype=(st), msg.u.param=(data)}
	
#define ASSIGN_CUSTOM_MSG_BUFFER(msg, t, st, buf) \
	{msg.type=(t), msg.subtype=(st), msg.u.buf=(buf)}
	
	
void control_send_msg(T_IO_MSG msg);
void app_send_msg(T_IO_MSG msg);	
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif      /* _CUSTOM_MSG_H_*/
