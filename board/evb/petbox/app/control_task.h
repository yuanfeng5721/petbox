/*********************************************************************************************************
*/
/*============================================================================*
  *                     Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _CONTROL_TASK_H_
#define _CONTROL_TASK_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

void control_task_init(void);

uint32_t control_feed_num(uint32_t num);
uint32_t control_feed_weight(uint32_t weight);
uint8_t control_history(uint8_t history);
uint8_t control_feed_water(uint8_t water);
bool control_set_auto_feed(bool state);
bool control_get_auto_feed(void);

#ifdef __cplusplus
}
#endif

#endif /* _CONTROL_TASK_H_ */
