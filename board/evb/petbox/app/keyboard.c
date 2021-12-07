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
#include "custom_msg.h"
#include "keyboard.h"
#include "board.h"

#define KEY_READ_PIN(n) GPIO_ReadInputDataBit(PNUM(n))

uint8_t keypin[KEY_NUM] = {CONFIG_KEY_PIN, FEED_KEY_PIN};

ST_KeyBoard keyboard[KEY_NUM] = {
	{KEY_ENABLE, 0, KEY_LOW, KEY_LOW, KEY_NULL, KEY_EVENT_NULL, CONFIG_KEY_PIN},
	{KEY_ENABLE, 0, KEY_LOW, KEY_LOW, KEY_NULL, KEY_EVENT_NULL, FEED_KEY_PIN},
};

static void *keyboard_timer_handle;
/*
************************************************************
*	函数名称：	keyboard_gpio_init
*	函数功能：	按键IO初始化
*	入口参数：	无
*	返回参数：	无
*	说明：		按下为低电平		释放为高电平
************************************************************
*/

static void keyboard_gpio_init(void)
{
    Pad_Config(CONFIG_KEY_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pinmux_Config(CONFIG_KEY_PIN, DWGPIO);
	
	Pad_Config(FEED_KEY_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pinmux_Config(FEED_KEY_PIN, DWGPIO);
	
	/* Initialize GPIO peripheral */
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
  
	GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin    = PNUM(CONFIG_KEY_PIN) | PNUM(FEED_KEY_PIN);
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);
	
	for(int i=0; i<KEY_NUM; i++)
	{
		keyboard[i].KeyShield = KEY_ENABLE;
		keyboard[i].KeyCount = 0;
		keyboard[i].KeyLevel = KEY_LOW;
		keyboard[i].KeyDownLevel = KEY_LOW;
		keyboard[i].KeyStatus = KEY_NULL;
		keyboard[i].KeyEvent = KEY_EVENT_NULL;
		keyboard[i].KeyPin = keypin[i];
	}
}
/*
************************************************************
*	函数名称：	KeyReadLevel
*	函数功能：	按键电平扫描
*	入口参数：	NULL
*	返回参数：	按键状态
*	说明：		
************************************************************
*/
//static void KeyReadLevel(void)
static void KeyReadLevel(uint8_t num)
{
//    uint8_t num;
//    
//    for(num=0; num<KEY_NUM; num++)
//    {
        if(keyboard[num].KeyShield == 0)
            return;
		
		if(KEY_READ_PIN(keyboard[num].KeyPin) == keyboard[num].KeyDownLevel)
			keyboard[num].KeyLevel = KEY_HIGH;
		else
			keyboard[num].KeyLevel = KEY_LOW;
//    }
}

/*
************************************************************
*	函数名称：	Keyboard
*	函数功能：	按键功能检测
*	入口参数：	无
*	返回参数：	按键键值
*	说明：		分单击、双击、长安
************************************************************
*/
void ReadKeyStatus(void)
{
    uint8_t i;
	
    for(i=0; i<KEY_NUM; i++)
    {
		KeyReadLevel(i);
        switch(keyboard[i].KeyStatus)
        {
            //state 0：no key press
            case KEY_NULL:
                if(keyboard[i].KeyLevel == KEY_HIGH)//key press
                {
                    keyboard[i].KeyStatus = KEY_SURE;
					keyboard[i].KeyEvent = KEY_EVENT_NULL;
                }
                else
                {
                    keyboard[i].KeyEvent = KEY_EVENT_NULL;
                }
                break;
            //state 1：key press check
            case KEY_SURE:
                if(keyboard[i].KeyLevel == KEY_HIGH)
                {
                    keyboard[i].KeyStatus = KEY_DOWN;
					keyboard[i].KeyEvent = KEY_EVENT_DOWN;
                    keyboard[i].KeyCount = 0;//clear count
                }
                else
                {
                    keyboard[i].KeyStatus = KEY_NULL;
                    keyboard[i].KeyEvent = KEY_EVENT_NULL;
                }
                break;
            //state 2：key press
            case KEY_DOWN:
                if(keyboard[i].KeyLevel != KEY_HIGH)
                {
                    keyboard[i].KeyStatus = KEY_UP;
                    keyboard[i].KeyEvent = KEY_EVENT_UP;
                }
                else if((keyboard[i].KeyLevel == KEY_HIGH) && (++keyboard[i].KeyCount >= KEY_EVENT_LONG_COUNT))
                {
                    keyboard[i].KeyStatus = KEY_LONG;
                    keyboard[i].KeyEvent = KEY_EVENT_LONG;//long press event
					//keyboard[i].KeyCount = 0;//clear count
                }
                else
                {
                    keyboard[i].KeyEvent = KEY_EVENT_NULL;//null event
                }
                break;
			//state 3: key allways press
            case KEY_LONG:
                if(keyboard[i].KeyLevel != KEY_HIGH)//key release
                {
                    keyboard[i].KeyStatus = KEY_UP;
                    keyboard[i].KeyEvent = KEY_EVENT_UP;//release event
					//keyboard[i].KeyEvent = KEY_EVENT_NULL;
                }
                else if((keyboard[i].KeyLevel == KEY_HIGH) && (keyboard[i].KeyCount >= KEY_EVENT_LONG_COUNT))
                {
					keyboard[i].KeyStatus = KEY_LONG;
                    keyboard[i].KeyEvent = KEY_EVENT_NULL;//long press event
                    //keyboard[i].KeyCount = 0;//clear count
                }
                else
                {
                    keyboard[i].KeyEvent = KEY_EVENT_NULL;//null event
                }
                break;
			//state 4: release key
			case KEY_UP:
				if(keyboard[i].KeyLevel != KEY_HIGH)
				{
					keyboard[i].KeyStatus = KEY_NULL;
					keyboard[i].KeyEvent = KEY_EVENT_NULL;
					keyboard[i].KeyCount = 0;//clear count
				}
				break;
        }
		if(keyboard[i].KeyEvent != KEY_EVENT_NULL)
		{
			MAKE_CUSTOM_MSG_PARAM(msg, CUSTOM_MSG_KEYBOARD, i, keyboard[i].KeyEvent);
			app_send_msg(msg);
		}
	}
}

void KeyTask(void)
{
	ReadKeyStatus();
	
//	if(keyboard[KEY_CONFIG].KeyEvent == KEY_EVENT_UP)
//	{
//		LOG_I("KEY_CONFIG Up\n");
//	}
//	else if(keyboard[KEY_CONFIG].KeyEvent == KEY_EVENT_DOWN)
//	{
//		LOG_I("KEY_CONFIG Down\n");
//	}
//	else if(keyboard[KEY_CONFIG].KeyEvent == KEY_EVENT_LONG)
//	{
//		LOG_I("KEY_CONFIG Long Down\n");
//	}
//	
//	if(keyboard[KEY_FEED].KeyEvent == KEY_EVENT_UP)
//	{
//		LOG_I("KEY_FEED Up\n");
//	}
//	else if(keyboard[KEY_FEED].KeyEvent == KEY_EVENT_DOWN)
//	{
//		LOG_I("KEY_FEED Down\n");
//	}
//	else if(keyboard[KEY_FEED].KeyEvent == KEY_EVENT_LONG)
//	{
//		LOG_I("KEY_FEED Long Down\n");
//	}
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
