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

typedef enum
{
	KEY_NULL        = 0x0,
	KEY_SURE        = 0x1,
	KEY_UP			= 0x2,
	KEY_DOWN		= 0x4,
	KEY_LONG        = 0x8,
}T_KEY_STATUS;

typedef enum
{
	KEY_CONFIG 		= 0,
	KEY_FEED		= 1,
	KEY_NUM			= 2,
}T_KEY_VALUE;

typedef enum
{
	KEY_EVENT_NULL  = 0x0,
	KEY_EVENT_UP	= 0x1,
	KEY_EVENT_DOWN	= 0x2,
	KEY_EVENT_LONG  = 0x3,
}T_KEY_EVENT;

typedef enum
{
	KEY_LOW    = 0,
	KEY_HIGH   = 1,
}T_KEY_LEVEL;

typedef enum
{
	KEY_DISABLE    = 0,
	KEY_ENABLE     = 1,
}T_KEY_SHIELD;

#define KEY_EVENT_LONG_COUNT		160		//���㳤��ʱ����Ŀǰkeyboard����ÿ50ms����һ�Ρ�


typedef struct
{
    T_KEY_SHIELD KeyShield;       //�������� 0:���Σ�1:������
	uint8_t KeyCount;             //������������
    T_KEY_LEVEL KeyLevel;         //���⵱ǰIO��ƽ������1��̧��0
    T_KEY_LEVEL KeyDownLevel;     //����ʱIOʵ�ʵĵ�ƽ
    T_KEY_STATUS KeyStatus;       //����״̬
    T_KEY_EVENT KeyEvent;         //�����¼�
	uint8_t KeyPin;               //����GPIO
}ST_KeyBoard;


int KeyBoard_Init(void);



#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif      /* _KEYBOARD_H_*/
