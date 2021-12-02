#if 1

#define LOG_TAG "keyboard"
/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <stdlib.h>
#include <os_sched.h>
#include <string.h>
#include <app_task.h>
#include <rtl876x_gpio.h>
#include <rtl876x_pinmux.h>
#include <platform_utils.h>
#include "os_timer.h"
#include "custom_log.h"
#include "cmd_task.h"
#include "mcu_api.h"
#include "custom_msg.h"
#include "keyboard.h"
#include "board.h"


uint8_t key_event[KEY_NUM][4];
uint8_t key_pin[KEY_NUM] = 
{
	CONFIG_KEY_PIN,
	FEED_KEY_PIN,
};
void *keyboard_timer_handle;
/*
************************************************************
*	函数名称：	keyboard_gpio_init
*
*	函数功能：	按键IO初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		按下为低电平		释放为高电平
************************************************************
*/

void keyboard_gpio_init(void)
{
    Pad_Config(CONFIG_KEY_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pinmux_Config(CONFIG_KEY_PIN, DWGPIO);
	
	Pad_Config(FEED_KEY_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pinmux_Config(FEED_KEY_PIN, DWGPIO);
	
	/* Initialize GPIO peripheral */
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
  
	GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin    = GPIO_GetPin(CONFIG_KEY_PIN) | GPIO_GetPin(FEED_KEY_PIN);
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);
	
	uint8_t i = 0, j = 0, event_num = 0;
	
	for(; i < KEY_NUM; i++)
	{
		for(j = 0; j < 4; j++)
			key_event[i][j] = event_num++;
	}
}
/*
************************************************************
*	函数名称：	KEY_Scan
*
*	函数功能：	按键电平扫描
*
*	入口参数：	num：按键编号
*
*	返回参数：	按键状态
*
*	说明：		
************************************************************
*/
T_KEY_STATUS KeyRead(uint32_t num)
{
	T_KEY_STATUS key_event_s = KEY_NONE;
	
	if(num >= KEY_NUM)
		return key_event_s;
	
	key_event_s = GPIO_ReadInputDataBit(GPIO_GetPin(key_pin[num]));
	
	if(key_event_s == 1)
		return KEY_UP;
	else if(key_event_s == 0)
		return KEY_DOWN;
	else
		return KEY_NONE;
}

/*
************************************************************
*	函数名称：	Keyboard
*
*	函数功能：	按键功能检测
*
*	入口参数：	无
*
*	返回参数：	按键键值
*
*	说明：		分单击、双击、长安
************************************************************
*/
T_KEY_STATUS KeyScan(void)
{
	static uint32_t key_busyflag = 0;									//按键处于非释放状态
	static uint8_t key_count = 0;										  //按键按下时间
	uint8_t time_out = 30, i = 0;										  //判断双击动作所需要的延时间隔
	
	for(; i < KEY_NUM; i++)
	{
		if(KeyRead(i) == KEY_DOWN && !(key_busyflag & (~(1 << i))))		//如果按下 且其他按键未按下
		{
			key_busyflag |= 1 << i;											//此按键处于忙状态
			
			if(++key_count >= KEYDOWN_LONG_TIME)							//按下计时
				key_count = KEYDOWN_LONG_TIME;								//达到长按时长则不变
			
			return KEY_NONE;												//返回无动作状态
		}
		else if(KeyRead(i) == KEY_UP && key_busyflag & (1 << i))			//如果释放 且 按键之前是按下过的
		{
			key_busyflag &= ~(1 << i);										//此按键处于空闲状态
			
			if(key_count == KEYDOWN_LONG_TIME)								//如果是长按
			{
				key_count = 0;												//按下计时清零
				return key_event[i][KEY_X_DOWNLONG];						//返回长按动作
			}
			else
			{
				key_count = 0;												//按下计时清零
				while(--time_out)											//这里主要是等待约250ms，判断是否有第二次按下
				{
					platform_delay_ms(10);											//让此任务进入阻塞态，这不影响代码正常的运行
					
					if(KeyRead(i) == KEY_DOWN)								//有第二次按下，说明为双击
					{
						while(KeyRead(i) == KEY_DOWN)						//等待释放，无此句，双击后会跟一个单击动作
							platform_delay_ms(5);									//让此任务进入阻塞态，这不影响代码正常的运行
						
						return key_event[i][KEY_X_DOUBLE];					//返回双击动作
					}
					
				}
				
				return key_event[i][KEY_X_DOWN];							//以上判断均无效，则为单击动作
			}
		}
	}
	
	key_busyflag = 0;
	key_count = 0;
	
	return KEY_NONE;
	
}

void KeyTask(void)
{
	T_KEY_STATUS key_event_r = KEY_NONE;
	uint8_t i = 0;
	while(1)
	{
		key_event_r = KeyScan();
		
		for(i=0; i<KEY_NUM; i++)
		{
			//单击判断-------------------------------------------------------
			if(key_event_r == key_event[i][KEY_X_DOWN])
			{
				LOG_I("(keycode:%d) short press\r\n", i);
			}
			
			//双击判断-------------------------------------------------------
			if(key_event_r == key_event[i][KEY_X_DOUBLE])
			{
				LOG_I("(keycode:%d) double press\r\n", i);
			}
			
			//长按判断-------------------------------------------------------
			if(key_event_r == key_event[i][KEY_X_DOWNLONG])
			{
				LOG_I("(keycode:%d)) long press\r\n", i);
			}
		}
	}
}

void keyboard_timer_callback(void *p_handle)
{
	uint32_t timer_id;
	
	os_timer_id_get(&p_handle, &timer_id);

	if (timer_id == CUSTOM_KEYBOARD_TIMER_ID)
	{
		KeyTask();
	}
}

int KeyBoard_Init(void)
{
	keyboard_gpio_init();
	
	if (os_timer_create(&keyboard_timer_handle, "keyboard", CUSTOM_KEYBOARD_TIMER_ID,
                       50, true, keyboard_timer_callback) == true)
    {
        // Timer created successfully, start the timer.
        os_timer_start(&keyboard_timer_handle);
		return 0;
    } else {
        // Timer failed to create.
        return -1;
    }
}	
#endif
