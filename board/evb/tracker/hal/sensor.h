/*********************************************************************************************************
*/
/*============================================================================*
  *                     Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _SENSOR_H_
#define _SENSOR_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SENSOR_NUM   2

#define LIS2DH_NUM_ID    0
#define STK8XXX_NUM_ID   1  

int8_t sensor_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _SENSOR_H_ */