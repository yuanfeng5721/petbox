
#include <stdio.h>

#include "board.h"
#include "rtl876x_i2c.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "custom_log.h"

void i2c_init(uint8_t slaveAddr)
{
	I2C_InitTypeDef  I2C_InitStruct;
    I2C_StructInit(&I2C_InitStruct);
	
	//init gpio
	RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
	Pad_Config(I2C0_SCL_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(I2C0_SDA_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(GSENSOR_INT_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);

	Pinmux_Config(I2C0_SCL_PIN, I2C0_CLK);
	Pinmux_Config(I2C0_SDA_PIN, I2C0_DAT);
	Pinmux_Config(GSENSOR_INT_PIN, DWGPIO);
	
	//init i2c
	RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);

    I2C_InitStruct.I2C_ClockSpeed           = 10000;
    I2C_InitStruct.I2C_DeviveMode           = I2C_DeviveMode_Master;
    I2C_InitStruct.I2C_AddressMode          = I2C_AddressMode_7BIT;
    I2C_InitStruct.I2C_SlaveAddress         = slaveAddr;
    I2C_InitStruct.I2C_Ack                  = I2C_Ack_Enable;
    I2C_InitStruct.I2C_RxThresholdLevel = 12;

    I2C_Init(I2C0, &I2C_InitStruct);
    I2C_Cmd(I2C0, ENABLE);
}


int i2c_write(uint8_t i2cx, uint8_t addr, uint8_t val)
{
	uint8_t buf[2] = {addr, val};
	
	if(i2cx == 0) {
		return (I2C_MasterWrite(I2C0, buf, 2)==I2C_Success)?0:-1;
	} else if(i2cx == 1) {
		return (I2C_MasterWrite(I2C1, buf, 2)==I2C_Success)?0:-1;
	} else {
		LOG_E("i2c_write:i2c instance error!\r\n");
		return -1;
	}
}

int i2c_read(uint8_t i2cx, uint8_t addr, uint8_t *val)
{
	if(i2cx == 0) {
		return (I2C_RepeatRead(I2C0, &addr, 1, val, 1)==I2C_Success)?0:-1;
	} else if(i2cx == 1) {
		return (I2C_RepeatRead(I2C1, &addr, 1, val, 1)==I2C_Success)?0:-1;	
	} else {
		LOG_E("i2c_read:i2c instance error!\r\n");
		return -1;
	}
}
