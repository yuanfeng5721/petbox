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
#define USE_LOCK_BOARD

/* include file */
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include <platform_utils.h>

/* GPIO function define begin */
#define DATA_UART_TX_PIN     P3_0
#define DATA_UART_RX_PIN     P3_1

#ifndef USE_LOCK_BOARD
#define AT_UART_TX_PIN    	 P4_0
#define AT_UART_RX_PIN    	 P4_1
#else
//#define AT_UART_TX_PIN    	 P2_4
//#define AT_UART_RX_PIN    	 P2_5
#define AT_UART_TX_PIN    	 P4_1//P4_2
#define AT_UART_RX_PIN    	 P4_2//P4_1
#endif

#define HX711_SCK            P2_4
#define HX711_DAT            P2_5

#define CONFIG_KEY_PIN			 P1_6
#define FEED_KEY_PIN			 P2_3

//feed water control pin
#define WATER_PUMP_EN_PIN    P4_0   //output
#define WATER_PUMP_LED_PIN   P0_7	//output
#define WATER_LEVEL_M_PIN	 P4_3	//input
#define WATER_LEVEL_L_PIN    P0_6	//input
#define WATER_AUTO_DET_PIN   P0_2//P0_1	//input

//feed moto control pin
#define FEED_MOTO_EN_PIN     P0_0	//output
#define FEED_MOTO_CTL1_PIN   P2_6	//output
#define FEED_MOTO_CTL2_PIN   P2_7	//output

#define FEED_BUCKET_DET_PIN  P0_4	//input
#define FEED_AUTO_DET_PIN    P0_1//P0_2	//input
#define FEED_NUM_EN_PIN		 P3_2   //output
#define FEED_NUM_COUNT_PIN   P3_3   //input

#define FEED_STUCK_DET_TX_PIN   P2_1 //output
#define FEED_STUCK_DET_RX_PIN   P2_2 //input

//pad pinmux define
#define PNUM(PIN)  GPIO_GetPin(PIN)

#define PAD_CFG_GPIO(PIN, PULL, DIR)  \
		Pad_Config(PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PULL, DIR, (DIR==PAD_OUT_ENABLE)?PAD_OUT_HIGH:PAD_OUT_LOW); \
		Pinmux_Config(PIN, DWGPIO);

#define GPIO_SET(PIN, VAL) GPIO_WriteBit(PNUM(PIN), (BitAction)(VAL)) 
#define GPIO_GET(PIN)  GPIO_ReadInputDataBit(PNUM(PIN))

#define delay_us(us) platform_delay_us(us)
//inline void delay_us(uint32_t us)
//{
//	platform_delay_us(us);
//}
#define delay_ms(ms) platform_delay_ms(ms)
//inline void delay_ms(uint32_t ms)
//{
//	platform_delay_ms(ms);
//}

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

