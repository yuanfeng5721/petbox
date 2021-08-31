/*
*******************************************************************************
*
*     Copyright (c) 2021 btlink Ltd.
*
*******************************************************************************
*  file name:           btlink_protocol_hdlr.c
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
#include "ctype.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "cmd_uart.h"
#include "btlink_trace.h"
#include "btlink_protocol_hdlr.h"
#include "btlink_protocol_cmd.h"
#include "btlink_protocol_util.h"
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

/*****************************************************************************
* Extern Function
*****************************************************************************/
/*****************************************************************************
 * global variable
 *****************************************************************************/
btlink_parsed_at_cmd_struct   parsed_at_cmd = {0};

btlink_config_struct g_btlink_config;

/*****************************************************************************
 * Local variable
 *****************************************************************************/
const char *btlink_dn_frame_header_str[] =
{
		"IPS",
		"APN",
		"SCS",
		"LSS",
		"RTH"
};

/*****************************************************************************
 * Local function
 *****************************************************************************/
/*****************************************************************************
 * FUNCTION
 *  btlink_get_frame_field
 *
 * DESCRIPTION
 *  get one file from raw donwlink frame based on the specific separator
 *
 * PARAMETERS
 *  raw_frame          [in]    raw frame to parse
 *  p_field            [out]   the field parsed
 *  field_max_length   [in]    max length of the field, exclude '\0'
 *  separator          [in]    the field separator
 *  tail               [in]    the tail character
 *
 * RETURNS
 *  bool
 *
 * GLOBALS AFFECTED
 *
 *****************************************************************************/
static bool btlink_get_frame_field(btlink_raw_dnlnk_frame_struct *raw_frame,
                                 uint8_t * p_field,
                                 uint8_t field_max_length,
                                 uint8_t separator,
                                 uint8_t tail)
{
    bool ret = false;
    uint8_t max_length;
    uint8_t i = 0;

    //Reserved argument should accept anything between the two adjacent separators
    max_length = field_max_length == 0 ? (raw_frame->length - raw_frame->position) : field_max_length;

    //|<---length--->|     length of raw_frame exclude the end null
    //+xxx,xxx,xxx,xx$\0
    //last position   ^    the last position is the end null
    while ( raw_frame->position < raw_frame->length
            && raw_frame->data[raw_frame->position] != separator
            && raw_frame->data[raw_frame->position] != tail    //end of raw frame
            && raw_frame->data[raw_frame->position] != '\0'    //end of raw frame buffer
            && i < max_length )
    {
        p_field[i] = raw_frame->data[raw_frame->position];
        raw_frame->position++;
        i++;
    }

    p_field[i] = '\0';
    if ( raw_frame->position < raw_frame->length
         && raw_frame->data[raw_frame->position] == separator )
    {
        raw_frame->position++;
        ret = true;
    }
    else if (raw_frame->data[raw_frame->position] == tail)
    {
        ret = true;
    }

    //Dbg_UartPrint("GPSTRK_P: Next pos: %d, Next char: %c\r\n",
    //raw_frame->position, raw_frame->data[raw_frame->position]);

    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  btlink_get_dnlnk_frame_arg_field
 *
 * DESCRIPTION
 *  wrap function of btlink_get_frame_field() to get one field
 *
 * PARAMETERS
 *  raw_frame [in]    raw frame to parse
 *  frame     [out]   parsed frame
 *
 * RETURNS
 *  bool
 *
 * GLOBALS AFFECTED
 *
 *****************************************************************************/
bool btlink_get_frame_arg_field(btlink_raw_dnlnk_frame_struct *raw_frame,
                              uint8_t * p_frame_arg_field,
                              uint8_t max_frame_arg_length,
                              uint8_t * p_buffer,
                              uint32_t buffer_size)
{
    bool ret = false;

    //clear temp buffer
    memset(p_buffer, 0, (buffer_size)*sizeof(uint8_t));

    //copy raw arg to temp buffer
    ret = btlink_get_frame_field(raw_frame, p_buffer, max_frame_arg_length, BTLINK_CHR_SEPARATOR, BTLINK_CHR_TAIL);

    //copy temp buffer to arg field
    if (ret && p_frame_arg_field != NULL)
    {
        strncpy((char *)p_frame_arg_field, (char *)p_buffer, max_frame_arg_length);
    }
#if 0
    if (ret)
    {
        DEBUG_TRACE(MOD_PROT, "GPSTRK_P: Get arg filed:");
        DEBUG_TRACE(MOD_PROT, "%s", p_buffer);
    }
#endif //GV500W_APP_DEBUG

    return ret;
}

bool verify_serial_number(uint8_t * sn)
{
    uint8_t i;
    bool ret = true;

    for (i = 0; i < BTLINK_LEN_SERIAL_NUM; i++)
    {
		#if 0
        if (!isxdigit(sn[i]))
		#else
    	if (('0' <= sn[i]) && (sn[i] <= '9'))
			{
			}
			else
		#endif
			{
					ret = false;
					break;
			}
    }
    return ret;
}

static bool btlink_get_dnlnk_frame_type(btlink_raw_dnlnk_frame_struct *raw_frame,
                                      btlink_parsed_dnlnk_frame_struct *frame)
{
    int i;
    bool ret = false;
    uint8_t header[BTLINK_LEN_HEADER + 1];

    // for example "AT@XXX=..."
    memset(header, 0, (BTLINK_LEN_HEADER + 1)*sizeof(uint8_t));
    strncpy((char *)header, (char *)&raw_frame->data[raw_frame->position], BTLINK_LEN_HEADER);
    raw_frame->position += (uint8_t)BTLINK_LEN_HEADER;
    //determine the frame type
    for ( i = 0; i < BTLINK_FH_ID_NUM; i++)
    {
        if ( btlink_dn_frame_header_str[i][0] != '\0'
			 #if 0
             && btlink_utils_app_strnicmp((char *)btlink_dn_frame_header_str[i], (char *)header, BTLINK_LEN_HEADER) == 0 )
			 #else
			 && (strncmp((char *)btlink_dn_frame_header_str[i], (char *)header, BTLINK_LEN_HEADER) == 0))
			 #endif
        {
            frame->type = (btlink_frame_header_index)i;
            ret = true;
            break;
        }
    }

		cmd_uart_print("frame->type:%d\r\n", frame->type);
		
    return ret;
}

/*******************************************************************************
 * Downlink Frame Parser
 ******************************************************************************/
static bool btlink_get_frame_arg_reserved(btlink_raw_dnlnk_frame_struct *raw_frame,
                                        uint8_t * p_buffer,
                                        uint8_t total)
{
    uint8_t i;
    bool ret = true;
    //uint8_t buffer[BTLINK_LEN_CONTENT+1]; //防止局部数组过大，造成堆栈溢出

    for (i = 0; i < total; i++)
    {
        ret = btlink_get_frame_arg_field(raw_frame, NULL, BTLINK_LEN_RESERVED,
                                       p_buffer, sizeof(p_buffer));
        if (ret == false) break;
    }

    return ret;

}

static bool btlink_get_dnlnk_frame_arg_ips(btlink_raw_dnlnk_frame_struct *raw_frame,
        btlink_parsed_dnlnk_frame_struct *frame)
{
    bool ret = true;
    uint8_t * p_arg_field;
    uint8_t buffer[BTLINK_LEN_CONTENT + 1]; //use the max filed length as the buffer size
		btlink_arg_ips_struct *ips = &frame->arg.ips;
		uint8_t i;    
		bool is_legal_ip;
	
    //<Report Mode>
    p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_IPS_REPORT_MODE, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
        if (buffer[0]=='\0') 
        {
            ips->report_mode = g_btlink_config.cfg_ips.report_mode;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_IPS_REPORT_MODE)) 
        {
						uint8_t report_mode = 0;
            report_mode = btlink_utils_atoi_buffer((char*)buffer);
            if (report_mode <= BTLINK_MAX_IPS_RERORT_MODE) 
            {
                ips->report_mode = report_mode;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;

		//<Buffer Mode>
    p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_IPS_BUFFER_MODE, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
        if (buffer[0]=='\0') 
        {
            ips->buffer_mode = g_btlink_config.cfg_ips.buffer_mode;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_IPS_BUFFER_MODE)) 
        {
						uint8_t buffer_mode = 0;
            buffer_mode = btlink_utils_atoi_buffer((char*)buffer);
            if (buffer_mode <= BTLINK_MAX_IPS_BUFFER_MODE) 
            {
                ips->buffer_mode = buffer_mode;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;
		
		//<Reserved>
		ret = btlink_get_frame_arg_reserved(raw_frame, buffer, 2);
		if (ret == false) goto error;
		
		//<main Service IP & DNS>
    p_arg_field = NULL; 
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_IPS_HOST, 
                                   buffer, sizeof(buffer));    
    if (ret) 
    {
        if (buffer[0] == '\0') 
        {
            ips->pri_mode = g_btlink_config.cfg_ips.pri_mode;
            if (g_btlink_config.cfg_ips.pri_mode == BTLINK_IPS_PRI_IP) 
            {
                for (i = 0; i < g_btlink_config.cfg_ips.pri.addr_len; i++) 
                {
                    ips->pri.addr[i] = g_btlink_config.cfg_ips.pri.addr[i];
                }
                ips->pri.addr_len = g_btlink_config.cfg_ips.pri.addr_len;
            }
            else 
            if (g_btlink_config.cfg_ips.pri_mode == BTLINK_IPS_PRI_DN) 
            {
                strncpy((char *)ips->pri_host, 
                        (char *)g_btlink_config.cfg_ips.pri_host, BTLINK_LEN_IPS_HOST);
            }
        }
        else 
        if (btlink_soc_ip_check((char *)buffer, (uint8_t *)(ips->pri.addr), &is_legal_ip) && is_legal_ip) 
        {
            ips->pri_mode = BTLINK_IPS_PRI_IP;
            ips->pri.addr_len = IP_ADDR_LEN;
        }
        else 
        if (btlink_util_ip_all_zero(buffer)) 
        {
            ips->pri.addr[0] = 0;
            ips->pri.addr[1] = 0;
            ips->pri.addr[2] = 0;
            ips->pri.addr[3] = 0;
            ips->pri_mode = BTLINK_IPS_PRI_IP;
            ips->pri.addr_len = IP_ADDR_LEN;
        }
        else 
        {
            char *p_dot;
            uint8_t host_len;
            
            p_dot = strchr((char *)buffer, '.');
            host_len = strlen((char *)buffer);
            if ( p_dot == NULL
                || 0 == p_dot - (char *)buffer
                || host_len == p_dot - (char *)buffer + 1) 
            {
                ret = false;
            }
            else 
            {
                ips->pri_mode = BTLINK_IPS_PRI_DN;
                strncpy((char *)ips->pri_host, (char *)buffer, host_len);
            }
        }
    }
    if (ret == false) goto error;

		//<main port>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_IPS_PORT, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
        if (buffer[0]=='\0') 
        {
            frame->arg.ips.pri.port = g_btlink_config.cfg_ips.pri.port;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_IPS_PORT)) 
        {
						uint16_t port = 0;
            port = btlink_utils_atoi_buffer((char*)buffer);
            if (port <= BTLINK_MAX_IPS_PORT) 
            {
                frame->arg.ips.pri.port = port;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;

		//<Backup Service IP & DNS>
    p_arg_field = NULL; 
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_IPS_HOST, 
                                   buffer, sizeof(buffer));    
    if (ret) 
    {
        if (buffer[0] == '\0') 
        {
            ips->sec_mode = g_btlink_config.cfg_ips.sec_mode;
            if (g_btlink_config.cfg_ips.sec_mode == BTLINK_IPS_PRI_IP) 
            {
                for (i = 0; i < g_btlink_config.cfg_ips.sec.addr_len; i++) 
                {
                    ips->sec.addr[i] = g_btlink_config.cfg_ips.sec.addr[i];
                }
                ips->sec.addr_len = g_btlink_config.cfg_ips.sec.addr_len;
            }
            else 
            if (g_btlink_config.cfg_ips.sec_mode == BTLINK_IPS_PRI_DN) 
            {
                strncpy((char *)ips->sec_host, 
                        (char *)g_btlink_config.cfg_ips.sec_host, BTLINK_LEN_IPS_HOST);
            }
        }
        else 
        if (btlink_soc_ip_check((char *)buffer, (uint8_t *)(ips->sec.addr), &is_legal_ip) && is_legal_ip) 
        {
            ips->sec_mode = BTLINK_IPS_PRI_IP;
            ips->sec.addr_len = IP_ADDR_LEN;
        }
        else 
        if (btlink_util_ip_all_zero(buffer)) 
        {
            ips->sec.addr[0] = 0;
            ips->sec.addr[1] = 0;
            ips->sec.addr[2] = 0;
            ips->sec.addr[3] = 0;
            ips->sec_mode = BTLINK_IPS_PRI_IP;
            ips->sec.addr_len = IP_ADDR_LEN;
        }
        else 
        {
            char *p_dot;
            uint8_t host_len;
            
            p_dot = strchr((char *)buffer, '.');
            host_len = strlen((char *)buffer);
            if ( p_dot == NULL
                || 0 == p_dot - (char *)buffer
                || host_len == p_dot - (char *)buffer + 1) 
            {
                ret = false;
            }
            else 
            {
                ips->sec_mode = BTLINK_IPS_PRI_DN;
                strncpy((char *)ips->sec_host, (char *)buffer, host_len);
            }
        }
    }
    if (ret == false) goto error;

		//<backup port>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_IPS_PORT, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
        if (buffer[0]=='\0') 
        {
            frame->arg.ips.sec.port = g_btlink_config.cfg_ips.sec.port;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_IPS_PORT)) 
        {
						uint16_t port = 0;
            port = btlink_utils_atoi_buffer((char*)buffer);
            if (port <= BTLINK_MAX_IPS_PORT) 
            {
                frame->arg.ips.sec.port = port;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;
		
		//<Reserved>
		ret = btlink_get_frame_arg_reserved(raw_frame, buffer, 6);
		if (ret == false) goto error;
		
error:
    return ret;
}

static bool btlink_get_dnlnk_frame_arg_apn(btlink_raw_dnlnk_frame_struct *raw_frame,
        btlink_parsed_dnlnk_frame_struct *frame)
{
    bool ret = true;
    uint8_t * p_arg_field;
    uint8_t buffer[BTLINK_LEN_CONTENT + 1]; //use the max filed length as the buffer size

    //<APN Quantity>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_APN_NUMS, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
			  if (buffer[0]=='\0') 
        {
            frame->arg.apn.apn_quantity = g_btlink_config.cfg_apn.apn_quantity;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_APN_NUMS)) 
        {
						uint16_t apn_nums = 0;
            apn_nums = btlink_utils_atoi_buffer((char*)buffer);
            if (apn_nums <= BTLINK_MAX_APN_NUMS) 
            {
                frame->arg.apn.apn_quantity = apn_nums;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;

		/* MCC&MNC */
    p_arg_field = frame->arg.apn.mcc_mnc;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_APN_MCC_MNC,
                                   buffer, sizeof(buffer));
    if (ret == false) goto error;

		/* APN NAME */
    p_arg_field = frame->arg.apn.apn_name;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_APN_NAME,
                                   buffer, sizeof(buffer));
    if (ret == false) goto error;

		/* APN USER NAME */
    p_arg_field = frame->arg.apn.apn_user_name;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_APN_USER_NAME,
                                   buffer, sizeof(buffer));
    if (ret == false) goto error;

		/* APN Password */
    p_arg_field = frame->arg.apn.apn_password;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_APN_PASSWORD,
                                   buffer, sizeof(buffer));
    if (ret == false) goto error;

		//<Reserved>
		ret = btlink_get_frame_arg_reserved(raw_frame, buffer, 4);
		if (ret == false) goto error;
		
error:
    return ret;
}

static bool btlink_get_dnlnk_frame_arg_scs(btlink_raw_dnlnk_frame_struct *raw_frame,
        btlink_parsed_dnlnk_frame_struct *frame)
{
    bool ret = true;
    uint8_t * p_arg_field;
    uint8_t buffer[BTLINK_LEN_CONTENT + 1]; //use the max filed length as the buffer size

		/*<password>*/
    p_arg_field = frame->password;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_SCS_PASSWORD, 
                                   buffer, sizeof(buffer));
    if(ret)
    {
        ret = btlink_util_password_match(p_arg_field);
    }
    if(ret == false) 
    {
				frame->step = BTLINK_PARSE_STEP_PWD;
				goto error;
    }
	
		// <New Password>
    p_arg_field = frame->arg.scs.dev_password;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_SCS_PASSWORD, 
                                   buffer, sizeof(buffer));
    if (ret)
    {
        uint8_t len = strlen((char *)p_arg_field);
        if (BTLINK_MIN_PASSWORD_LEN <=len && len <= BTLINK_MAX_PASSWORD_LEN) 
        {
            ret = btlink_util_isalnum_buffer(p_arg_field, BTLINK_LEN_SCS_PASSWORD);
        }
        else if (len !=0 ) 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;
		
    //<data zone Mask>
    p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_SCS_DATA_ZONE_MASK,
                                   buffer, sizeof(buffer));
    if (ret)
    {
				if (buffer[0]=='\0') 
        {
            frame->arg.scs.data_zone_mask = g_btlink_config.cfg_scs.data_zone_mask;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_SCS_DATA_ZONE_MASK))
        {
            frame->arg.scs.data_zone_mask = btlink_hex_string_2_int((char *)buffer);
        }
        else
        {
            ret = false;
        }
    }
    if (ret == false) goto error;

		//<Reserved>
		ret = btlink_get_frame_arg_reserved(raw_frame, buffer, 4);
		if (ret == false) goto error;
		
error:
    return ret;
}

static bool btlink_get_dnlnk_frame_arg_lss(btlink_raw_dnlnk_frame_struct *raw_frame,
        btlink_parsed_dnlnk_frame_struct *frame)
{
    bool ret = true;
    uint8_t * p_arg_field;
    uint8_t buffer[BTLINK_LEN_CONTENT + 1]; //use the max filed length as the buffer size

    //<Mode Selection>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_LSS_MODE_SELECT, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
				if (buffer[0]=='\0') 
        {
            frame->arg.lss.mode_selection = g_btlink_config.cfg_lss.mode_selection;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_LSS_MODE_SELECT)) 
        {
						uint8_t mode_selection = 0;
            mode_selection = btlink_utils_atoi_buffer((char*)buffer);
            if (mode_selection <= BTLINK_MAX_LSS_MODE_SELECT) 
            {
                frame->arg.lss.mode_selection = mode_selection;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;

		//<Continues Send Interval>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_LSS_CONT_SEND_INTERVAL, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
				if (buffer[0]=='\0') 
        {
            frame->arg.lss.continue_send_interval = g_btlink_config.cfg_lss.continue_send_interval;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_LSS_CONT_SEND_INTERVAL)) 
        {
						uint16_t continus_send_interval = 0;
            continus_send_interval = btlink_utils_atoi_buffer((char*)buffer);
            if (continus_send_interval <= BTLINK_MAX_LSS_CONT_SEND_INTERNVAL) 
            {
                frame->arg.lss.continue_send_interval = continus_send_interval;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;
		
    //<Start Mode>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_LSS_START_MODE, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
				if (buffer[0]=='\0') 
        {
            frame->arg.lss.start_mode = g_btlink_config.cfg_lss.start_mode;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_LSS_START_MODE)) 
        {
						uint8_t start_mode = 0;
            start_mode = btlink_utils_atoi_buffer((char*)buffer);
            if (start_mode <= BTLINK_MAX_LSS_START_MODE) 
            {
                frame->arg.lss.start_mode = start_mode;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;
		
		// <Specified Time of Day>
    p_arg_field = frame->arg.lss.spec_time_of_day;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_LSS_SPEC_TIME_DAY,
                                   buffer, sizeof(buffer));
    if (ret == false) goto error;

		//<Wakeup Interval>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_LSS_WAKEUP_INTERVAL, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
				if (buffer[0]=='\0') 
        {
            frame->arg.lss.wakeup_interval = g_btlink_config.cfg_lss.wakeup_interval;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_LSS_WAKEUP_INTERVAL)) 
        {
						uint8_t wakeup_interval = 0;
            wakeup_interval = btlink_utils_atoi_buffer((char*)buffer);
            if ((BTLINK_LSS_WAKEUP_INTERVAL_1 <= wakeup_interval 
							  && wakeup_interval <= BTLINK_LSS_WAKEUP_INTERVAL_4)
								|| wakeup_interval == BTLINK_LSS_WAKEUP_INTERVAL_6
								|| wakeup_interval == BTLINK_LSS_WAKEUP_INTERVAL_8
								|| wakeup_interval == BTLINK_LSS_WAKEUP_INTERVAL_12
								|| wakeup_interval == BTLINK_LSS_WAKEUP_INTERVAL_24) 
            {
                frame->arg.lss.wakeup_interval = wakeup_interval;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;

		//<PSM Network Hold Time>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_LSS_PSM_NET_HOLD_TIME, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
				if (buffer[0]=='\0') 
        {
            frame->arg.lss.psm_net_hold_time = g_btlink_config.cfg_lss.psm_net_hold_time;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_LSS_PSM_NET_HOLD_TIME)) 
        {
						uint32_t psm_net_hold_time = 0;
            psm_net_hold_time = btlink_utils_atoi_buffer((char*)buffer);
            if (psm_net_hold_time <= BTLINK_MAX_LSS_NET_HOLD_TIME) 
            {
                frame->arg.lss.psm_net_hold_time = psm_net_hold_time;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;

		//<Report Frequency>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_LSS_REPORT_FREG, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
				if (buffer[0]=='\0') 
        {
            frame->arg.lss.report_freq = g_btlink_config.cfg_lss.report_freq;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_LSS_REPORT_FREG)) 
        {
						uint8_t report_freq = 0;
            report_freq = btlink_utils_atoi_buffer((char*)buffer);
            if (BTLINK_MIN_LSS_REPORT_FREQ <= report_freq 
							  && report_freq <= BTLINK_MAX_LSS_REPORT_FREQ) 
            {
                frame->arg.lss.report_freq = report_freq;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;
		
		//<GNSS Enable>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_LSS_GNSS_ENABLE, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
				if (buffer[0]=='\0') 
        {
            frame->arg.lss.gnss_enable = g_btlink_config.cfg_lss.gnss_enable;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_LSS_GNSS_ENABLE)) 
        {
						uint8_t gnss_enable = 0;
            gnss_enable = btlink_utils_atoi_buffer((char*)buffer);
            if (gnss_enable <= BTLINK_MAX_LSS_GNSS_ENABLE) 
            {
                frame->arg.lss.gnss_enable = gnss_enable;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;

		//<GNSS Fix Delay>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_LSS_GNSS_FIX_DELAY, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
				if (buffer[0]=='\0') 
        {
            frame->arg.lss.gnss_fix_delay = g_btlink_config.cfg_lss.gnss_fix_delay;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_LSS_GNSS_FIX_DELAY)) 
        {
						uint8_t gnss_fix_delay = 0;
            gnss_fix_delay = btlink_utils_atoi_buffer((char*)buffer);
            if (BTLINK_MIN_LSS_GNSS_FIX_DELAY <= gnss_fix_delay 
							  && gnss_fix_delay <= BTLINK_MAX_LSS_GNSS_FIX_DELAY) 
            {
                frame->arg.lss.gnss_fix_delay = gnss_fix_delay;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;

		//<AGPS Mode>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_LSS_AGPS_MODE, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
				if (buffer[0]=='\0') 
        {
            frame->arg.lss.agps_mode = g_btlink_config.cfg_lss.agps_mode;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_LSS_AGPS_MODE)) 
        {
						uint8_t agps_mode = 0;
            agps_mode = btlink_utils_atoi_buffer((char*)buffer);
            if (agps_mode <= BTLINK_MAX_LSS_AGPS_MODE) 
            {
                frame->arg.lss.agps_mode = agps_mode;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;

		//<Battery Low Percent>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_LSS_BAT_LOW_PERCENT, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
				if (buffer[0]=='\0') 
        {
            frame->arg.lss.bat_low_percent = g_btlink_config.cfg_lss.bat_low_percent;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_LSS_BAT_LOW_PERCENT)) 
        {
						uint8_t bat_low_percent = 0;
            bat_low_percent = btlink_utils_atoi_buffer((char*)buffer);
            if (bat_low_percent <= BTLINK_MAX_LSS_BAT_LOW_PERCENT) 
            {
                frame->arg.lss.bat_low_percent = bat_low_percent;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;
		
		//<Reserved>
		ret = btlink_get_frame_arg_reserved(raw_frame, buffer, 4);
		if (ret == false) goto error;

		//<Sensor Enable>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_LSS_SENSOR_ENABLE, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
				if (buffer[0]=='\0') 
        {
            frame->arg.lss.sensor_enable = g_btlink_config.cfg_lss.sensor_enable;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_LSS_SENSOR_ENABLE)) 
        {
						uint8_t sensor_enable = 0;
            sensor_enable = btlink_utils_atoi_buffer((char*)buffer);
            if (sensor_enable <= BTLINK_MAX_LSS_SENSOR_ENABLE) 
            {
                frame->arg.lss.sensor_enable = sensor_enable;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;
		
		//<Non Movement Duration>
		p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_LSS_NON_MOVE_DUR, 
                                   buffer, sizeof(buffer));
    if (ret) 
    {
				if (buffer[0]=='\0') 
        {
            frame->arg.lss.non_move_duration = g_btlink_config.cfg_lss.non_move_duration;
        }
        else 
        if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_LSS_NON_MOVE_DUR)) 
        {
						uint8_t non_move_dur = 0;
            non_move_dur = btlink_utils_atoi_buffer((char*)buffer);
            if (BTLINK_MIN_LSS_NON_MOVE_DUR <= non_move_dur 
							  && non_move_dur <= BTLINK_MAX_LSS_NON_MOVE_DUR) 
            {
                frame->arg.lss.non_move_duration = non_move_dur;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if(ret == false) goto error;
		
		//<Reserved>
		ret = btlink_get_frame_arg_reserved(raw_frame, buffer, 4);
		if (ret == false) goto error;
		
error:
    return ret;
}

static bool btlink_get_dnlnk_frame_arg_rth(btlink_raw_dnlnk_frame_struct *raw_frame,
        btlink_parsed_dnlnk_frame_struct *frame)
{
    bool ret = true;
    uint8_t * p_arg_field;
    uint8_t buffer[BTLINK_LEN_CONTENT + 1]; //use the max filed length as the buffer size

    //<Sub Command>
    p_arg_field = NULL;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_RTH_SUB_COMMAND,
                                   buffer, sizeof(buffer));
    if (ret)
    {
			  if (btlink_utils_isdigit_buffer(buffer, BTLINK_LEN_RTH_SUB_COMMAND)) 
        {
						uint8_t sub_command = 0;
            sub_command = btlink_utils_atoi_buffer((char*)buffer);
            if (sub_command <= BTLINK_MAX_RTH_SUB_COMMAND) 
            {
                frame->arg.rth.sub_command = sub_command;
            }
            else 
            {
                ret = false;
            }
        }
        else 
        {
            ret = false;
        }
    }
    if (ret == false) goto error;

		//<Reserved>
		ret = btlink_get_frame_arg_reserved(raw_frame, buffer, 4);
		if (ret == false) goto error;
		
error:
    return ret;
}

static bool btlink_get_dnlnk_frame_arg (btlink_raw_dnlnk_frame_struct *raw_frame,
                                      btlink_parsed_dnlnk_frame_struct *frame)
{
    bool ret = false;

    switch (frame->type)
    {
				case BTLINK_FH_ID_IPS:
						ret = btlink_get_dnlnk_frame_arg_ips(raw_frame, frame);
						break;
				
				case BTLINK_FH_ID_APN:
						ret = btlink_get_dnlnk_frame_arg_apn(raw_frame, frame);
						break;
    
				case BTLINK_FH_ID_SCS:
						ret = btlink_get_dnlnk_frame_arg_scs(raw_frame, frame);
						break;
				
				case BTLINK_FH_ID_LSS:
						ret = btlink_get_dnlnk_frame_arg_lss(raw_frame, frame);
						break;
				
				case BTLINK_FH_ID_RTH:
						ret = btlink_get_dnlnk_frame_arg_rth(raw_frame, frame);
						break;

        default:
            break;
    }

    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  btlink_get_dnlnk_frame_sn
 *
 * DESCRIPTION
 *  get serial number of the downlink frame
 *
 * PARAMETERS
 *  raw_frame [in]      raw frame to parse
 *  frame       [out]   parsed frame
 *
 * RETURNS
 *  bool
 *
 * GLOBALS AFFECTED
 *
 *****************************************************************************/
static bool btlink_get_dnlnk_frame_sn(btlink_raw_dnlnk_frame_struct *raw_frame,
                                    btlink_parsed_dnlnk_frame_struct *frame)
{
    bool ret = true;
    uint8_t * p_arg_field;
    uint8_t buffer[BTLINK_LEN_SERIAL_NUM + 1];

    //<Serial number>
    p_arg_field = frame->serial_number;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_SERIAL_NUM, buffer, sizeof(buffer));
    return (ret && verify_serial_number(p_arg_field));
}

/*****************************************************************************
 * FUNCTION
 *  btlink_get_dnlnk_frame_ack
 *
 * DESCRIPTION
 *  get detailed downlink frame ack flag
 *
 * PARAMETERS
 *  raw_frame [in]      raw frame to parse
 *  frame       [out]   parsed frame
 *
 * RETURNS
 *  bool
 *
 * GLOBALS AFFECTED
 *
 *****************************************************************************/
static bool btlink_get_dnlnk_frame_ack (btlink_raw_dnlnk_frame_struct *raw_frame,
                                      btlink_parsed_dnlnk_frame_struct *frame)
{
    bool ret = false;
    frame->ack = true;
    ret = true;
    return ret;
}

/******************************************************************************
* Function    : btlink_get_dnlnk_frame_password
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
bool btlink_get_dnlnk_frame_password(btlink_raw_dnlnk_frame_struct *raw_frame, 
                                       btlink_parsed_dnlnk_frame_struct *frame)
{
    bool ret = true;
    uint8_t* p_arg_field = NULL;
    uint8_t buffer[BTLINK_LEN_SCS_PASSWORD+1] = {0};

    p_arg_field = frame->password;
    ret = btlink_get_frame_arg_field(raw_frame, p_arg_field, BTLINK_LEN_SCS_PASSWORD, 
                                   buffer, sizeof(buffer));
		
    return (bool)(ret && btlink_util_password_match(p_arg_field));

}

/*****************************************************************************
 * FUNCTION
 *  btlink_verify_dnlnk_frame_tail
 *
 * DESCRIPTION
 *  verify if the downlink frame tail character valid
 *
 * PARAMETERS
 *  raw_frame [in]      raw frame to parse
 *
 * RETURNS
 *  bool
 *
 * GLOBALS AFFECTED
 *
 *****************************************************************************/
static bool btlink_verify_dnlnk_frame_tail(btlink_raw_dnlnk_frame_struct *raw_frame)
{
    return (raw_frame->data[raw_frame->position] == BTLINK_CHR_TAIL);
}

/******************************************************************************
* Function    : btlink_parse_dnlnk_frame
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
bool btlink_parse_dnlnk_frame(btlink_raw_dnlnk_frame_struct *raw_frame,
                            uint8_t * number,
                            btlink_parsed_dnlnk_frame_struct *frame)
{
    bool ret = false;
    uint8_t para_pos = 0;

		cmd_uart_print("raw_frame->data:%s\r\n", raw_frame->data);
	
    if ( raw_frame->length > 0 &&
         ( strncmp((const char *)raw_frame->data, BTLINK_DNLNK_HEADER,  BTLINK_HEADER_CHKLEN) == 0 ||
           strncmp((const char *)raw_frame->data, BTLINK_DNLNK_HEADER_LC,  BTLINK_HEADER_CHKLEN) == 0)
       )
    {
        //skip the header prefix char
        raw_frame->position = BTLINK_HEADER_CHKLEN;

        //get the frame type
        frame->step = BTLINK_PARSE_STEP_TYPE;
        ret = btlink_get_dnlnk_frame_type(raw_frame, frame);
        if (ret == false) goto error;
        raw_frame->position++; //skip '='
        para_pos = raw_frame->position;

				//get password
        if ( frame->type != BTLINK_FH_ID_SCS) 
        {
            frame->step = BTLINK_PARSE_STEP_PWD;
            ret = btlink_get_dnlnk_frame_password(raw_frame, frame);
        }
        if (ret == false) goto error;
				
        //get the detailed args
        frame->step = BTLINK_PARSE_STEP_ARG;
        ret = btlink_get_dnlnk_frame_arg(raw_frame, frame);
        if (ret == false) goto error;

        //get serial number
        frame->step = BTLINK_PARSE_STEP_SN;
        ret = btlink_get_dnlnk_frame_sn(raw_frame, frame);
        if (ret == false) goto error;

        //get ACK
        frame->step = BTLINK_PARSE_STEP_ACK;
        ret = btlink_get_dnlnk_frame_ack(raw_frame, frame);
        if (ret == false) goto error;

        //verify tail character
        frame->step = BTLINK_PARSE_STEP_TAIL;
        ret = btlink_verify_dnlnk_frame_tail(raw_frame);
        if (ret == false) goto error;

        //Set the oa number
        if (number != NULL)
        {
            memset((char *)frame->oa_number, 0, (MAX_CC_ADDR_LEN + 1));
            strncpy((char *)frame->oa_number, (char *)number, MAX_CC_ADDR_LEN);
        }

        //get parameter string
        memset((char *)frame->para_string, 0, (BTLINK_LEN_PARAMETER + 1));
        strncpy((char *)frame->para_string, (char *)(&raw_frame->data[para_pos]), (raw_frame->position - para_pos));
    }

error:
    frame->valid = ret;

    return  ret;
}

/******************************************************************************
* Function    : btlink_assemble_ack_frame
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
bool btlink_assemble_ack_frame(btlink_parsed_dnlnk_frame_struct *dn_frame)
{
    bool ret = true;

    #if 0
    assert_param(dn_frame != NULL);

    switch (dn_frame->type)
    {
        case BTLINK_FH_ID_BSI:     //Bearer Setting Information
            //btlink_assemble_general_ack_frame(dn_frame, ack_frame, NULL);
            btlink_assemble_general_ack_frame(dn_frame, 0xff);
            break;

        default:
            ret  = false;
            break;
    }
    #endif	
    return ret;
}

/******************************************************************************
* Function    : btlink_check_and_exec_protocol
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
bool btlink_check_and_exec_protocol(uint8_t *msg_content, uint16_t msg_length, uint8_t *number)
{
    bool valid = false;
    btlink_raw_dnlnk_frame_struct raw_frame;
    btlink_parsed_dnlnk_frame_struct dn_frame;

    memset(&raw_frame, 0, sizeof(btlink_raw_dnlnk_frame_struct));
    raw_frame.position = 0;
    raw_frame.length = msg_length;
    strncpy((char *)raw_frame.data, (char *)msg_content, msg_length);

    memset(&dn_frame, 0, sizeof(btlink_parsed_dnlnk_frame_struct));
    dn_frame.type = BTLINK_FH_ID_UNKNOWN;
    if (btlink_parse_dnlnk_frame(&raw_frame, number, &dn_frame))
    {
        valid = true;
        btlink_cmd_process(&dn_frame);
    }
    else
    {
        memset(&dn_frame, 0, sizeof(btlink_parsed_dnlnk_frame_struct));
    }
    return valid;
}

/******************************************************************************
* Function    : btlink_cmd_parse
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
int btlink_cmd_parse(char *cmd, int16_t cmd_len)
{
	int retcode = -1;
	CommandLine_t command_line = {0};
	
	//cmd_uart_print("cmd:%s, cmd_len:%d\r\n", cmd, cmd_len);
	//BTLINK_DEBUG_TRACE(DBG_QPROT, "%s", cmd);

	if (cmd_len >= COMMAND_LINE_SIZE)
	{
		cmd_len = COMMAND_LINE_SIZE;
	}
	
	if (true == rmmi_is_btlink_cmd((uint8_t *)cmd))
	{
			strncpy(command_line.character, cmd, cmd_len);
			command_line.character[COMMAND_LINE_SIZE] = '\0';
			command_line.length = strlen(command_line.character);
			command_line.position = 0;
			
			if (true == rmmi_btlink_cmd_hdlr(&command_line))
			{
					retcode = 0;
			}
	}
	
	return retcode;
}

/******************************************************************************
* Function    : btlink_is_protocol_format
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
bool btlink_is_protocol_format(uint8_t * msg_content, uint16_t msg_length)
{
    bool ret = false;	

    if (msg_length >= BTLINK_HEADER_CHKLEN
         && (strncmp((const char *)msg_content, BTLINK_DNLNK_HEADER,  BTLINK_HEADER_CHKLEN) == 0 ||
             strncmp((const char *)msg_content, BTLINK_DNLNK_HEADER_LC,  BTLINK_HEADER_CHKLEN) == 0))
    {
        ret = true;
    }

    return ret;
}
