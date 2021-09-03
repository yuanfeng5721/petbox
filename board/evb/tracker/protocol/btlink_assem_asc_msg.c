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
#include "btlink_assem_asc_msg.h"
#include "btlink_assem_asc_msg_struct.h"
#include "btlink_assem_asc_msg_enums.h"
#include "btlink_protocol_def.h"
#include "btlink_protocol_verno.h"
#include "btlink_cmd_rmmi.h"

/*****************************************************************************
* Define
*****************************************************************************/

/*****************************************************************************
* Typedef
*****************************************************************************/

/*****************************************************************************
* Extern Variable
*****************************************************************************/
extern const char *btlink_dn_frame_header_str[];

/*****************************************************************************
* Extern Function
*****************************************************************************/
extern uint16_t btlink_prot_assemble_cfg_ips_ascii(uint8_t* buff, uint16_t buf_len);
extern uint16_t btlink_prot_assemble_cfg_apn_ascii(uint8_t* buff, uint16_t buf_len);
extern uint16_t btlink_prot_assemble_cfg_scs_ascii(uint8_t* buff, uint16_t buf_len);
extern uint16_t btlink_prot_assemble_cfg_lss_ascii(uint8_t* buff, uint16_t buf_len);
	
/*****************************************************************************
 * global variable
 *****************************************************************************/
static uint32_t g_send_cnt = 0;

/*****************************************************************************
 * Local variable
 *****************************************************************************/
btlink_frame_header_index multi_alc_header_index[BTLINK_MAX_ATCMD_NUM] = 
{
	BTLINK_FH_ID_IPS, BTLINK_FH_ID_APN,
	BTLINK_FH_ID_SCS, BTLINK_FH_ID_LSS,
	BTLINK_FH_ID_NUM, BTLINK_FH_ID_NUM,
	BTLINK_FH_ID_NUM, BTLINK_FH_ID_NUM
};

uint16_t (*assemble_packet_func[BTLINK_MAX_ATCMD_NUM])(uint8_t *buff, uint16_t buf_len) = 
{
	btlink_prot_assemble_cfg_ips_ascii, btlink_prot_assemble_cfg_apn_ascii,
	btlink_prot_assemble_cfg_scs_ascii, btlink_prot_assemble_cfg_lss_ascii,
	NULL, NULL,
	NULL, NULL
};

char g_btlink_pro_verno[9] = {0};
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
* Function    : btlink_get_pro_verno
* 
* Author      : Eric.xu
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : Return device ID and protocol version number string
******************************************************************************/
char* btlink_get_pro_verno(void)
{
    return g_btlink_pro_verno;
}

/******************************************************************************
* Function    : btlink_set_pro_verno
* 
* Author      : Eric.xu
* 
* Parameters  : 
* 
* Return      : 
* 
* Description :
******************************************************************************/
void btlink_set_pro_verno(void)
{
	uint8_t buff[16];
	uint8_t len = 0;
	
	sprintf(g_btlink_pro_verno, "%02x%02x%02x", BTLINK_DEVICE_TYPE, BTLINK_PRO_MAR_VER, BTLINK_PRO_MIN_VER);
}

/******************************************************************************
* Function    : btlink_get_pro_verno
* 
* Author      : Eric.xu
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : Return device ID and protocol version number string
******************************************************************************/
char* btlink_get_imei(void)
{
    return (char *)(g_btlink_config.imei);
}

/******************************************************************************
* Function    : btlink_set_imei
* 
* Author      : Eric.xu
* 
* Parameters  : 
* 
* Return      : 
* 
* Description :
******************************************************************************/
void btlink_set_imei(void)
{
	memcpy(g_btlink_config.imei, "123456789012345", BTLINK_LEN_IMEI);
}

/******************************************************************************
* Function    : btlink_prot_print_cfg_head_ascii
* 
* Author      : Eric.xu
* 
* Parameters  : 
* 
* Return      : 
* 
* Description :
******************************************************************************/
uint16_t btlink_prot_print_cfg_head_ascii(uint8_t *buff, uint16_t buf_len, uint8_t type)
{
	uint16_t len = 0;

	len += btlink_prot_pack_string_ascii(&buff[len], buf_len-len, 
										(uint8_t *)BTLINK_ATRSP_HEADER, strlen(BTLINK_ATRSP_HEADER), false);

	len += btlink_prot_pack_string_ascii(&buff[len], buf_len-len, (uint8_t *)btlink_dn_frame_header_str[type], 
										strlen(btlink_dn_frame_header_str[type]),false);

	len += btlink_prot_pack_char_ascii(&buff[len], buf_len-len, BTLINK_CHR_ATSEP_HDR, 1);

	len += btlink_prot_pack_string_ascii(&buff[len], buf_len-len, (uint8_t*)btlink_get_pro_verno(), 
										strlen(btlink_get_pro_verno()),false);

	len += btlink_prot_pack_string_ascii(&buff[len], buf_len-len, g_btlink_config.imei, 
										strlen((char*)g_btlink_config.imei), true);

	return len;
}

/******************************************************************************
* Function    : btlink_prot_print_cfg_tail_ascii
* 
* Author      : Eric.xu
* 
* Parameters  : 
* 
* Return      : 
* 
* Description :
******************************************************************************/
uint16_t btlink_prot_print_cfg_tail_ascii(uint8_t* buff, uint16_t buf_len)
{
    uint16_t len = 0;
	
		#if 0
    uint8_t ack_time[BTLINK_FLD_LEN_SEND_TIME+1];
    quec_util_get_rtc_time(ack_time);
    len += quec_prot_pack_string_ascii(&buff[len], buf_len-len, ack_time, 
                                strlen((kal_char*)ack_time), KAL_TRUE);
		#endif
	
    len += btlink_prot_pack_char_ascii(&buff[len], buf_len-len, BTLINK_CHR_TAIL, 1);

    return len;
}

uint16_t btlink_prot_assemble_cfg_ips_ascii(uint8_t* buff, uint16_t buf_len)
{
	snprintf((char *)buff, buf_len,
		"%c%d%c%d%c%c"
		"%c%s%c%d"
		"%c%s%c%d"
		"%c%c%c%c%c%c",
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_ips.report_mode,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_ips.buffer_mode,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_ips.pri_host,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_ips.pri.port,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_ips.sec_host,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_ips.sec.port,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR);

	return strlen((char*)buff);
}

uint16_t btlink_prot_assemble_cfg_apn_ascii(uint8_t* buff, uint16_t buf_len)
{
	uint8_t len = 0;
	uint8_t idx = 0;
	
	snprintf((char *)buff, buf_len,
		"%c%d",
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_apn.apn_quantity);

	for (idx=0; idx<g_btlink_config.cfg_apn.apn_quantity; idx++)
	{
		len = strlen((char *)buff);
		snprintf((char *)&buff[len], buf_len,
			"%c%s%c%s%c%s%c%s",
			BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_apn.mcc_mnc,
			BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_apn.apn_name,
			BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_apn.apn_user_name,
			BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_apn.apn_password);
	}
	
	len = strlen((char *)buff);
	snprintf((char *)&buff[len], buf_len,
		"%c%c%c%c",
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR);
	return strlen((char*)buff);
}

uint16_t btlink_prot_assemble_cfg_scs_ascii(uint8_t* buff, uint16_t buf_len)
{
	snprintf((char *)buff, buf_len,
		"%c%s%c%08X"
		"%c%c%c%c", 
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_scs.dev_password,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_scs.data_zone_mask,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR);

	return strlen((char*)buff);
}

uint16_t btlink_prot_assemble_cfg_lss_ascii(uint8_t* buff, uint16_t buf_len)
{
	snprintf((char *)buff, buf_len,
		"%c%d%c%d"
		"%c%d%c%s"
		"%c%d%c%d"
		"%c%d%c%d"
		"%c%d%c%d"
		"%c%d"
		"%c%c%c%c"
		"%c%d%c%d"
		"%c%c%c%c",
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_lss.mode_selection,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_lss.continue_send_interval,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_lss.start_mode,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_lss.spec_time_of_day,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_lss.wakeup_interval,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_lss.psm_net_hold_time,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_lss.report_freq,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_lss.gnss_enable,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_lss.gnss_fix_delay,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_lss.agps_mode,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_lss.bat_low_percent,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_lss.sensor_enable,
		BTLINK_CHR_SEPARATOR, g_btlink_config.cfg_lss.non_move_duration,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR,
		BTLINK_CHR_SEPARATOR);

	return strlen((char*)buff);
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

    BTLINK_DEBUG_TRACE(DBG_QPROT, "btlink_util_get_report_count: %d", report_count);
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
		//uint8_t device_type = 0;
		//uint8_t mar_ver = 0;
		//uint8_t min_ver = 0;
	
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
		snprintf((char*)&buff[len], buf_len-len, "%c%d%d", BTLINK_CHR_SEPARATOR, BTLINK_SW_MAR_VER, BTLINK_SW_MIN_VER);
    len += strlen((char*)&buff[len]);
	
		/* Sub Command */
	  len += btlink_prot_pack_char_ascii(&buff[len], buf_len-len, BTLINK_CHR_SEPARATOR, 1);
	
		/* Tail(Send Time & Count Num & '#') */
		len += btlink_prot_frame_tail_ascii(&buff[len], buf_len-len);
	
}
