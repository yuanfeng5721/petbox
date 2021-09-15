/* STK832X PSEUDO CODE
 * STK832X SA0/SDO(Pin1) Must be GND or connected to VDD and cannot be float.
 * #define STKI2C_ADDRESS	0x0F  //Pin1 connected to GND
 * #define STKI2C_ADDRESS	0x1F  //Pin1 connected to VDD
 * 
 * STK8BAxx No FIFO No SPI mode
 * #define STKI2C_ADDRESS	0x18
 * 
 */
#include "board.h"
#include "rtl876x_nvic.h"
#include "rtl876x_gpio.h"
#include "lis2dh12.h"
#include "lis2dh12registers.h"
#include "i2c.h"
#include "os_sched.h"
#include "custom_log.h"
#include "stk8xxx.h"

/* SPI mode needs to open this macro, IIC mode is blocked */
#define STK_SPI_MODE	0  

#define STK_REG_CHIPID	0x00
#define STK8325_CHIPID_VAL	0x25
#define STK832X_CHIPID_VAL	0x23    /* STK8XXX CHIP ID = 0x23 */
/* STK8BAxx */
#define STK8BA50_X_CHIPID_VAL	0x86   /* S or R resolution = 10 bit */
#define STK8BA5X_CHIPID_VAL	0x87

#define STK8XXX_PID_LIST_NUM	4
uint8_t stk8xxx_pid_list[STK8XXX_PID_LIST_NUM] = {STK8325_CHIPID_VAL, STK832X_CHIPID_VAL, STK8BA50_X_CHIPID_VAL, STK8BA5X_CHIPID_VAL};

#define STK8XXX_REG_RANGESEL	0x0F
#define STK8XXX_RANGE_2G		0x03
#define STK8XXX_RANGE_4G		0x05
#define STK8XXX_RANGE_8G		0x08

/* STK8XXX_REG_BWSEL */
#define STK8XXX_REG_BWSEL	0x10
#define STK8XXX_BWSEL_BW_7_81               0x08    /* ODR = BW x 2 = 15.62Hz */
#define STK8XXX_BWSEL_BW_15_63              0x09    /* ODR = BW x 2 = 31.26Hz */
#define STK8XXX_BWSEL_BW_31_25              0x0A    /* ODR = BW x 2 = 62.5Hz */
#define STK8XXX_BWSEL_BW_62_5               0x0B    /* ODR = BW x 2 = 125Hz */
#define STK8XXX_BWSEL_BW_125                0x0C    /* ODR = BW x 2 = 250Hz */
#define STK8XXX_BWSEL_BW_250                0x0D    /* ODR = BW x 2 = 500Hz */
#define STK8XXX_BWSEL_BW_500                0x0E    /* ODR = BW x 2 = 1000Hz */
#define STK8XXX_BWSEL_BW_1000               0x0F    /* ODR = BW x 2 = 2000Hz */

/* STK8XXX_REG_POWMODE */
#define STK8XXX_REG_PWMD	0x11
#define STK8XXX_PWMD_SUSPEND                0x80
#define STK8XXX_PWMD_LOWPOWER               0x40
#define STK8XXX_PWMD_NORMAL                 0x00

void delay_ms(uint16_t ms)
{
	os_delay(ms);
}

void WriteAccRegister(uint8_t reg, uint8_t val)
{
	i2c_write(0, reg, val);
}

void ReadAccRegister(uint8_t reg, uint8_t *val)
{
	i2c_read(0, reg, val);
}

/* Read WHO_AM_I register */
bool STK8XXX_Check_chipid(void)
{
	uint8_t temp = 0;
	unsigned char RegAddr = STK_REG_CHIPID;
    uint16_t i = 0, pid_num = (sizeof(stk8xxx_pid_list) / sizeof(stk8xxx_pid_list[0]));

    for (i = 0; i < pid_num; i++)
    {
    	ReadAccRegister(RegAddr, &temp);
        if (temp == stk8xxx_pid_list[i])
        {
        	LOG_I("read stkchip id ok, chip_id = 0x%x\n", temp);
            return true;
        }
    }
	LOG_E("read stkchip id fail, chip_id = 0x%x\n", temp);
    return false;
}

/*
 * Anymotion init register
 * Must be placed after the software reset
 *
 */
void STK8XXX_Anymotion_init()
{
	unsigned char ARegAddr, ARegWriteValue;
	
	/* Enable X Y Z-axis any-motion (slope) interrupt */
    ARegAddr       = 0x16;
    ARegWriteValue = 0x07;
    WriteAccRegister(ARegAddr, ARegWriteValue);

	/* Set anymotion Interrupt trigger threshold */
    ARegAddr       = 0x28;
    ARegWriteValue = 0x14; 
    WriteAccRegister(ARegAddr, ARegWriteValue);

	/* Enable any-motion */
    ARegAddr       = 0x2A;
    ARegWriteValue = 0x04; 
    WriteAccRegister(ARegAddr, ARegWriteValue);

	/* Map any-motion (slope) interrupt to INT1 */
    ARegAddr       = 0x19;
    ARegWriteValue = 0x04; 
    WriteAccRegister(ARegAddr, ARegWriteValue);

}
/*
 * Sigmotion init register
 * Must be placed after the software reset
 *
 */
void STK8XXX_Sigmotion_init()
{
	unsigned char SRegAddr, SRegWriteValue;
	
	/* Enable X Y Z-axis sig-motion (slope) interrupt */
    SRegAddr       = 0x16;
    SRegWriteValue = 0x07;
    WriteAccRegister(SRegAddr, SRegWriteValue);

	/* Set sig-motion Interrupt trigger threshold */
    SRegAddr       = 0x28;
    SRegWriteValue = 0x14; 
    WriteAccRegister(SRegAddr, SRegWriteValue);

	/* Enable significant motion */
    SRegAddr       = 0x2A;
    SRegWriteValue = 0x02; 
    WriteAccRegister(SRegAddr, SRegWriteValue);

	/* Map significant motion interrupt to INT1 */
    SRegAddr       = 0x19;
    SRegWriteValue = 0x01; 
    WriteAccRegister(SRegAddr, SRegWriteValue);

}

/* Disable Gsensor */
void STK8XXX_Suspend_mode()
{
	unsigned char RegAddr, RegWriteValue;
	
    /* suspend mode enable */
	RegAddr       = STK8XXX_REG_PWMD;
    RegWriteValue = STK8XXX_PWMD_SUSPEND;
    WriteAccRegister(RegAddr, RegWriteValue);
}

/*
 * Initializes an example function
 * The initial configuration is in active mode.
 * STK8XXX is in the gear of ODR=2000Hz, and the range is set to +/-4G
 *
 */
bool STK8XXX_Initialization()
{
    unsigned char RegAddr, RegWriteValue;

	if(STK8XXX_Check_chipid())
	{
		goto STK8XXX_SUCCESS_INIT;
	}else{
		goto STK8XXX_FAIL_INIT;
	}
	
STK8XXX_SUCCESS_INIT:	
    /* soft-reset */
	RegAddr       = 0x14;
    RegWriteValue = 0xB6;
    WriteAccRegister(RegAddr, RegWriteValue);
	delay_ms(50);//unit ms
	
#if STK_SPI_MODE
	uint8_t tmp_data = 0;
	/* set at SPI mode, resolution */
    RegAddr       = 0x70;
	RegWriteValue = 0x5A;
    ReadAccRegister(RegAddr, &tmp_data);
	if(0xE1 == tmp_data)
	{
		WriteAccRegister(0x33, RegWriteValue);
	}
#endif	
    /* set range, resolution */
    RegAddr       = STK8XXX_REG_RANGESEL;
    RegWriteValue = STK8XXX_RANGE_4G; 
    WriteAccRegister(RegAddr, RegWriteValue);

    /* set bandwidth */
    RegAddr       = STK8XXX_REG_BWSEL;
    RegWriteValue = STK8XXX_BWSEL_BW_1000; 
    WriteAccRegister(RegAddr, RegWriteValue);

	//STK8XXX_Anymotion_init();
	//STK8XXX_Sigmotion_init();

    /* set power mode */
    RegAddr       = STK8XXX_REG_PWMD;
    RegWriteValue = STK8XXX_PWMD_NORMAL;	// active mode
    WriteAccRegister(RegAddr, RegWriteValue);

    /* set i2c watch dog */
    RegAddr       = 0x34;
    RegWriteValue = 0x04; // enable watch dog
    WriteAccRegister(RegAddr, RegWriteValue);

    /* int config */
    RegAddr       = 0x20;
    RegWriteValue = 0x05; // INT1/INT2 push-pull, active high
    WriteAccRegister(RegAddr, RegWriteValue);

	LOG_D("STK8XXX_Initialization successful");
	return true;
STK8XXX_FAIL_INIT:
	LOG_D("STK8XXX_Initialization fail");
	return false;
}

/* Read data from registers */
void STK8XXX_Getregister_data(short int *X_DataOut, short int *Y_DataOut, short int *Z_DataOut)
{
    uint8_t RegAddr, RegReadValue[2];
	uint8_t temp = 0;
	
    RegAddr      	= 0x02;
    RegReadValue[0] = 0x00;
	ReadAccRegister(RegAddr, &RegReadValue[0]);
    RegAddr      	= 0x03;
    RegReadValue[1] = 0x00;
	ReadAccRegister(RegAddr, &RegReadValue[1]);
	*X_DataOut = (short int)RegReadValue[1] << 8 | RegReadValue[0]; 
	

	RegAddr      	= 0x04;
    RegReadValue[0] = 0x00;
	ReadAccRegister(RegAddr, &RegReadValue[0]);
    RegAddr      	= 0x05;
    RegReadValue[1] = 0x00;
	ReadAccRegister(RegAddr, &RegReadValue[1]);
	*Y_DataOut = (short int)RegReadValue[1] << 8 | RegReadValue[0]; 
	

	RegAddr      	= 0x06;
    RegReadValue[0] = 0x00;
	ReadAccRegister(RegAddr, &RegReadValue[0]);
    RegAddr      	= 0x07;
    RegReadValue[1] = 0x00;
	ReadAccRegister(RegAddr, &RegReadValue[1]);
	*Z_DataOut = (short int)RegReadValue[1] << 8 | RegReadValue[0]; 
	
	ReadAccRegister(0x00, &temp);
	if(0x86 == temp)	//resolution = 10 bit
	{
		*X_DataOut >>= 6;
		*Y_DataOut >>= 6;
		*Z_DataOut >>= 6;
	}else{				//resolution = 12 bit
		*X_DataOut >>= 4;
		*Y_DataOut >>= 4;
		*Z_DataOut >>= 4;
	}
	LOG_D("X_DataOut = %d,Y_DataOut = %d,Z_DataOut = %d\r\n",*X_DataOut,*Y_DataOut,*Z_DataOut);
}



