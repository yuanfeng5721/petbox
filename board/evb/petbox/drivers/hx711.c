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
#include "custom_log.h"

uint32_t HX711_Buffer;
uint32_t Weight_Maopi;
int32_t Weight_Shiwu;
uint8_t Flag_Error = 0;

//У׼����
//��Ϊ��ͬ�Ĵ������������߲��Ǻ�һ�£���ˣ�ÿһ����������Ҫ�������������������ʹ����ֵ��׼ȷ��
//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
//������Գ���������ƫСʱ����С����ֵ��
//��ֵ����ΪС��
#define GapValue 106.5

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
  */
void hx711_gpio_init(void)
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

uint32_t hx711_Read(void)	//����128
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
    count=count^0x800000;//��25�������½�����ʱ��ת������
	delay_us(1);
	hx711_set_clk(0);  
	return(count);
}

//****************************************************
//��ȡëƤ����
//****************************************************
void Get_Maopi(void)
{
	Weight_Maopi = hx711_Read();	
	//Weight_Maopi = 8321771;
	LOG_I("hx711 read data: %d \r\n", Weight_Maopi);
} 

//****************************************************
//����
//****************************************************
void Get_Weight(void)
{
	HX711_Buffer = hx711_Read();
	if(HX711_Buffer > Weight_Maopi)			
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;				//��ȡʵ���AD������ֵ��
	
		Weight_Shiwu = (int32_t)((float)Weight_Shiwu/GapValue); 	//����ʵ���ʵ������
																		//��Ϊ��ͬ�Ĵ������������߲�һ������ˣ�ÿһ����������Ҫ���������GapValue���������
																		//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
																		//������Գ���������ƫСʱ����С����ֵ��
	}

	
}