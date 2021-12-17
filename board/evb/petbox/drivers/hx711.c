#include <stdio.h>
#include <stdlib.h>
#include <os_msg.h>
#include <trace.h>
#include <app_msg.h>
#include <rtl876x.h>
#include <rtl876x_rcc.h>
#include <rtl876x_gpio.h>
#include <rtl876x_pinmux.h>
#include <platform_utils.h>
#include "board.h"
#include "os_timer.h"
#include "custom_log.h"
#include "custom_msg.h"
#include "hx711.h"

uint32_t HX711_Buffer;
uint32_t Weight_Maopi;
int32_t Weight_Shiwu;
uint8_t Flag_Error = 0;


void *weight_timer_handle;

//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数
//1400,5
//2810,10
//5100,20
//12100,50
//23400,100
//34900,150
//46680,200
//57762,250
//69550,300
//92650,400
//115500,500
//140000,600
//207330,900
//230500,1000
//346680,1500
//458050,2000
#define GapValue 231.5

void Get_Maopi(void);
uint32_t Get_Weight(void);

static void weight_timer_callback(void *param)
{
	uint32_t weight;
	weight = Get_Weight();
	//Get_Maopi();
	//Get_Weight();
	MAKE_CUSTOM_MSG_PARAM(msg, CUSTOM_MSG_CONTROL, CONTROL_MSG_REPORT_REMAIN, weight);
	control_send_msg(msg);
}

static void weight_timer(void)
{
	if (os_timer_create(&weight_timer_handle, "weight", CUSTOM_WEIGHT_TIMER_ID,
							   5*1000, true, weight_timer_callback) == true)
	{
		// Timer created successfully, start the timer.
		os_timer_start(&weight_timer_handle);
	}
	else
	{
		LOG_I("weight timer error!!!!\r\n");
	}
}

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
	
	delay_ms(10);
	
	Get_Maopi();
	weight_timer();
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

uint32_t hx711_Read(void)	//增益128
{
	uint32_t count; 
	uint8_t i; 
  	hx711_set_data(Bit_SET); 
	delay_us(5);
  	hx711_set_clk(Bit_RESET); 
	
  	count=0; 
  	while(hx711_get_data())
	{
		//timeout
	}
  	for(i=0;i<24;i++)
	{ 
	  	hx711_set_clk(Bit_SET); 
	  	count=count<<1; 
		delay_us(5);
		hx711_set_clk(Bit_RESET);
	  	if(hx711_get_data())
			count++; 
		delay_us(5);
	} 
 	hx711_set_clk(Bit_SET); 
    count=count^0x800000;//第25个脉冲下降沿来时，转换数据
	delay_us(5);
	hx711_set_clk(Bit_RESET);  
	return(count);
}

//****************************************************
//获取毛皮重量
//****************************************************
void Get_Maopi(void)
{
	Weight_Maopi = 0;
	
	for(int i=0; i<5; i++)
	{
		Weight_Maopi += hx711_Read();	
		delay_ms(100);
	}
	Weight_Maopi = Weight_Maopi/5;
	LOG_I("Maopi: %d \r\n", Weight_Maopi);
} 

//****************************************************
//称重
//****************************************************
uint32_t Get_Weight(void)
{
	Weight_Shiwu = 0;	
	HX711_Buffer = 0;
	
	for(int i=0; i<5; i++)
	{
		HX711_Buffer += hx711_Read();	
		delay_ms(100);
	}
	
	HX711_Buffer = HX711_Buffer/5;
	if(HX711_Buffer > Weight_Maopi)			
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;				//获取实物的AD采样数值。
		LOG_I("Weight raw: %d \r\n", HX711_Buffer);
		LOG_I("Weight-Maopi: %d \r\n", Weight_Shiwu);
		Weight_Shiwu = (int32_t)((float)Weight_Shiwu/GapValue); 	//计算实物的实际重量
																		//因为不同的传感器特性曲线不一样，因此，每一个传感器需要矫正这里的GapValue这个除数。
																		//当发现测试出来的重量偏大时，增加该数值。
		//LOG_I("Weight_Shiwu: %d g\r\n", Weight_Shiwu);																//如果测试出来的重量偏小时，减小改数值。
		if(Weight_Shiwu>MAX_WEIGHT)
		{
			Weight_Shiwu = 5000;
		}
	}
	
	LOG_I("Weight: %d g\r\n", Weight_Shiwu);
	return Weight_Shiwu;
}