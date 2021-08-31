/*
*******************************************************************************
*
*     Copyright (c) 2021 btlink Ltd.
*
*******************************************************************************
*  file name:           btlink_assem_msg.c
*  author:              Eric
*  version:             1.00
*  file description:
*******************************************************************************
*  revision history:    date               version                  author
*
*  change summary:
*
*******************************************************************************
*/

/******************************************************************************
* Include Files
******************************************************************************/
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "btlink_trace.h"
#include "btlink_assem_hex_msg_enums.h"
#include "btlink_assem_hex_msg_struct.h"
#include "btlink_assem_asc_msg_struct.h"
#include "btlink_protocol_cmd.h"
#include "btlink_protocol_util.h"
#include "btlink_protocol_verno.h"

/*****************************************************************************
* Define
*****************************************************************************/

/*****************************************************************************
* Typedef
*****************************************************************************/

/*****************************************************************************
* Extern Variable
*****************************************************************************/

/*****************************************************************************
* Extern Function
*****************************************************************************/

/*****************************************************************************
 * global variable
 *****************************************************************************/

/*****************************************************************************
 * Local variable
 *****************************************************************************/
uint8_t   g_hex_imei[BTLINK_HEX_IMEI_LEN];    /*for @Tracker HEX protocol*/
btlink_cell_info_struct g_cell_info = {0};

/*****************************************************************************
 * Local function
 *****************************************************************************/
/******************************************************************************
* Function    : btlink_hex_pack_u8
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/ 
uint8_t btlink_hex_pack_u8(uint8_t* buff, uint8_t c) 
{ 
	buff[0] = c;
	return 1; 
}
/*Caution: string can not be longer than 255 bytes*/ 
/******************************************************************************
* Function    : btlink_hex_pack_str
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/ 
static uint8_t btlink_hex_pack_str(uint8_t* buff, uint8_t* str) 
{ 
	strcpy((char*)buff, (char*)str);
	return strlen((char*)str); 
}
/******************************************************************************
* Function    : btlink_hex_pack_hex_str
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/ 
static uint8_t btlink_hex_pack_hex_str(uint8_t* buff, uint8_t* str) 
{ 
	uint32_t i, j, n;
	uint8_t temp_str[256];
	memset(temp_str, 0, sizeof(temp_str));
	strcpy((char*)temp_str, (char*)str);
	n = strlen((char*)temp_str);
	for (i=0; i<n; i++)
	{ 
		if (temp_str[i] <= '9' && temp_str[i] >= '0')
		{ 
			temp_str[i] -= '0'; 
		}
		else if (temp_str[i] <= 'f' && temp_str[i] >= 'a')
		{ 
			temp_str[i] = temp_str[i] - 'a' + 10; 
		}
		else if (temp_str[i] <= 'F' && temp_str[i] >= 'A')
		{ 
			temp_str[i] = temp_str[i] - 'A' + 10; 
		} 
	}
	
	for (i=0,j=0; i<n; i+=2,j++)
	{ 
		buff[j] = (temp_str[i] <<4) | temp_str[i+1]; 
	}
	
	return n/2; 
}
/******************************************************************************
* Function    : btlink_hex_unpack_u16
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/ 
uint16_t btlink_hex_unpack_u16(uint8_t* buff, bool byte_reverse) 
{ 
	btlink_u16_u8_union u16;
	if (byte_reverse)
	{ 
		u16.c[1] = buff[0];
		u16.c[0] = buff[1]; 
	}
	else
	{ 
		u16.c[0] = buff[0];
		u16.c[1] = buff[1]; 
	}
	
	return u16.num; 
}
/******************************************************************************
* Function    : btlink_hex_unpack_u32
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/ 
uint32_t btlink_hex_unpack_u32(uint8_t* buff, bool byte_reverse) 
{ 
	btlink_u32_u8_union u32;
	if (byte_reverse)
	{ 
		u32.c[3] = buff[0];
		u32.c[2] = buff[1];
		u32.c[1] = buff[2];
		u32.c[0] = buff[3]; 
	}
	else
	{ 
		u32.c[0] = buff[0];
		u32.c[1] = buff[1];
		u32.c[2] = buff[2];
		u32.c[3] = buff[3]; 
	}
	return u32.num; 
}
/******************************************************************************
* Function    : btlink_hex_pack_u16
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/ 
uint8_t btlink_hex_pack_u16(uint8_t* buff, uint16_t u, bool byte_reverse) 
{ 
	btlink_u16_u8_union u16;
	u16.num = u;
	if (byte_reverse)
	{ 
		buff[0] = u16.c[1];
		buff[1] = u16.c[0]; 
	}
	else
	{ 
		buff[0] = u16.c[0];
		buff[1] = u16.c[1]; 
	}
	return 2; 
}
/******************************************************************************
* Function    : btlink_hex_pack_u32
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/ 
uint8_t btlink_hex_pack_u32(uint8_t* buff, uint32_t u, bool byte_reverse) 
{ 
	btlink_u32_u8_union u32;
	uint8_t i;
	u32.num = u;
	if (byte_reverse)
	{ for (i = 0; i < 4; i++)
		{ 
			buff[i] = u32.c[3 - i]; 
		} 
	}
	else 
	{ 
		for (i = 0; i < 4; i++) 
		{ 
			buff[i] = u32.c[i]; 
		} 
	}
	return 4; 
}
/******************************************************************************
* Function    : btlink_hex_pack_s16
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/ 
uint8_t btlink_hex_pack_s16(uint8_t* buff, int16_t s, bool byte_reverse) 
{ 
	btlink_s32_u8_union s16;
	s16.num = s;
	if (byte_reverse)
	{ 
		buff[0] = s16.c[1];
		buff[1] = s16.c[0]; 
	}
	else
	{ 
		buff[0] = s16.c[0];
		buff[1] = s16.c[1]; 
	}
	return 2; 
}
/******************************************************************************
* Function    : btlink_hex_pack_s32
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/ 
uint8_t btlink_hex_pack_s32(uint8_t* buff, int32_t s, bool byte_reverse) 
{ 
	btlink_s32_u8_union s32;
	uint8_t i;
	s32.num = s;
	if (byte_reverse)
	{ 
		for (i = 0; i < 4; i++)
		{ 
			buff[i] = s32.c[3 - i]; 
		} 
	}
	else
	{ 
		for (i = 0; i < 4; i++)
		{ 
			buff[i] = s32.c[i]; 
		} 
	}
	return 4; 
} 

/******************************************************************************
* Function    : btlink_hex_pack_number_of_point
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_number_of_point(uint8_t* buff, uint8_t number)
{
    return btlink_hex_pack_u8(buff, number);
}

/******************************************************************************
* Function    : btlink_hex_pack_gps_accuracy
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_gps_accuracy(uint8_t* buff, uint8_t hdop)
{
    return btlink_hex_pack_u8(buff, hdop);
}

/******************************************************************************
* Function    : btlink_hex_pack_speed
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_speed(uint8_t* buff, uint8_t* speed)
{
    uint16_t integer;
    uint8_t  fraction;
    char* pdot;
    uint8_t* pbuff;

    integer = atoi((char*)speed);
    pdot = strchr((char*)speed, '.');
    if (pdot != NULL)
    {
        fraction = atoi(pdot+1);
    }
    else
    {
        fraction = 0;
    }

    pbuff = buff;
    pbuff += btlink_hex_pack_u16(pbuff, integer, true);
    pbuff += btlink_hex_pack_u8(pbuff, fraction);

    return (pbuff - buff);
}

/******************************************************************************
* Function    : btlink_hex_pack_heading
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_heading(uint8_t* buff, uint8_t* heading)
{
    return btlink_hex_pack_u16(buff, atoi((char*)heading), true);
}


/******************************************************************************
* Function    : btlink_hex_pack_altitude
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_altitude(uint8_t* buff, uint8_t* altitude)
{
    uint16_t integer;
    uint8_t  fraction;
    char* pdot;
    uint8_t negative;

    if (altitude[0] == '-')
    {
        negative = 1;
    }
    else
    {
        negative = 0;
    }

    integer = atoi((char*)altitude);
    pdot = strchr((char*)altitude, '.');
    if (pdot != NULL)
    {
        fraction = atoi(pdot+1);
    }
    else
    {
        fraction = 0;
    }

    if (5 <= fraction && fraction <= 9 )
    {
        if (negative)
        {
            if (integer)
            {
                integer--;
            }
            else
            {
                integer = (uint16_t)-1;
            }
        }
        else
        {
            integer++;
        }
    }

    return btlink_hex_pack_u16(buff, integer, true);
}

/******************************************************************************
* Function    : btlink_hex_pack_longitude
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_longitude(uint8_t* buff, uint8_t* lng)
{
    int32_t lng32;

    lng32 = (int32_t)(atof((char*)lng) * BTLINK_HEX_LNGLAT_PRECISION);

    return btlink_hex_pack_s32(buff, lng32, true);
}

/******************************************************************************
* Function    : btlink_hex_pack_latitude
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_latitude(uint8_t* buff, uint8_t* lat)
{
    int32_t lat32;

    lat32 = (int32_t)(atof((char*)lat) * BTLINK_HEX_LNGLAT_PRECISION);

    return btlink_hex_pack_s32(buff, lat32, true);
}

/******************************************************************************
* Function    : btlink_hex_pack_time
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_time(uint8_t* buff, uint8_t* time)
{
    uint8_t* pbuff = buff;
    uint8_t* ptime = time;
    char num[5] = {0};
    uint8_t i;

    snprintf(num, 4+1, "%s", ptime);
    pbuff += btlink_hex_pack_u16(pbuff, atoi(num), true);

    ptime += 4;
    for (i = 0; i < 5; i++)
    {
        memset(num, 0, 5);
        snprintf(num, 2+1, "%s", (ptime + i * 2));
        pbuff += btlink_hex_pack_u8(pbuff, atoi(num));
    }

    return (pbuff - buff);
}

static uint8_t btlink_hex_pack_gps_info(uint8_t* buff, btlink_gps_info_struct* gps_pos)
{
    uint8_t* pbuff;

    pbuff = buff;

    //Accuracy
    buff += btlink_hex_pack_gps_accuracy(buff, 0);
	
	  //GPS UTC
    buff += btlink_hex_pack_time(buff, gps_pos->time);
	
    //Longitude & Latitude
		buff += btlink_hex_pack_longitude(buff, gps_pos->lng);
		buff += btlink_hex_pack_latitude(buff, gps_pos->lat);
	
    //Speed
		buff += btlink_hex_pack_speed(buff, gps_pos->speed);
    
    //Heading
    buff += btlink_hex_pack_heading(buff, gps_pos->heading);
	
    //Altitude
    buff += btlink_hex_pack_altitude(buff, gps_pos->altitude);

    return (buff - pbuff);
}

/******************************************************************************
* Function    : btlink_hex_pack_device_type
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_device_type(uint8_t* buff, uint8_t device_type)
{
	 return btlink_hex_pack_u8(buff, device_type);
}

/******************************************************************************
* Function    : btlink_hex_pack_device_type
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_verno(uint8_t* buff)
{
	uint8_t* pbuff;
	pbuff = buff;
	
	/* Mar Version */
	pbuff += btlink_hex_pack_u8(buff, BTLINK_SW_MAR_VER);
	
	/* Min Version */
	pbuff += btlink_hex_pack_u8(buff, BTLINK_SW_MIN_VER);

	return (buff - pbuff);
}

/******************************************************************************
* Function    : btlink_hex_pack_battery_level
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_battery_level(uint8_t* buff)
{
	 return btlink_hex_pack_u8(buff, 100);
}

/******************************************************************************
* Function    : btlink_hex_pack_event_type
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_event_type(uint8_t* buff, uint16_t event_type)
{
	 return btlink_hex_pack_u16(buff, event_type, true);
}

/******************************************************************************
* Function    : btlink_hex_get_imei_digit
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
void btlink_hex_get_imei_digit(uint8_t* imei)
{
    uint8_t i;

    for (i = 0; i < BTLINK_HEX_IMEI_LEN; i++)
    {
        char buff[3] = {0};
        snprintf(buff, 2+1, "%s", (imei+(i*2)));
        g_hex_imei[i] = atoi(buff);
    }
}

/******************************************************************************
* Function    : btlink_hex_pack_imei
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_imei(uint8_t* buff)
{
    uint8_t i;

    for (i = 0; i < BTLINK_HEX_IMEI_LEN; i++) 
    {
        buff[i] = g_hex_imei[i];
    }

    return BTLINK_HEX_IMEI_LEN;
}

/******************************************************************************
* Function    : btlink_hex_pack_u16_mask_rsp
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_u16_mask_rsp(uint8_t* buff, uint16_t mask)
{
    return btlink_hex_pack_u16(buff, mask, true);
}

/******************************************************************************
* Function    : btlink_hex_pack_u32_mask_rsp
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_u32_mask_rsp(uint8_t* buff, uint32_t mask)
{
    return btlink_hex_pack_u32(buff, mask, true);
}

/******************************************************************************
* Function    : btlink_hex_pack_cell_info
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_cell_info(uint8_t* buff, btlink_cell_info_struct* cell_info)
{
	uint8_t* pbuff;
	pbuff = buff;
	
	/* MNC */
	pbuff += btlink_hex_pack_u16(pbuff, cell_info->mnc, true);
	
	/* MCC */
	pbuff += btlink_hex_pack_u16(pbuff, cell_info->mcc, true);
	
	/* LAC */
	pbuff += btlink_hex_pack_u16(pbuff, cell_info->lac, true);
	
	/* Cell Id */
	pbuff += btlink_hex_pack_u16(pbuff, cell_info->cell_id, true);
 
	/* CSQ */
	pbuff += btlink_hex_pack_u16(pbuff, cell_info->csq, true);
	
	return (buff - pbuff);
}

/******************************************************************************
* Function    : btlink_hex_pack_count_number
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_count_number(uint8_t* buff)
{
    return btlink_hex_pack_u16(buff, btlink_get_report_count(), true);
}

/******************************************************************************
* Function    : btlink_hex_pack_tail
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static uint8_t btlink_hex_pack_tail(uint8_t* buff)
{
    return btlink_hex_pack_u8(buff, '#');
}

/******************************************************************************
* Function    : btlink_assemm_hex_prt_msg_hdlr
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
void btlink_assemm_hex_prt_msg_hdlr(uint8_t* buff, uint16_t event_type, btlink_gps_info_struct *pos)
{
		uint8_t* pbuff = NULL;
	
	  pbuff = buff;
	
		BTLINK_DEBUG_TRACE(DBG_QPROT, "%d", BTLINK_DEVICE_TYPE);
		//DBG_DIRECT("Device Type:%d", BTLINK_DEVICE_TYPE);
	
		/* IMEI */
		pbuff += btlink_hex_pack_imei(pbuff);
	
		/* Device Type */
		pbuff += btlink_hex_pack_device_type(pbuff, BTLINK_DEVICE_TYPE);
	
		/* Protocol Version */
		pbuff += btlink_hex_pack_verno(pbuff);
	
		/* Event Type */
		pbuff += btlink_hex_pack_event_type(pbuff, event_type);
	
		/* Data Zone Mask */
		pbuff += btlink_hex_pack_u32_mask_rsp(pbuff, g_btlink_config.cfg_scs.data_zone_mask);
	
		/* Battery level */
		pbuff += btlink_hex_pack_battery_level(pbuff);
	
		/* GPS Number */
	  pbuff += btlink_hex_pack_number_of_point(pbuff, 1);
	
		/* GPS Information */
		pbuff += btlink_hex_pack_gps_info(pbuff, pos);
		
		/* Cell Information */
		pbuff += btlink_hex_pack_cell_info(pbuff, &g_cell_info);
		
		/*Send Time*/
		//pbuff += btlink_hex_pack_time(pbuff, ack_time);
    
    /*Count Number*/
    pbuff += btlink_hex_pack_count_number(pbuff);

    /*Tail*/
    pbuff += btlink_hex_pack_tail(pbuff); 
}
