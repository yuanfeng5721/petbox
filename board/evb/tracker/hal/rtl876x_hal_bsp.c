/**
*****************************************************************************************
*     Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      main.c
   * @brief     Source file for BLE BT5 central project, mainly used for initialize modules
   * @author    berni
   * @date      2018-04-27
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2018 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <board.h>
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_hal_bsp.h"

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
  */
void board_gpio_init(void)
{
	//system 3.8v power
    Pad_Config(SYS_3V8_CTL_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(SYS_3V8_CTL_PIN, DWGPIO);
	
	//modem power ,power key and status
	Pad_Config(MODEM_PWR_CTL_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(MODEM_PWR_CTL_PIN, DWGPIO);
	
	Pad_Config(MODEM_PWR_KEY_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_LOW);
    Pinmux_Config(MODEM_PWR_KEY_PIN, DWGPIO);
	
	Pad_Config(MODEM_STATUS_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pinmux_Config(MODEM_STATUS_PIN, DWGPIO);
	
	//led ctl pin
	Pad_Config(BLUE_LED_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(BLUE_LED_PIN, DWGPIO);
	
	Pad_Config(GREEN_LED_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(GREEN_LED_PIN, DWGPIO);
	
	//GNSS power and lan ctl
	Pad_Config(GNSS_PWR_CTL_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(GNSS_PWR_CTL_PIN, DWGPIO);
	
	Pad_Config(GNSS_LAN_CTL_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(GNSS_LAN_CTL_PIN, DWGPIO);
	
	/* Initialize GPIO peripheral */
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin    = SYS_3V8_CTL_OUTPUT | MODEM_PWR_CTL_OUTPUT | MODEM_PWR_KEY_OUTPUT | GNSS_PWR_CTL_OUTPUT | GNSS_LAN_CTL_OUTPUT | BLUE_LED_OUTPUT | GREEN_LED_OUTPUT;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);
	
	GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin    = MODEM_STATUS_INPUT;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);
	
	GPIO_WriteBit(GNSS_PWR_CTL_OUTPUT, (BitAction)(0));
	GPIO_WriteBit(SYS_3V8_CTL_OUTPUT, (BitAction)(0));
	GPIO_WriteBit(MODEM_PWR_CTL_OUTPUT, (BitAction)(0));
	GPIO_WriteBit(GREEN_LED_OUTPUT, (BitAction)(0));
	GPIO_WriteBit(BLUE_LED_OUTPUT, (BitAction)(0));
}

/**
  * @brief  Initialize GPIO peripheral.
  * @param  No parameter.
  * @return void
  */
void sys_3v8_power(uint8_t onoff)
{
	if(onoff)
		GPIO_WriteBit(SYS_3V8_CTL_OUTPUT, (BitAction)(1));
	else
		GPIO_WriteBit(SYS_3V8_CTL_OUTPUT, (BitAction)(0));
}

void modem_power(uint8_t onoff)
{
	if(onoff)
		GPIO_WriteBit(MODEM_PWR_CTL_OUTPUT, (BitAction)(1));
	else
		GPIO_WriteBit(MODEM_PWR_CTL_OUTPUT, (BitAction)(0));
}

void gnss_power(uint8_t onoff)
{
	if(onoff)
		GPIO_WriteBit(GNSS_PWR_CTL_OUTPUT, (BitAction)(1));
	else
		GPIO_WriteBit(GNSS_PWR_CTL_OUTPUT, (BitAction)(0));
}

void green_led_ctl(uint8_t onoff)
{
	if(onoff)
		GPIO_WriteBit(GREEN_LED_OUTPUT, (BitAction)(1));
	else
		GPIO_WriteBit(GREEN_LED_OUTPUT, (BitAction)(0));
}

void blue_led_ctl(uint8_t onoff)
{
	if(onoff)
		GPIO_WriteBit(BLUE_LED_OUTPUT, (BitAction)(1));
	else
		GPIO_WriteBit(BLUE_LED_OUTPUT, (BitAction)(0));
}

