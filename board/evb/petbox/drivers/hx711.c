#include <stdio.h>
#include <stdlib.h>
#include <os_msg.h>
#include <trace.h>
#include <app_msg.h>
#include <board.h>
#include <rtl876x.h>
#include <rtl876x_rcc.h>
#include <rtl876x_gpio.h>
#include <rtl876x_pinmux.h>
#include <platform_utils.h>


uint32_t HX711_Buffer;
uint32_t Weight_Maopi;
int32_t Weight_Shiwu;
uint8_t Flag_Error = 0;

//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数
#define GapValue 106.5

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
  */
void board_gpio_init(void)
{
    Pad_Config(HX711_SCK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pinmux_Config(HX711_SCK, DWGPIO);
	
	Pad_Config(HX711_DAT, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pinmux_Config(HX711_DAT, DWGPIO);
	
	/* Initialize GPIO peripheral */
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin    = GPIO_GetPin(HX711_SCK);
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);
	
	GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin    = GPIO_GetPin(HX711_DAT);
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);
	
	GPIO_WriteBit(GPIO_GetPin(HX711_SCK), (BitAction)(0));
}


uint8_t hx711_get_data(void)
{
	return GPIO_ReadInputDataBit(GPIO_GetPin(HX711_DAT));
}

void hx711_set_data(BitAction BitVal)
{
	GPIO_WriteBit(GPIO_GetPin(HX711_DAT), BitVal);
}

void hx711_set_clk(BitAction BitVal)
{
	GPIO_WriteBit(GPIO_GetPin(HX711_SCK), BitVal);
}

void delay_us(uint32_t us)
{
	platform_delay_us(us);
}

uint32_t hx711_Read(void)	//增益128
{
	uint32_t count; 
	uint8_t i; 
  	hx711_set_data(Bit_SET); 
	delay_us(1);
  	hx711_set_clk(0); 
	
  	count=0; 
  	while(hx711_get_data()); 
  	for(i=0;i<24;i++)
	{ 
	  	hx711_set_clk(1); 
	  	count=count<<1; 
		delay_us(1);
		hx711_set_clk(0);
	  	if(hx711_get_data())
			count++; 
		delay_us(1);
	} 
 	hx711_set_clk(1); 
    count=count^0x800000;//第25个脉冲下降沿来时，转换数据
	delay_us(1);
	hx711_set_clk(0);  
	return(count);
}

//****************************************************
//获取毛皮重量
//****************************************************
void Get_Maopi(void)
{
	Weight_Maopi = hx711_Read();	
	//Weight_Maopi = 8321771;
} 

//****************************************************
//称重
//****************************************************
void Get_Weight(void)
{
	HX711_Buffer = hx711_Read();
	if(HX711_Buffer > Weight_Maopi)			
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;				//获取实物的AD采样数值。
	
		Weight_Shiwu = (int32_t)((float)Weight_Shiwu/GapValue); 	//计算实物的实际重量
																		//因为不同的传感器特性曲线不一样，因此，每一个传感器需要矫正这里的GapValue这个除数。
																		//当发现测试出来的重量偏大时，增加该数值。
																		//如果测试出来的重量偏小时，减小改数值。
	}

	
}