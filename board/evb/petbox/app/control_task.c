/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_task.c
   * @brief     Routines to create App task and handle events & messages
   * @author    jane
   * @date      2017-06-02
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <os_msg.h>
#include <os_task.h>
#include <os_sched.h>
#include <gap.h>
#include <gap_le.h>
#include <app_task.h>
#include <app_msg.h>
#include <os_msg.h>
#include <rtl876x_gpio.h>
#include "board.h"
#include "custom_msg.h"
#include "custom_log.h"

#include "control_task.h"
#include "control.h"
#include "mcu_api.h"
#include "protocol.h"
/** @defgroup  PERIPH_APP_TASK Peripheral App Task
    * @brief This file handles the implementation of application task related functions.
    *
    * Create App task and handle events & messages
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/
#define CONTROL_TASK_PRIORITY             1         //!< Task priorities
#define CONTROL_TASK_STACK_SIZE           256 * 6   //!<  Task stack size
//#define MAX_NUMBER_OF_GAP_MESSAGE     0x20      //!<  GAP message queue size
//#define MAX_NUMBER_OF_IO_MESSAGE      0x20      //!<  IO message queue size
//#define MAX_NUMBER_OF_EVENT_MESSAGE   (MAX_NUMBER_OF_GAP_MESSAGE + MAX_NUMBER_OF_IO_MESSAGE)    //!< Event message queue size

/*============================================================================*
 *                              Variables
 *============================================================================*/
void *control_task_handle;   //!< APP Task handle
//void *evt_queue_handle;  //!< Event queue handle
static void *msg_queue_handle;   //!< IO queue handle

/*============================================================================*
 *                              Functions
 *============================================================================*/
void control_main_task(void *p_param);


void control_exception_send(uint8_t exception)
{
	mcu_dp_enum_update(DPID_REALTIME_DATA,exception);
}
/**
 * @brief  Initialize App task
 * @return void
 */
void control_task_init()
{
    os_task_create(&control_task_handle, "control", control_main_task, 0, CONTROL_TASK_STACK_SIZE,
                   CONTROL_TASK_PRIORITY);
}

void control_handle_io_msg(T_IO_MSG io_msg)
{
    uint16_t subtype = io_msg.subtype;

    switch (subtype)
    {
		case CONTROL_MSG_INIT:
		{
			Control_Init();
		}
		break;
		
		case CONTROL_MSG_FEEDFOOD_START:
		{
			feed_food(io_msg.u.param);
		}
		break;
		
		case CONTROL_MSG_FEEDFOOD_STOP:
		{
			
		}
		break;
		
		case CONTROL_MSG_FEEDWATER:
		{
			feed_water(io_msg.u.param);
		}
		break;
		
		case CONTROL_MSG_FOODSTUCK:
		{
			control_exception_send(1);
		}
		break;
		
		case CONTROL_MSG_WATER_LOW:
		{
			control_exception_send(2);
		}
		break;
		
		case CONTROL_MSG_PLAY_VOICE:
		{
			module_play_voice(io_msg.u.param);
		}
		break;
		
		case CONTROL_MSG_REPORT_REMAIN:
		{
			mcu_dp_value_update(DPID_FEED_REMAINING,io_msg.u.param);
		}
		break;
		
		default:
			break;
    }
}

/**
 * @brief        control task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
void control_main_task(void *p_param)
{
//    uint8_t event;
    os_msg_queue_create(&msg_queue_handle, MAX_NUMBER_OF_IO_MESSAGE, sizeof(T_IO_MSG));
//    os_msg_queue_create(&evt_queue_handle, MAX_NUMBER_OF_EVENT_MESSAGE, sizeof(uint8_t));

    while (true)
    {
		T_IO_MSG io_msg;
		if (os_msg_recv(msg_queue_handle, &io_msg, 0) == true)
		{
			if (io_msg.type == CUSTOM_MSG_CONTROL)
			{
				control_handle_io_msg(io_msg);
			}
		}
		os_delay(200);
		//LOG_I("control info......\r\n");
    }
}

void control_send_msg(T_IO_MSG msg)
{
	os_msg_send(msg_queue_handle, &msg, 0);
}

uint32_t control_feed_num(uint32_t num)
{
	uint8_t i;
	LOG_I("need feed %d part\r\n", num);
	MAKE_CUSTOM_MSG_PARAM(msg, CUSTOM_MSG_CONTROL, CONTROL_MSG_FEEDFOOD_START, num);
	os_msg_send(msg_queue_handle, &msg, 0);
	MAKE_CUSTOM_MSG_PARAM(msg1, CUSTOM_MSG_CONTROL, CONTROL_MSG_PLAY_VOICE, num);
	os_msg_send(msg_queue_handle, &msg1, 0);
	return num;
}

uint32_t control_feed_weight(uint32_t weight)
{
	LOG_I("set feed weight %d\r\n", weight);
	
	return weight;
}

uint8_t control_history(uint8_t history)
{
	LOG_I("set feed history %d\r\n", history);
	
	return history;
}

uint8_t control_feed_water(uint8_t water)
{
	LOG_I("set feed water %s\r\n", water?"on":"off");
	
	MAKE_CUSTOM_MSG_PARAM(msg, CUSTOM_MSG_CONTROL, CONTROL_MSG_FEEDWATER, water);
	os_msg_send(msg_queue_handle, &msg, 0);
	
	MAKE_CUSTOM_MSG_PARAM(msg1, CUSTOM_MSG_CONTROL, CONTROL_MSG_PLAY_VOICE, 1);
	os_msg_send(msg_queue_handle, &msg1, 0);
	return water;
}
/** @} */ /* End of group PERIPH_APP_TASK */


