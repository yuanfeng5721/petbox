/*********************************************************************************************************
*/
/*============================================================================*
  *                     Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _CONTROL_H_
#define _CONTROL_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdint.h>
#include "rtl876x_nvic.h"
#ifdef __cplusplus
extern "C" {
#endif

//#define WATER_LEVEL_M_HANDLER    GPIO31_Handler
//#define WATER_LEVEL_L_HANDLER    GPIO6_Handler
#define FEED_WATER_DET_HANDLER   GPIO2_Handler//GPIO1_Handler
#define FEED_WATER_DET_IRQn		 GPIO2_IRQn//GPIO1_IRQn
//#define FEED_BUCKET_DET_HANDLER  GPIO4_Handler
#define FEED_FOOD_DET_HANDLER    GPIO1_Handler//GPIO2_Handler
#define FEED_FOOD_DET_IRQn		 GPIO1_IRQn//GPIO2_IRQn
#define FOOD_STUCK_DET_HANDLER   GPIO18_Handler
#define FOOD_STUCK_DET_IRQn		 GPIO18_IRQn
#define FEED_NUM_COUNT_HANDLER   GPIO27_Handler
#define FEED_NUM_COUNT_IRQn      GPIO27_IRQn

#define EXCEPTION_CHECK_FREQ    (5*1000)  //5S
#define EXCEPTION_REPORT_FREQ   (60*1000*1)

void feed_food(uint8_t num);
void feed_weight(uint32_t weight);
void feed_history(uint8_t history);
void feed_water(bool status);

void Control_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* _CONTROL_H_ */
