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

#include "btlink_assem_asc_msg_struct.h"
#include "btlink_assem_asc_msg_enums.h"
#include "btlink_protocol_def.h"
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
extern const char *btlink_dn_frame_header_str;

/*****************************************************************************
* Extern Function
*****************************************************************************/

/*****************************************************************************
 * global variable
 *****************************************************************************/
static uint32_t g_send_cnt = 0;

/*****************************************************************************
 * Local variable
 *****************************************************************************/

/*****************************************************************************
 * Local function
 *****************************************************************************/
/******************************************************************************
* Function    : btlink_prot_pack_char_ascii
* 
* Author      : Eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : Pack 'num' same characters into to buffer
******************************************************************************/ 
uint16_t btlink_prot_pack_char_ascii(uint8_t* buff, uint16_t buf_len, char ch, uint8_t num) 
{ 
	uint8_t i = 0;
	uint16_t len = (num < buf_len) ? num : buf_len;
	for (i=0; i<len; i++)
	{ 
		buff[i] = ch; 
	}
	return len; 
} 

/******************************************************************************
* Function    : btlink_prot_pack_string_ascii
* 
* Author      : Eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : Pack a string into buffer
******************************************************************************/ 
uint16_t btlink_prot_pack_string_ascii(uint8_t* buff, uint16_t buf_len, uint8_t* str, uint16_t str_len, bool with_sep) 
{ 
	uint16_t len = with_sep ? ((str_len+1 < buf_len) ? str_len+1 : buf_len) : ((str_len < buf_len) ? str_len : buf_len);
	if (with_sep)
	{ 
		buff[0] = ',';
		strncpy((char*)&buff[1], (char*)str, len-1); 
	}
	else
	{ 
		strncpy((char*)buff, (char*)str, len); 
	}
	return len; 
}

/******************************************************************************
* Function    : btlink_util_get_rtc_time
* 
* Author      : Eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
void btlink_util_get_rtc_time(uint8_t *ack_time)
{
    btlink_time_struct  rtc_time;

    memset(ack_time, 0, sizeof((char *)ack_time));
    //applib_dt_get_rtc_time(&rtc_time);

    snprintf((char *)ack_time, BTLINK_FLD_LEN_SEND_TIME+1, "%04d%02d%02d%02d%02d%02d", 
            rtc_time.nYear, 
            rtc_time.nMonth,
            rtc_time.nDay,
            rtc_time.nHour,
            rtc_time.nMin,
            rtc_time.nSec);
}

/******************************************************************************
* Function    : btlink_util_get_report_count
* 
* Author      : Eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
uint32_t btlink_util_get_report_count(void)
{
    uint32_t report_count = g_send_cnt;

    //QUEC_DEBUG_TRACE(DBG_QPROT, "btlink_util_get_report_count: %d", report_count);
    g_send_cnt++;
    if (g_send_cnt > 0xffff) 
    {
        g_send_cnt = 0;
    }

    return report_count;
}

/******************************************************************************
* Function    : btlink_util_convert_hex_to_ascii
* 
* Author      : Eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : convert, for example 0x3D4F to "3D4F"
******************************************************************************/
void btlink_util_convert_hex_to_ascii(uint8_t* data)
{
    int32_t i, length = 0;
    uint8_t tmp_data[128];
    uint8_t tmp_low = 0;
    uint8_t tmp_high = 0;

    memset(tmp_data, 0, sizeof(tmp_data));
    strcpy((char *)tmp_data, (char *)data);

    length = strlen((char *)tmp_data);

    for (i=0; i<length; i++)
    {
        tmp_high = ((tmp_data[i] & 0xF0) >> 4);
        tmp_low = tmp_data[i] & 0x0F;

        if (tmp_high <= 9)
        {   
            data[i*2] = tmp_high + '0';
        }
        else if (tmp_high >= 0x0A && tmp_high<= 0x0F)
        {
            data[i*2] = (tmp_high - 10) + 'A';
        }
         if (tmp_low <= 9)
        {   
            data[i*2+1] = tmp_low + '0';
        }
        else if (tmp_low >= 0x0A && tmp_low <= 0x0F)
        {
            data[i*2+1] = (tmp_low - 10) + 'A';
        }
    }
}

/******************************************************************************
* Function    : btlink_prot_frame_tail_ascii
* 
* Author      : Eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
uint16_t btlink_prot_frame_tail_ascii(uint8_t* buff, uint16_t buf_len) 
{ 
	uint16_t len = 0;
	uint8_t ack_time[BTLINK_FLD_LEN_SEND_TIME+1] = "";
	
	btlink_util_get_rtc_time(ack_time); 
	len += btlink_prot_pack_string_ascii(&buff[len], buf_len-len, ack_time, strlen((char*)ack_time), true);
	
	snprintf((char *)&buff[len], buf_len-len, "%c%04X", BTLINK_CHR_SEPARATOR, btlink_util_get_report_count()); 
	len += strlen((char*)&buff[len]);
	
	len += btlink_prot_pack_char_ascii(&buff[len], buf_len-len, BTLINK_CHR_TAIL, 1);
	return len; 
} 

/******************************************************************************
* Function    : btlink_pack_asc_ack_msg_hdlr
* 
* Author      : Eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : pack +ACK ASCII message.
******************************************************************************/ 
void btlink_pack_asc_ack_msg_hdlr(uint8_t* buff, uint16_t buf_len, uint8_t* cmd_str)
{
		uint8_t imei[1+BTLINK_LEN_IMEI] = "123456789012345";
		uint16_t len = 0;
		uint8_t device_type = 0;
		uint8_t mar_ver = 0;
		uint8_t min_ver = 0;
	
		/* Head */
		len += btlink_prot_pack_string_ascii(&buff[len], buf_len-len, (uint8_t *)ASC_ACK_HEAD, strlen(ASC_ACK_HEAD), false);

		/* Command id */
		//btlink_dn_frame_header_str[BTLINK_FH_ID_DBG]
		len += btlink_prot_pack_string_ascii(&buff[len], buf_len-len, cmd_str, strlen((char *)cmd_str), false);
	
		/* IMEI */
		len += btlink_prot_pack_string_ascii(&buff[len], buf_len-len, imei, strlen((char *)imei), true);
	
		/* Device Type */
		snprintf((char*)&buff[len], buf_len-len, "%c%02X", BTLINK_CHR_SEPARATOR, BTLINK_DEVICE_TYPE);
    len += strlen((char*)&buff[len]);
	
		/* Protocol Version */
		snprintf((char*)&buff[len], buf_len-len, "%c%d%d", BTLINK_CHR_SEPARATOR, BTLINK_MAR_VER, BTLINK_MIN_VER);
    len += strlen((char*)&buff[len]);
	
		/* Sub Command */
	  len += btlink_prot_pack_char_ascii(&buff[len], buf_len-len, BTLINK_CHR_SEPARATOR, 1);
	
		/* Tail(Send Time & Count Num & '#') */
		len += btlink_prot_frame_tail_ascii(&buff[len], buf_len-len);
	
}