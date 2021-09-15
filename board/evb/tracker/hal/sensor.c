#include "board.h"
#include "rtl876x_nvic.h"
#include "rtl876x_gpio.h"
#include "i2c.h"
#include "os_sched.h"
#include "custom_log.h"

#include "stk8xxx.h"
#include "lis2dh12.h"
#include "sensor.h"

static uint8_t slaveaddr[MAX_SENSOR_NUM] = {LIS2_ADDRESS, STK8XXX_SLAVEADDR};
	
void sensor_gpio_init(void)
{
	GPIO_InitTypeDef Gpio_Struct;
    GPIO_StructInit(&Gpio_Struct);

    Gpio_Struct.GPIO_Pin = GPIO_GetPin(GSENSOR_INT_PIN);
    Gpio_Struct.GPIO_Mode = GPIO_Mode_IN;
    Gpio_Struct.GPIO_ITCmd = ENABLE;
    Gpio_Struct.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
    Gpio_Struct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    Gpio_Struct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
    Gpio_Struct.GPIO_DebounceTime = 0;
    GPIO_Init(&Gpio_Struct);

    GPIO_MaskINTConfig(GPIO_GetPin(GSENSOR_INT_PIN), DISABLE);
    GPIO_INTConfig(GPIO_GetPin(GSENSOR_INT_PIN), ENABLE);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = GSENSOR_IRQ;//P2_4
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

int8_t sensor_init(void)
{
	uint8_t i;
	
	
	//sensor_gpio_init();
	for(i=0; i<MAX_SENSOR_NUM; i++) {
		i2c_init(slaveaddr[i]);
		if(i == LIS2DH_NUM_ID) {
			if(u8LIS2_TestRead())
			{
				vLIS2_Init();
				break;
			}
		} else if(i == STK8XXX_NUM_ID) {
			if(STK8XXX_Initialization())
			{
				break;
			}
		}
	}
	if(i >= MAX_SENSOR_NUM) {
		LOG_E("Gsensor dect error!\r\n");
		return -1;
	} else {
		LOG_E("Gsensor dect is %s!\r\n", (i==LIS2DH_NUM_ID)?"lis2dh":"stk8xxx");
		return i;
	}
}