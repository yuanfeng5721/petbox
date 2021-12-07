#define LOG_TAG "CONTROL"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <os_sched.h>
#include <rtl876x_gpio.h>
#include <rtl876x_pinmux.h>
#include <platform_utils.h>
#include "rtl876x_nvic.h"
#include "os_timer.h"
#include "custom_msg.h"
#include "custom_log.h"
#include "control.h"
#include "board.h"

void *feedwater_timer_handle;
void *feedfood_timer_handle;
void *device_status_timer_handle;

static uint8_t g_feed_food_num = 0;

void control_timer_init(void);
extern void control_send_msg(T_IO_MSG msg);

void Control_Init(void)
{
	static bool init_state = false;
	
	if(!init_state)
	{
		//output
		//feed water
		PAD_CFG_GPIO(WATER_PUMP_EN_PIN, PAD_PULL_NONE, PAD_OUT_ENABLE);
		PAD_CFG_GPIO(WATER_PUMP_LED_PIN, PAD_PULL_NONE, PAD_OUT_ENABLE);
		
		//feed food
		PAD_CFG_GPIO(FEED_MOTO_EN_PIN, PAD_PULL_NONE, PAD_OUT_ENABLE);
		PAD_CFG_GPIO(FEED_MOTO_CTL1_PIN, PAD_PULL_NONE, PAD_OUT_ENABLE);
		PAD_CFG_GPIO(FEED_MOTO_CTL2_PIN, PAD_PULL_NONE, PAD_OUT_ENABLE);
		PAD_CFG_GPIO(FEED_STUCK_DET_TX_PIN, PAD_PULL_NONE, PAD_OUT_ENABLE);
		PAD_CFG_GPIO(FEED_NUM_EN_PIN, PAD_PULL_NONE, PAD_OUT_ENABLE);
		
		//input
		//feed water
		PAD_CFG_GPIO(WATER_LEVEL_M_PIN, PAD_PULL_NONE, PAD_OUT_DISABLE);
		PAD_CFG_GPIO(WATER_LEVEL_L_PIN, PAD_PULL_NONE, PAD_OUT_DISABLE);
		PAD_CFG_GPIO(WATER_AUTO_DET_PIN, PAD_PULL_NONE, PAD_OUT_DISABLE);
		
		//feed food
		PAD_CFG_GPIO(FEED_BUCKET_DET_PIN, PAD_PULL_NONE, PAD_OUT_DISABLE);
		PAD_CFG_GPIO(FEED_AUTO_DET_PIN, PAD_PULL_NONE, PAD_OUT_DISABLE);
		PAD_CFG_GPIO(FEED_NUM_COUNT_PIN, PAD_PULL_NONE, PAD_OUT_DISABLE);
		PAD_CFG_GPIO(FEED_STUCK_DET_RX_PIN, PAD_PULL_NONE, PAD_OUT_DISABLE);
		
		/* Initialize GPIO peripheral */
		RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

		GPIO_InitTypeDef GPIO_InitStruct;
		
		//output
		GPIO_StructInit(&GPIO_InitStruct);
		GPIO_InitStruct.GPIO_Pin    = PNUM(WATER_PUMP_EN_PIN) | PNUM(WATER_PUMP_LED_PIN) | PNUM(FEED_MOTO_EN_PIN) 
									| PNUM(FEED_MOTO_CTL1_PIN) | PNUM(FEED_MOTO_CTL2_PIN) | PNUM(FEED_STUCK_DET_TX_PIN)
									| PNUM(FEED_NUM_EN_PIN);
		GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
		GPIO_Init(&GPIO_InitStruct);
		
		GPIO_SET(FEED_STUCK_DET_TX_PIN,Bit_SET);
		
		//input no interrupt
		GPIO_StructInit(&GPIO_InitStruct);
		GPIO_InitStruct.GPIO_Pin    = PNUM(WATER_LEVEL_M_PIN)|PNUM(WATER_LEVEL_L_PIN)|PNUM(FEED_BUCKET_DET_PIN)|PNUM(FEED_STUCK_DET_RX_PIN);
		GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
		GPIO_Init(&GPIO_InitStruct);
		
		//input with interrupt
		GPIO_StructInit(&GPIO_InitStruct);
		GPIO_InitStruct.GPIO_Pin        = PNUM(WATER_AUTO_DET_PIN)|PNUM(FEED_AUTO_DET_PIN);
		GPIO_InitStruct.GPIO_Mode       = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_ITCmd      = ENABLE;
		GPIO_InitStruct.GPIO_ITTrigger  = GPIO_INT_Trigger_EDGE;
		GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
		GPIO_InitStruct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_ENABLE;
		GPIO_InitStruct.GPIO_DebounceTime = 20;/* unit:ms , can be 1~64 ms */
		GPIO_Init(&GPIO_InitStruct);

		GPIO_ClearINTPendingBit(PNUM(WATER_AUTO_DET_PIN)|PNUM(FEED_AUTO_DET_PIN));
		
		NVIC_InitTypeDef NVIC_InitStruct;
		NVIC_InitStruct.NVIC_IRQChannel = FEED_WATER_DET_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStruct);
		
		NVIC_InitStruct.NVIC_IRQChannel = FEED_FOOD_DET_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStruct);
		
//		NVIC_InitStruct.NVIC_IRQChannel = FOOD_STUCK_DET_IRQn;
//		NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
//		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStruct);
		
		control_timer_init();
		
		GPIO_MaskINTConfig(PNUM(WATER_AUTO_DET_PIN)|PNUM(FEED_AUTO_DET_PIN), DISABLE);
		GPIO_INTConfig(PNUM(WATER_AUTO_DET_PIN)|PNUM(FEED_AUTO_DET_PIN), ENABLE);
		init_state = !init_state;
	}
	else
	{
		LOG_I("control hardware had been initialized!!!!\r\n");
	}
}

static void feedfood_timer_callback(void *param)
{
	if(--g_feed_food_num == 0)
	{
		LOG_I("feed food end!!!!\r\n");
		GPIO_SET(FEED_MOTO_EN_PIN,Bit_RESET);
		GPIO_SET(FEED_MOTO_CTL1_PIN,Bit_RESET);
		GPIO_SET(FEED_MOTO_CTL2_PIN,Bit_RESET);
	}
	else
	{
		os_timer_start(&feedfood_timer_handle);
	}
}

static void feed_food_timer(void)
{
	if (os_timer_create(&feedfood_timer_handle, "feedfood", CUSTOM_FEED_FOOD_TIMER_ID,
							   10*1000, false, feedfood_timer_callback) == true)
	{
		// Timer created successfully, start the timer.
		//os_timer_start(&feedwater_timer_handle);
	}
	else
	{
		LOG_I("feed food timer error!!!!\r\n");
	}
}

void feed_food(uint8_t num)
{
	LOG_I("feed food %d !!!!\r\n", g_feed_food_num);
	if(!g_feed_food_num)
	{
		g_feed_food_num = num;
		LOG_I("feed food start!!!!\r\n");
		//start moto and count 
		os_timer_start(&feedfood_timer_handle);
		GPIO_SET(FEED_MOTO_EN_PIN,Bit_SET);
		GPIO_SET(FEED_MOTO_CTL1_PIN,Bit_SET);
		GPIO_SET(FEED_MOTO_CTL2_PIN,Bit_RESET);
	}
	else
	{
		LOG_I("had been feed food!!!!\r\n");
	}
}

static void feedwater_timer_callback(void *param)
{
	GPIO_SET(WATER_PUMP_EN_PIN,Bit_RESET);
	GPIO_SET(WATER_PUMP_LED_PIN,Bit_RESET);
	LOG_I("feed water end!!!!\r\n");
}

static void feed_water_timer(void)
{
	if (os_timer_create(&feedwater_timer_handle, "feedwater", CUSTOM_FEED_WATER_TIMER_ID,
							   20*1000, false, feedwater_timer_callback) == true)
	{
		// Timer created successfully, start the timer.
		//os_timer_start(&feedwater_timer_handle);
	}
	else
	{
		LOG_I("feed water timer error!!!!\r\n");
	}
}

void feed_water(bool status)
{
	static bool status_bak = 0;
	
	LOG_I("feed water %d !!!!\r\n", status);
	if(status_bak != status)
	{
		status_bak = status;
		if(status)
		{
			os_timer_start(&feedwater_timer_handle);
			GPIO_SET(WATER_PUMP_EN_PIN,Bit_SET);
			GPIO_SET(WATER_PUMP_LED_PIN,Bit_SET);
		}
		else
		{
			GPIO_SET(WATER_PUMP_EN_PIN,Bit_RESET);
			GPIO_SET(WATER_PUMP_LED_PIN,Bit_RESET);
		}
	}
}

static void device_status_check_callback(void *param)
{
	
	if(GPIO_GET(WATER_LEVEL_M_PIN))
	{
		LOG_I("Water level to middle!!!!\r\n");
	}
	
	if(GPIO_GET(WATER_LEVEL_L_PIN))
	{
		LOG_I("Water level to the low!!!!\r\n");
		MAKE_CUSTOM_MSG(msg, CUSTOM_MSG_CONTROL, CONTROL_MSG_WATER_LOW);
		control_send_msg(msg);
	}
	
	if(GPIO_GET(FEED_BUCKET_DET_PIN))
	{
		LOG_I("Feed bucket had removed!!!!\r\n");
		//MAKE_CUSTOM_MSG(msg, CUSTOM_MSG_CONTROL, CONTROL_MSG_FOODSTUCK);
		//control_send_msg(msg);
	}
	
	if(GPIO_GET(FEED_STUCK_DET_RX_PIN))
	{
		LOG_I("Feed stuck!!!!\r\n");
		MAKE_CUSTOM_MSG(msg, CUSTOM_MSG_CONTROL, CONTROL_MSG_FOODSTUCK);
		control_send_msg(msg);
	}
}

static void device_status_check_start(void)
{
	if (os_timer_create(&device_status_timer_handle, "status", CUSTOM_DEVICE_STATUS_TIMER_ID,
							   EXCEPTION_CHECK_FREQ, true, device_status_check_callback) == true)
	{
			// Timer created successfully, start the timer.
			os_timer_start(&device_status_timer_handle);
	}
	else
	{
		LOG_I("device status check error!!!!\r\n");
	}
}

void control_timer_init(void)
{
	feed_water_timer();
	feed_food_timer();
	device_status_check_start();
}

void WATER_LEVEL_M_HANDLER(void)
{
	
}
void WATER_LEVEL_L_HANDLER(void)
{
	
}
void FEED_WATER_DET_HANDLER(void)
{
	GPIO_INTConfig(PNUM(WATER_AUTO_DET_PIN), DISABLE);
    GPIO_MaskINTConfig(PNUM(WATER_AUTO_DET_PIN), ENABLE);

	LOG_I("auto feed water !!!!\r\n");
	MAKE_CUSTOM_MSG_PARAM(msg, CUSTOM_MSG_CONTROL, CONTROL_MSG_FEEDWATER, 1);
	control_send_msg(msg);
    GPIO_ClearINTPendingBit(PNUM(WATER_AUTO_DET_PIN));
    GPIO_MaskINTConfig(PNUM(WATER_AUTO_DET_PIN), DISABLE);
    GPIO_INTConfig(PNUM(WATER_AUTO_DET_PIN), ENABLE);
}
void FEED_BUCKET_DET_HANDLER(void)
{
	
}
void FEED_FOOD_DET_HANDLER(void)
{
	GPIO_INTConfig(PNUM(FEED_AUTO_DET_PIN), DISABLE);
    GPIO_MaskINTConfig(PNUM(FEED_AUTO_DET_PIN), ENABLE);

	LOG_I("auto feed food !!!!\r\n");
	MAKE_CUSTOM_MSG_PARAM(msg, CUSTOM_MSG_CONTROL, CONTROL_MSG_FEEDFOOD, 1);
	control_send_msg(msg);
    GPIO_ClearINTPendingBit(PNUM(FEED_AUTO_DET_PIN));
    GPIO_MaskINTConfig(PNUM(FEED_AUTO_DET_PIN), DISABLE);
    GPIO_INTConfig(PNUM(FEED_AUTO_DET_PIN), ENABLE);
}

void FOOD_STUCK_DET_HANDLER(void)
{
	GPIO_INTConfig(PNUM(FEED_STUCK_DET_RX_PIN), DISABLE);
    GPIO_MaskINTConfig(PNUM(FEED_STUCK_DET_RX_PIN), ENABLE);

	LOG_I("food stuck!!!!\r\n");
	MAKE_CUSTOM_MSG(msg, CUSTOM_MSG_CONTROL, CONTROL_MSG_FOODSTUCK);
	control_send_msg(msg);
    GPIO_ClearINTPendingBit(PNUM(FEED_STUCK_DET_RX_PIN));
    GPIO_MaskINTConfig(PNUM(FEED_STUCK_DET_RX_PIN), DISABLE);
    GPIO_INTConfig(PNUM(FEED_STUCK_DET_RX_PIN), ENABLE);
}
