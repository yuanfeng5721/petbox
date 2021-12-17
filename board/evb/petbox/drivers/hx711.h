/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     hx711.h
* @brief    Data uart operations for testing multilink.
* @details  Data uart init and print data through data uart.
* @author   wangkun
* @date     2021-12-07
* @version  v0.1
*********************************************************************************************************
*/
#ifndef _HX711_H_
#define _HX711_H_

#include <stdint.h>
#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */
#define MAX_WEIGHT  5000

void hx711_gpio_init(void);
void Get_Maopi(void);
  
/** @} */ /* End of group DATA_UART_APIs */
/** @} */ /* End of group DATA_UART_CMD */
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif

