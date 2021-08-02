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

void board_gpio_init(void);
void sys_3v8_power(uint8_t onoff);
void modem_power(uint8_t onoff);
void gnss_power(uint8_t onoff);
void green_led_ctl(uint8_t onoff);
void blue_led_ctl(uint8_t onoff);


#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_HAL_BSP_H_ */