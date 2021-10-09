/*
*******************************************************************************
*
*     Copyright (c) 2021 btlink Ltd.
*
*******************************************************************************
*  file name:           btlink_timer.c
*  author:              Eric
*  version:             1.00
*  file description:
*******************************************************************************
*  revision history:    date               version                  author
*
*  change summary:
*
*******************************************************************************
*/

/******************************************************************************
* Include Files
******************************************************************************/
#include "os_timer.h"
#include "rtl876x_hal_bsp.h"

#include "btlink_timer.h"
#include "btlink_protocol_cmd.h"
#include "btlink_assem_hex_msg.h"
/*****************************************************************************
* Define
*****************************************************************************/
#define TIMER_1_SEC_PERIOD      1000 //1000ms
#define TIMER_1_MIN_PERIOD      TIMER_1_SEC_PERIOD * 60
/*****************************************************************************
* Typedef
*****************************************************************************/

/*****************************************************************************
* Extern Variable
*****************************************************************************/
extern uint16_t g_con_send_min_cnt;

/*****************************************************************************
* Extern Function
*****************************************************************************/

/*****************************************************************************
 * Location variable
 *****************************************************************************/
void *blink_sec_timer_handle;
void *blink_min_timer_handle;
 
/*****************************************************************************
* Location Function
*****************************************************************************/

// Timer callback function.
void btlink_min_timer_callback(void *p_handle)
{
	uint32_t timer_id;
	uint8_t buff[256] = {0};
	
	// Which timer expired?
	os_timer_id_get(&p_handle, &timer_id);

	if (timer_id == MIN_TIMER_ID)
	{
		g_con_send_min_cnt++;
		if (g_con_send_min_cnt >= g_btlink_config.cfg_lss.continue_send_interval)
		{
			g_con_send_min_cnt = 0;
			//assemble continue send message.
		}
		
		btlink_assemm_hex_prt_msg_hdlr(buff, 0x00, NULL);
		
		btlink_start_min_timer();
	}
}

int btink_min_timer_init(void)
{
	if (os_timer_create(&blink_min_timer_handle, "Mins Base Timer", MIN_TIMER_ID,
								TIMER_1_MIN_PERIOD, false, btlink_min_timer_callback))
	{
		// Timer created successfully, start the timer.
		return 0;
	}
	else
	{
		// Timer failed to create.
		return -1;
	}
}

int btlink_start_min_timer(void)
{
	if (os_timer_start(&blink_min_timer_handle))
	{
		// Timer start successfully
		return 0;
	}
	else
	{
		// Timer failed to start.
		return -1;
	}
}

int btlink_stop_min_timer(void)
{
	if (os_timer_stop(&blink_min_timer_handle))
	{
		// Timer start successfully
		return 0;
	}
	else
	{
		// Timer failed to start.
		return -1;
	}
}