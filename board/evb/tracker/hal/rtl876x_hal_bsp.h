/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
  * @file    rtl876x_hal_bsp.h
  * @brief   Message definition for user application task.
  * @note    If new message types/sub types are to be added, add to the end of enumeration.
  *          The orders are NOT to changed.
  * @author  Lory
  * @date    2017.2.9
  * @version v1.0
*********************************************************************************************************
*/
/*============================================================================*
  *                     Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _RTL876X_HAL_BSP_H_
#define _RTL876X_HAL_BSP_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************/
typedef enum{
	START_TIMER_ID = 0x100,
	LEDS_TIMER_ID,
	SEC_TIMER_ID,    /* seconds base timer */
	MIN_TIMER_ID,    /* minutes base timer */
}CUSTOM_TIMER_ID;
/********************************************************************************/


#define BIT0        (1)
#define BLUE_LED	0x10
#define GREEN_LED   0x20
#define LED_OFF     0x00
#define LED_ON		0x01
#define LED_BLINK   0x02

typedef enum {
	BLUE_LED_OFF = BLUE_LED|LED_OFF,
	BLUE_LED_ON =  BLUE_LED|LED_ON,
	BLUE_LED_BLINK = BLUE_LED|LED_BLINK,
	GREEN_LED_OFF = GREEN_LED|LED_OFF,
	GREEN_LED_ON = GREEN_LED|LED_ON,
	GREEN_LED_BLINK = GREEN_LED|LED_BLINK,
}LedEvent;

typedef enum {
	LED_BLUE = 0,
	LED_GREEN,
	LED_MAX,
} Leds_t;

typedef void (*led_ctl)(uint8_t onoff);

extern led_ctl led_ctl_tab[LED_MAX];

void board_gpio_init(void);
void sys_3v8_power(uint8_t onoff);
void modem_power(uint8_t onoff);
void gnss_power(uint8_t onoff);
void green_led_ctl(uint8_t onoff);
void leds_init(void);
void blue_led_ctl(uint8_t onoff);


#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_HAL_BSP_H_ */
