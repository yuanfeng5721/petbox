/*********************************************************************************************************
*/
/*============================================================================*
  *                     Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _I2C_H_
#define _I2C_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void i2c_init(uint8_t slaveAddr);
int i2c_write(uint8_t i2cx, uint8_t addr, uint8_t val);
int i2c_read(uint8_t i2cx, uint8_t addr, uint8_t *val);
#ifdef __cplusplus
}
#endif

#endif /* _I2C_H_ */
