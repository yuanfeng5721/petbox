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
#include "os_msg.h"
#include "os_task.h"
#include "trace.h"
#include "gap.h"
#include "gap_le.h"
#include "cmd_uart.h"
#include "cmd_parse.h"
#include "app_msg.h"
#include "app_task.h"

/** @defgroup  BT5_CENTRAL_APP_TASK BT5 Central App Task
    * @brief This file handles the implementation of application task related functions.
    *
    * Create App task and handle events & messages
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/
#define CMD_TASK_PRIORITY             1         //!< Task priorities
#define CMD_TASK_STACK_SIZE           256 * 10   //!<  Task stack size
#define MAX_NUMBER_OF_GAP_MESSAGE     0x20      //!<  GAP message queue size
#define MAX_NUMBER_OF_IO_MESSAGE      0x20      //!<  IO message queue size
#define MAX_NUMBER_OF_EVENT_MESSAGE   (MAX_NUMBER_OF_GAP_MESSAGE + MAX_NUMBER_OF_IO_MESSAGE)    //!< Event message queue size

/*============================================================================*
 *                              Variables
 *============================================================================*/
void *cmd_task_handle;   //!< CMD Task handle
void *evt_queue_handle;  //!< Event queue handle
void *io_queue_handle;   //!< IO queue handle

T_CMD_IF    cmd_if;
/*============================================================================*
 *                              Functions
 *============================================================================*/
void cmd_task(void *p_param);


void cmd_handle_io_msg(T_IO_MSG io_msg)
{
    uint16_t msg_type = io_msg.type;
    uint8_t rx_char;

    switch (msg_type)
    {
    case IO_MSG_TYPE_UART:
        /* We handle user command informations from Data UART in this branch. */
        rx_char = (uint8_t)io_msg.subtype;
        cmd_collect(&cmd_if, &rx_char, sizeof(rx_char));
        break;
    default:
        break;
    }
}
/**
 * @brief  Initialize App task
 * @return void
 */
void cmd_task_init()
{
    os_task_create(&cmd_task_handle, "cmd", cmd_task, 0, CMD_TASK_STACK_SIZE,
                   CMD_TASK_PRIORITY);
}

/**
 * @brief        App task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
void cmd_task(void *p_param)
{
    uint8_t event;

    os_msg_queue_create(&io_queue_handle, MAX_NUMBER_OF_IO_MESSAGE, sizeof(T_IO_MSG));
    os_msg_queue_create(&evt_queue_handle, MAX_NUMBER_OF_EVENT_MESSAGE, sizeof(uint8_t));

    cmd_uart_init(evt_queue_handle, io_queue_handle);
    //cmd_init(&cmd_if, btlink_cmd_parse, "cmd_task");

    while (true)
    {
        if (os_msg_recv(evt_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            if (event == EVENT_IO_TO_APP)
            {
                T_IO_MSG io_msg;
                if (os_msg_recv(io_queue_handle, &io_msg, 0) == true)
                {
                    cmd_handle_io_msg(io_msg);
                }
            }
//            else
//            {
//                gap_handle_msg(event);
//            }
        }
    }
}

/** @} */ /* End of group BT5_CENTRAL_APP_TASK */


