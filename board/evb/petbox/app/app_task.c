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
#include <rtl876x_gpio.h>
#include "board.h"
#include "custom_msg.h"
#include "custom_log.h"
#include "mcu_api.h"
#include "wifi.h"
#include "keyboard.h"

/** @defgroup  PERIPH_APP_TASK Peripheral App Task
    * @brief This file handles the implementation of application task related functions.
    *
    * Create App task and handle events & messages
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/
#define APP_TASK_PRIORITY             1         //!< Task priorities
#define APP_TASK_STACK_SIZE           256 * 4   //!<  Task stack size
//#define MAX_NUMBER_OF_GAP_MESSAGE     0x20      //!<  GAP message queue size
//#define MAX_NUMBER_OF_IO_MESSAGE      0x20      //!<  IO message queue size
//#define MAX_NUMBER_OF_EVENT_MESSAGE   (MAX_NUMBER_OF_GAP_MESSAGE + MAX_NUMBER_OF_IO_MESSAGE)    //!< Event message queue size

/*============================================================================*
 *                              Variables
 *============================================================================*/
void *app_task_handle;   //!< APP Task handle
//void *evt_queue_handle;  //!< Event queue handle
static void *io_queue_handle;   //!< IO queue handle

/*============================================================================*
 *                              Functions
 *============================================================================*/
void app_main_task(void *p_param);

/**
 * @brief  Initialize App task
 * @return void
 */
void app_task_init()
{
    os_task_create(&app_task_handle, "app", app_main_task, 0, APP_TASK_STACK_SIZE,
                   APP_TASK_PRIORITY);
}

void app_handle_io_msg(T_IO_MSG io_msg)
{
    uint16_t msg_type = io_msg.type;
	uint16_t msg_subtype = io_msg.subtype;
	if(msg_type == CUSTOM_MSG_WIFI)
	{
		switch (msg_subtype)
		{
			case CUSTOM_MSG_WIFI_RESET:
			{
				LOG_I("Reset wifi module!!!! \r\n");
				mcu_reset_wifi();
			}
			break;
			
			case CUSTOM_MSG_WIFI_STATE:
			{
				uint8_t state = io_msg.u.param;
				if(state == WIFI_CONN_CLOUD)
				{
					MAKE_CUSTOM_MSG(msg, CUSTOM_MSG_CONTROL, CONTROL_MSG_INIT);
					control_send_msg(msg);
				}
			}
			break;
			
			case CUSTOM_MSG_WIFI_SEND:
				
			break;
			
			default:
				break;
		}
	}
	else if(msg_type == CUSTOM_MSG_KEYBOARD)
	{
		uint8_t keycode = msg_subtype;;
		uint8_t keyaction = io_msg.u.param;
		const char code[][8] = {"config","feed"};
		const char action[][8] = {"null", "up", "down", "long"};
		LOG_I("keycode:%s, keyaction:%s \r\n",code[keycode], action[keyaction]);
		switch (keyaction)
		{
			case KEY_EVENT_UP:
			{
				if(keycode == KEY_FEED)
				{
					MAKE_CUSTOM_MSG_PARAM(msg, CUSTOM_MSG_CONTROL, CONTROL_MSG_FEEDFOOD_START, 1);
					app_send_msg(msg);
				}
			}
			break;
			
			case KEY_EVENT_DOWN:
			{
				
			}
			break;
			
			case KEY_EVENT_LONG:
			{
				if(keycode == KEY_CONFIG)
				{
					MAKE_CUSTOM_MSG(msg, CUSTOM_MSG_WIFI, CUSTOM_MSG_WIFI_RESET);
					app_send_msg(msg);
				}
			}
			break;
			
			default:
				break;
		}
	}
}

/**
 * @brief        App task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
void app_main_task(void *p_param)
{
//    uint8_t event;
    os_msg_queue_create(&io_queue_handle, MAX_NUMBER_OF_IO_MESSAGE, sizeof(T_IO_MSG));
//    os_msg_queue_create(&evt_queue_handle, MAX_NUMBER_OF_EVENT_MESSAGE, sizeof(uint8_t));

    driver_init();
	//os_delay(20000);
    while (true)
    {
//        if (os_msg_recv(evt_queue_handle, &event, 0xFFFFFFFF) == true)
//        {
//            if (event == EVENT_IO_TO_APP)
//            {
//                T_IO_MSG io_msg;
//                if (os_msg_recv(io_queue_handle, &io_msg, 0) == true)
//                {
//                    app_handle_io_msg(io_msg);
//                }
//            }
//            else
//            {
//                //gap_handle_msg(event);
//            }
//        }
		wifi_uart_service();
		{
			T_IO_MSG io_msg;
			if (os_msg_recv(io_queue_handle, &io_msg, 0) == true)
			{
				app_handle_io_msg(io_msg);
			}
		}
		os_delay(100);
		//LOG_I("Test info......\r\n");
		//Get_Weight();
    }
}

void app_send_msg(T_IO_MSG msg)
{
	os_msg_send(io_queue_handle, &msg, 0);
}
/** @} */ /* End of group PERIPH_APP_TASK */


