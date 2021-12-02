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
*	�������ƣ�	keyboard_gpio_init
*
*	�������ܣ�	����IO��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		����Ϊ�͵�ƽ		�ͷ�Ϊ�ߵ�ƽ
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
*	�������ƣ�	KEY_Scan
*
*	�������ܣ�	������ƽɨ��
*
*	��ڲ�����	num���������
*
*	���ز�����	����״̬
*
*	˵����		
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
*	�������ƣ�	Keyboard
*
*	�������ܣ�	�������ܼ��
*
*	��ڲ�����	��
*
*	���ز�����	������ֵ
*
*	˵����		�ֵ�����˫��������
************************************************************
*/
T_KEY_STATUS KeyScan(void)
{
	static uint32_t key_busyflag = 0;									//�������ڷ��ͷ�״̬
	static uint8_t key_count = 0;										  //��������ʱ��
	uint8_t time_out = 30, i = 0;										  //�ж�˫����������Ҫ����ʱ���
	
	for(; i < KEY_NUM; i++)
	{
		if(KeyRead(i) == KEY_DOWN && !(key_busyflag & (~(1 << i))))		//������� ����������δ����
		{
			key_busyflag |= 1 << i;											//�˰�������æ״̬
			
			if(++key_count >= KEYDOWN_LONG_TIME)							//���¼�ʱ
				key_count = KEYDOWN_LONG_TIME;								//�ﵽ����ʱ���򲻱�
			
			return KEY_NONE;												//�����޶���״̬
		}
		else if(KeyRead(i) == KEY_UP && key_busyflag & (1 << i))			//����ͷ� �� ����֮ǰ�ǰ��¹���
		{
			key_busyflag &= ~(1 << i);										//�˰������ڿ���״̬
			
			if(key_count == KEYDOWN_LONG_TIME)								//����ǳ���
			{
				key_count = 0;												//���¼�ʱ����
				return key_event[i][KEY_X_DOWNLONG];						//���س�������
			}
			else
			{
				key_count = 0;												//���¼�ʱ����
				while(--time_out)											//������Ҫ�ǵȴ�Լ250ms���ж��Ƿ��еڶ��ΰ���
				{
					platform_delay_ms(10);											//�ô������������̬���ⲻӰ���������������
					
					if(KeyRead(i) == KEY_DOWN)								//�еڶ��ΰ��£�˵��Ϊ˫��
					{
						while(KeyRead(i) == KEY_DOWN)						//�ȴ��ͷţ��޴˾䣬˫������һ����������
							platform_delay_ms(5);									//�ô������������̬���ⲻӰ���������������
						
						return key_event[i][KEY_X_DOUBLE];					//����˫������
					}
					
				}
				
				return key_event[i][KEY_X_DOWN];							//�����жϾ���Ч����Ϊ��������
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
			//�����ж�-------------------------------------------------------
			if(key_event_r == key_event[i][KEY_X_DOWN])
			{
				LOG_I("(keycode:%d) short press\r\n", i);
			}
			
			//˫���ж�-------------------------------------------------------
			if(key_event_r == key_event[i][KEY_X_DOUBLE])
			{
				LOG_I("(keycode:%d) double press\r\n", i);
			}
			
			//�����ж�-------------------------------------------------------
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
