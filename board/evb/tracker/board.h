/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file     board.h
* @brief        Pin definitions
* @details
* @author   Chuanguo Xue
* @date     2015-4-7
* @version  v0.1
* *********************************************************************************************************
*/

#ifndef _BOARD_H_
#define _BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* include file */
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"

/* GPIO function define begin */
#define DATA_UART_TX_PIN     P3_0
#define DATA_UART_RX_PIN     P3_1

#define I2C0_SCL_PIN     	 P3_2
#define I2C0_SDA_PIN     	 P3_4

#define GSENSOR_INT_PIN      P0_4
#define GSENSOR_IRQ			 GPIO4_IRQn

#define SYS_3V8_CTL_PIN      P3_3
#define SYS_3V8_CTL_OUTPUT   GPIO_GetPin(SYS_3V8_CTL_PIN)

#define MODEM_PWR_CTL_PIN    P2_1
#define MODEM_PWR_CTL_OUTPUT GPIO_GetPin(MODEM_PWR_CTL_PIN)

#define AT_UART_TX_PIN    	 P4_0
#define AT_UART_RX_PIN    	 P4_1

#define MODEM_PWR_KEY_PIN    P2_3
#define MODEM_PWR_KEY_OUTPUT GPIO_GetPin(MODEM_PWR_KEY_PIN)
#define MODEM_STATUS_PIN     P2_2
#define MODEM_STATUS_INPUT   GPIO_GetPin(MODEM_STATUS_PIN)

#define GNSS_PWR_CTL_PIN     P2_4
#define GNSS_PWR_CTL_OUTPUT  GPIO_GetPin(GNSS_PWR_CTL_PIN)

#define GNSS_LAN_CTL_PIN     P3_5
#define GNSS_LAN_CTL_OUTPUT  GPIO_GetPin(GNSS_LAN_CTL_PIN)

#define BLUE_LED_PIN         P0_0
#define BLUE_LED_OUTPUT      GPIO_GetPin(BLUE_LED_PIN)

#define GREEN_LED_PIN        P0_2
#define GREEN_LED_OUTPUT     GPIO_GetPin(GREEN_LED_PIN)

#define KEY_PAD_PIN          P0_1
#define KEY_PAD_INPUT        GPIO_GetPin(KEY_PAD_PIN)

/* GPIO function define end */

/* custom define */
#define PIN_LOW            0
#define PIN_HIGH           1

/* if use user define dlps enter/dlps exit callback function */
#define USE_USER_DEFINE_DLPS_EXIT_CB      0
#define USE_USER_DEFINE_DLPS_ENTER_CB     0

/* if use any peripherals below, #define it 1 */
#define USE_I2C0_DLPS        0
#define USE_I2C1_DLPS        0
#define USE_TIM_DLPS         0
#define USE_QDECODER_DLPS    0
#define USE_IR_DLPS          0
#define USE_RTC_DLPS         0
#define USE_UART_DLPS        0
#define USE_ADC_DLPS         0
#define USE_SPI0_DLPS        0
#define USE_SPI1_DLPS        0
#define USE_SPI2W_DLPS       0
#define USE_KEYSCAN_DLPS     0
#define USE_DMIC_DLPS        0
#define USE_GPIO_DLPS        0
#define USE_PWM0_DLPS        0
#define USE_PWM1_DLPS        0
#define USE_PWM2_DLPS        0
#define USE_PWM3_DLPS        0


/* do not modify USE_IO_DRIVER_DLPS macro */
#define USE_IO_DRIVER_DLPS   (USE_I2C0_DLPS | USE_I2C1_DLPS | USE_TIM_DLPS | USE_QDECODER_DLPS\
                              | USE_IR_DLPS | USE_RTC_DLPS | USE_UART_DLPS | USE_SPI0_DLPS\
                              | USE_SPI1_DLPS | USE_SPI2W_DLPS | USE_KEYSCAN_DLPS | USE_DMIC_DLPS\
                              | USE_GPIO_DLPS | USE_USER_DEFINE_DLPS_EXIT_CB\
                              | USE_RTC_DLPS | USE_PWM0_DLPS | USE_PWM1_DLPS | USE_PWM2_DLPS\
                              | USE_PWM3_DLPS | USE_USER_DEFINE_DLPS_ENTER_CB)

#ifdef __cplusplus
}
#endif

#endif

