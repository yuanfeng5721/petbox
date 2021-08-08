/*
*******************************************************************************
*
*     Copyright (c) 2021 btlink Ltd.
*
*******************************************************************************
*  file name:           btlink_protocol_cmd.c
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
#include "string.h"
#include "stdio.h"

#include "btlink_protocol_cmd.h"
#include "btlink_protocol_hdlr.h"
#include "btlink_protocol_util.h"

/*************************************************************************
* Extern Variable and Function
*************************************************************************/
extern const char *btlink_dn_frame_header_str[];

/*************************************************************************
* Global Variable
*************************************************************************/
char btlink_canack_sn[ 1 + BTLINK_FLD_LEN_SERIAL_NUM] = "";
/*******************************************************/
//Temporary  variables definition
/*******************************************************/
static void btlink_cmd_exec_dbg(btlink_parsed_dnlnk_frame_struct *dn_frame)
{
		#if 0 //eric
    btlink_arg_dbg_struct *arg_dbg;

    arg_dbg = &(dn_frame->arg.dbg);
		#endif
}

static void btlink_cmd_exec_ips(btlink_parsed_dnlnk_frame_struct *dn_frame)
{
    btlink_arg_ips_struct *arg_ips = &(dn_frame->arg.ips);
    bool need_save = false;
	
		// <Report Mode>
		if (g_btlink_config.cfg_ips.report_mode != arg_ips->report_mode)
		{
			g_btlink_config.cfg_ips.report_mode = arg_ips->report_mode;
			need_save = true;
		}

    //<Buffer Mode>
		if (g_btlink_config.cfg_ips.buffer_mode != arg_ips->buffer_mode)
		{
			g_btlink_config.cfg_ips.buffer_mode = arg_ips->buffer_mode;
			need_save = true;
		}
		
		//<Main Server IP & DNS>
    if (arg_ips->pri_mode != g_btlink_config.cfg_ips.pri_mode)
    {
        g_btlink_config.cfg_ips.pri_mode = arg_ips->pri_mode;
        if (arg_ips->pri_mode == BTLINK_IPS_PRI_IP)
        {
            memcpy((char *)(g_btlink_config.cfg_ips.pri.addr), (char *)(arg_ips->pri.addr), MAX_SOCK_ADDR_LEN);
            g_btlink_config.cfg_ips.pri.addr_len = arg_ips->pri.addr_len;

            /*Clear domain name*/
            memset(g_btlink_config.cfg_ips.pri_host, 0, sizeof(g_btlink_config.cfg_ips.pri_host));
        }
        else if (arg_ips->pri_mode == BTLINK_IPS_PRI_DN)
        {
            /*TODO: DNS*/
            memcpy((char *)g_btlink_config.cfg_ips.pri_host, (char *)arg_ips->pri_host, BTLINK_FLD_LEN_HOST);
        }
        need_save = true;
    }
    else
    {
        if (arg_ips->pri_mode == BTLINK_IPS_PRI_IP)
        {
            if (!IP_ALL_MATCH(arg_ips->pri.addr, g_btlink_config.cfg_ips.pri.addr))
            {
                memcpy((char *)(g_btlink_config.cfg_ips.pri.addr), (char *)(arg_ips->pri.addr), MAX_SOCK_ADDR_LEN);
                g_btlink_config.cfg_ips.pri.addr_len = arg_ips->pri.addr_len;

                need_save = true;
            }
        }
        else if (arg_ips->pri_mode == BTLINK_IPS_PRI_DN)
        {
            if (strncmp((char *)g_btlink_config.cfg_ips.pri_host, (char *)arg_ips->pri_host, BTLINK_FLD_LEN_HOST)!=0)
            {
                /*TODO: DNS*/
                memcpy((char *)g_btlink_config.cfg_ips.pri_host, (char *)arg_ips->pri_host, BTLINK_FLD_LEN_HOST);
                need_save = true;
            }
        }
    }

		//<Main Port>
		if (g_btlink_config.cfg_ips.pri.port != arg_ips->pri.port)
		{
			g_btlink_config.cfg_ips.pri.port = arg_ips->pri.port;
			need_save = true;
		}

		//<Backup Server IP & DNS>
    if (arg_ips->sec_mode != g_btlink_config.cfg_ips.sec_mode)
    {
        g_btlink_config.cfg_ips.sec_mode = arg_ips->sec_mode;
        if (arg_ips->sec_mode == BTLINK_IPS_PRI_IP)
        {
            memcpy((char *)(g_btlink_config.cfg_ips.sec.addr), (char *)(arg_ips->sec.addr), MAX_SOCK_ADDR_LEN);
            g_btlink_config.cfg_ips.sec.addr_len = arg_ips->sec.addr_len;

            /*Clear domain name*/
            memset(g_btlink_config.cfg_ips.sec_host, 0, sizeof(g_btlink_config.cfg_ips.sec_host));
        }
        else if (arg_ips->sec_mode == BTLINK_IPS_PRI_DN)
        {
            /*TODO: DNS*/
            memcpy((char *)g_btlink_config.cfg_ips.sec_host, (char *)arg_ips->sec_host, BTLINK_FLD_LEN_HOST);
        }
        need_save = true;
    }
    else
    {
        if (arg_ips->sec_mode == BTLINK_IPS_PRI_IP)
        {
            if (!IP_ALL_MATCH(arg_ips->sec.addr, g_btlink_config.cfg_ips.sec.addr))
            {
                memcpy((char *)(g_btlink_config.cfg_ips.sec.addr), (char *)(arg_ips->sec.addr), MAX_SOCK_ADDR_LEN);
                g_btlink_config.cfg_ips.sec.addr_len = arg_ips->sec.addr_len;

                need_save = true;
            }
        }
        else if (arg_ips->sec_mode == BTLINK_IPS_PRI_DN)
        {
            if (strncmp((char *)g_btlink_config.cfg_ips.sec_host, (char *)arg_ips->sec_host, BTLINK_FLD_LEN_HOST)!=0)
            {
                /*TODO: DNS*/
                memcpy((char *)g_btlink_config.cfg_ips.sec_host, (char *)arg_ips->sec_host, BTLINK_FLD_LEN_HOST);
                need_save = true;
            }
        }
    }
		
		//<Backup Port>
		if (g_btlink_config.cfg_ips.sec.port != arg_ips->sec.port)
		{
			g_btlink_config.cfg_ips.sec.port = arg_ips->sec.port;
			need_save = true;
		}
		
		if (need_save == true)
		{
			//FlashCfgCmdParaWrite(BTLINK_FLASH_CFG_STRUCT_ADDR, (uint32 *)(&g_btlink_config.cfg_glob));
    }
}

static void btlink_cmd_exec_apn(btlink_parsed_dnlnk_frame_struct *dn_frame)
{
    btlink_arg_apn_struct *arg_apn = &(dn_frame->arg.apn);
    bool need_save = false;
	
		// <APN Quantity>
		if (g_btlink_config.cfg_apn.apn_quantity != arg_apn->apn_quantity)
		{
			g_btlink_config.cfg_apn.apn_quantity = arg_apn->apn_quantity;
			need_save = true;
		}
		
		// <MCC & MNC>
		if (strlen((char *)arg_apn->mcc_mnc) != 0
        && strncmp((char *)g_btlink_config.cfg_apn.mcc_mnc, (char *)arg_apn->mcc_mnc, MAX_MCC_MNC_LEN) != 0)
    {
        memset(g_btlink_config.cfg_apn.mcc_mnc, 0, MAX_MCC_MNC_LEN);
        strncpy((char *)g_btlink_config.cfg_apn.mcc_mnc, (char *)arg_apn->mcc_mnc, MAX_MCC_MNC_LEN);
        need_save = true;
    }

		// <APN Name>
		if (strlen((char *)arg_apn->apn_name) != 0
        && strncmp((char *)g_btlink_config.cfg_apn.apn_name, (char *)arg_apn->apn_name, MAX_APN_LEN) != 0)
    {
        memset(g_btlink_config.cfg_apn.apn_name, 0, MAX_APN_LEN);
        strncpy((char *)g_btlink_config.cfg_apn.apn_name, (char *)arg_apn->apn_name, MAX_APN_LEN);
        need_save = true;
    }
		
		// <APN User Name>
		if (strlen((char *)arg_apn->apn_user_name) != 0
        && strncmp((char *)g_btlink_config.cfg_apn.apn_user_name, (char *)arg_apn->apn_user_name, MAX_APN_USER_NAME_LEN) != 0)
    {
        memset(g_btlink_config.cfg_apn.apn_user_name, 0, MAX_APN_USER_NAME_LEN);
        strncpy((char *)g_btlink_config.cfg_apn.apn_user_name, (char *)arg_apn->apn_user_name, MAX_APN_USER_NAME_LEN);
        need_save = true;
    }
		
		// <APN Password>
		if (strlen((char *)arg_apn->apn_password) != 0
        && strncmp((char *)g_btlink_config.cfg_apn.apn_password, (char *)arg_apn->apn_password, MAX_APN_PASSWORD_LEN) != 0)
    {
        memset(g_btlink_config.cfg_apn.apn_password, 0, MAX_APN_PASSWORD_LEN);
        strncpy((char *)g_btlink_config.cfg_apn.apn_password, (char *)arg_apn->apn_password, MAX_APN_PASSWORD_LEN);
        need_save = true;
    }
		
		if (need_save == true)
		{
			//FlashCfgCmdParaWrite(BTLINK_FLASH_CFG_STRUCT_ADDR, (uint32 *)(&g_btlink_config.cfg_glob));
    }
}

static void btlink_cmd_exec_scs(btlink_parsed_dnlnk_frame_struct *dn_frame)
{
    btlink_arg_scs_struct *arg_scs = &(dn_frame->arg.scs);
    bool need_save = false;
	
		// <Data Zone Mask>
		if (g_btlink_config.cfg_scs.data_zone_mask != arg_scs->data_zone_mask)
		{
			g_btlink_config.cfg_scs.data_zone_mask = arg_scs->data_zone_mask;
			need_save = true;
		}
		
		if (need_save == true)
		{
			//FlashCfgCmdParaWrite(BTLINK_FLASH_CFG_STRUCT_ADDR, (uint32 *)(&g_btlink_config.cfg_glob));
    }
}

static void btlink_cmd_exec_lss(btlink_parsed_dnlnk_frame_struct *dn_frame)
{
    btlink_arg_lss_struct *arg_lss = &(dn_frame->arg.lss);
    bool need_save = false;
	
		// <Mode Selection>
		if (g_btlink_config.cfg_lss.mode_selection != arg_lss->mode_selection)
		{
			g_btlink_config.cfg_lss.mode_selection = arg_lss->mode_selection;
			need_save = true;
		}
		
		// <Continues Send Interval>
		if (g_btlink_config.cfg_lss.continue_send_interval != arg_lss->continue_send_interval)
		{
			g_btlink_config.cfg_lss.continue_send_interval = arg_lss->continue_send_interval;
			need_save = true;
		}

		// <Start Mode>
		if (g_btlink_config.cfg_lss.start_mode != arg_lss->start_mode)
		{
			g_btlink_config.cfg_lss.start_mode = arg_lss->start_mode;
			need_save = true;
		}

		// <Specified Time of Day>
		if (strlen((char *)arg_lss->spec_time_of_day) != 0
        && strncmp((char *)g_btlink_config.cfg_lss.spec_time_of_day, (char *)arg_lss->spec_time_of_day, MAX_SPEC_TIME_DAY_LEN) != 0)
    {
        memset(g_btlink_config.cfg_lss.spec_time_of_day, 0, MAX_SPEC_TIME_DAY_LEN);
        strncpy((char *)g_btlink_config.cfg_lss.spec_time_of_day, (char *)arg_lss->spec_time_of_day, MAX_SPEC_TIME_DAY_LEN);
        need_save = true;
    }

		// <Wakeup Interval>
		if (g_btlink_config.cfg_lss.wakeup_interval != arg_lss->wakeup_interval)
		{
			g_btlink_config.cfg_lss.wakeup_interval = arg_lss->wakeup_interval;
			need_save = true;
		}

		// <PSM Network Hold Time>
		if (g_btlink_config.cfg_lss.psm_net_hold_time != arg_lss->psm_net_hold_time)
		{
			g_btlink_config.cfg_lss.psm_net_hold_time = arg_lss->psm_net_hold_time;
			need_save = true;
		}

		// <Report Frequency>
		if (g_btlink_config.cfg_lss.report_freq != arg_lss->report_freq)
		{
			g_btlink_config.cfg_lss.report_freq = arg_lss->report_freq;
			need_save = true;
		}

		// <GNSS Enable>
		if (g_btlink_config.cfg_lss.gnss_enable != arg_lss->gnss_enable)
		{
			g_btlink_config.cfg_lss.gnss_enable = arg_lss->gnss_enable;
			need_save = true;
		}

		// <GNSS Fix Delay>
		if (g_btlink_config.cfg_lss.gnss_fix_delay != arg_lss->gnss_fix_delay)
		{
			g_btlink_config.cfg_lss.gnss_fix_delay = arg_lss->gnss_fix_delay;
			need_save = true;
		}

		// <AGPS Mode>
		if (g_btlink_config.cfg_lss.agps_mode != arg_lss->agps_mode)
		{
			g_btlink_config.cfg_lss.agps_mode = arg_lss->agps_mode;
			need_save = true;
		}
		
		// <Battery Low Percent>
		if (g_btlink_config.cfg_lss.bat_low_percent != arg_lss->bat_low_percent)
		{
			g_btlink_config.cfg_lss.bat_low_percent = arg_lss->bat_low_percent;
			need_save = true;
		}

		// <Sensor Enable>
		if (g_btlink_config.cfg_lss.sensor_enable != arg_lss->sensor_enable)
		{
			g_btlink_config.cfg_lss.sensor_enable = arg_lss->sensor_enable;
			need_save = true;
		}

		// <Non-Movement Duration>
		if (g_btlink_config.cfg_lss.non_move_duration != arg_lss->non_move_duration)
		{
			g_btlink_config.cfg_lss.non_move_duration = arg_lss->non_move_duration;
			need_save = true;
		}
		
		if (need_save == true)
		{
			//FlashCfgCmdParaWrite(BTLINK_FLASH_CFG_STRUCT_ADDR, (uint32 *)(&g_btlink_config.cfg_glob));
    }
}

static void btlink_cmd_exec(btlink_parsed_dnlnk_frame_struct *dn_frame)
{
    switch (dn_frame->type)
    {
        case BTLINK_FH_ID_DBG:
            btlink_cmd_exec_dbg(dn_frame);
            break;

				case BTLINK_FH_ID_IPS:
						btlink_cmd_exec_ips(dn_frame);
						break;
				
				case BTLINK_FH_ID_APN:
						btlink_cmd_exec_apn(dn_frame);
						break;
    
				case BTLINK_FH_ID_SCS:
						btlink_cmd_exec_scs(dn_frame);
						break;
				
				case BTLINK_FH_ID_LSS:
						btlink_cmd_exec_lss(dn_frame);
						break;
				
				case BTLINK_FH_ID_RTH:
						break;

        default:
            break;
    }
}

void btlink_cmd_process(btlink_parsed_dnlnk_frame_struct *dn_frame)
{
	//uint8_t tmp_buff[BTLINK_MAX_ACK_FRAME_SIZE] = {0};
	btlink_cmd_exec(dn_frame);

	#if 0
	//Step2 Assemble and send ACK frame
	if (dn_frame->ack)
	{
		memset(&(g_btlink_config.cfg_cmdack), 0, sizeof(btlink_config_cmd_ack_struct));
		g_btlink_config.cfg_cmdack.flag = false;
		#if 0
		btlink_assemble_ack_frame(dn_frame);
		//btlink_make_ack_report_packet(&g_resp_report_items.ack, parsed_at_cmd.character);
		#else
		switch (dn_frame->type)
		{
			case BTLINK_FH_ID_DBG:
			case BTLINK_FH_ID_CFG:
			{
				snprintf((char *)tmp_buff, BTLINK_FLD_LEN_CAN_DATA,
					"%s%s"    //+ACK:BTXXX
					"%c%s"    //parameter string  //serial_number
					"%c%04X"  //count number
					"%c",    // Tail
					BTLINK_ACK_HEADER, btlink_dn_frame_header_str[dn_frame->type],
					BTLINK_CHR_SEPARATOR, dn_frame->para_string, /*dn_frame->serial_number,*/
					BTLINK_CHR_SEPARATOR, btlink_get_report_count(),
					BTLINK_CHR_TAIL);
				btlink_assemble_dat_cmd_string(tmp_buff, g_btlink_config.cfg_cmdack.data);
				g_btlink_config.cfg_cmdack.flag = true;
				break;
			}

			default:
				break;
		}
		#endif
	}
	#endif
}

/*****************************************************************************************
//函数名称：btlink_print_reboot_log
//功能描述：打印开机log函数
//输入参数：
//输出参数：无
//返回参数：void
//补充说明：
******************************************************************************************/
void btlink_print_reboot_log(void)
{
	uint8_t tmp_buff[BTLINK_MAX_ACK_FRAME_SIZE] = {0};
	uint8_t len = 0;
	
	len = btlink_assemble_dat_cmd_string((uint8_t *)REBOOT_LOG, tmp_buff);
	//usart3_PutSendData(tmp_buff, len);
}
