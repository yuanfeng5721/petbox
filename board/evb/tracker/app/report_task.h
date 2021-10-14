/*********************************************************************************************************
*/
/*============================================================================*
  *                     Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _REPORT_TASK_H_
#define _REPORT_TASK_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void report_task_init(void);
int report_send(uint8_t *pdata, size_t dsize, uint32_t wait_ms);

#ifdef __cplusplus
}
#endif

#endif /* _REPORT_TASK_H_ */
