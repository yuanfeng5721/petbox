/*********************************************************************************************************
*/
/*============================================================================*
  *                     Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _STK8XXX_H_
#define _STK8XXX_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STK8XXX_SLAVEADDR   0x1f

bool STK8XXX_Check_chipid(void);
bool STK8XXX_Initialization();
#ifdef __cplusplus
}
#endif

#endif /* _STK8XXX_H_ */





