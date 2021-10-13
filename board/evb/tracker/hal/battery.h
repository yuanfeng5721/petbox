/*********************************************************************************************************
*/
/*============================================================================*
  *                     Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _BATTERY_H_
#define _BATTERY_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
	uint16_t voltage;
	uint8_t  cap;
} T_BATTERY_CAP_MAP;

void battery_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _BATTERY_H_ */
