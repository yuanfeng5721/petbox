/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Wireless Solutions.
*
*****************************************************************************/
/*******************************************************************************
 *
 * Filename:
 * ---------
 *	btlink_protocol_cmd.h
 *
 * Project:
 * --------
 *  
 *
 * Description:
 * ------------
 *   AVL GPS tracker protocol global configuration nvram definition
 *
 * Author:
 * -------
 * -------
 *
 *******************************************************************************/

#ifndef _BTLINK_PROTOCOL_CMD_H_
#define _BTLINK_PROTOCOL_CMD_H_

#include "trace.h"
#include "btlink_protocol_hdlr.h"

/***************************************************************************** 
* Typedef for NVRAM Data
*****************************************************************************/
typedef struct {
    uint8_t   report_mode;
    uint8_t   buffer_mode;
		/* Main Server IP & DNS */
    uint8_t   pri_mode;
    uint8_t 	pri_host[1+BTLINK_LEN_IPS_HOST];
    sockaddr_struct pri;
		/* Backup Server IP & DNS */
		uint8_t   sec_mode;
    uint8_t 	sec_host[1+BTLINK_LEN_IPS_HOST];
    sockaddr_struct sec;
} btlink_config_ips_struct;

typedef struct {
    uint8_t   apn_quantity;
    char mcc_mnc[MAX_MCC_MNC_LEN];
		char apn_name[MAX_APN_LEN];
		char apn_user_name[MAX_APN_USER_NAME_LEN];
		char apn_password[MAX_APN_PASSWORD_LEN];
} btlink_config_apn_struct;

typedef struct {
    uint32_t   data_zone_mask;
} btlink_config_scs_struct;

typedef struct {
    uint8_t   mode_selection;
		uint16_t  continue_send_interval;
		uint8_t   start_mode;
		char      spec_time_of_day[MAX_SPEC_TIME_DAY_LEN];  /*HHMM*/
		uint8_t   wakeup_interval;
		uint32_t  psm_net_hold_time;
		uint8_t   report_freq; 
		uint8_t   gnss_enable;
		uint8_t   gnss_fix_delay;
		uint8_t   agps_mode;
		uint8_t   bat_low_percent;
		uint8_t   sensor_enable;
		uint8_t   non_move_duration;
} btlink_config_lss_struct;

typedef struct {
    uint8_t   sub_command;
} btlink_config_rth_struct;

/***************************************************************************** 
* Terminal set Configuration Setting
* Refer to ALL response
*****************************************************************************/
typedef struct {
    btlink_config_ips_struct             cfg_ips;
    btlink_config_apn_struct             cfg_apn;
		btlink_config_scs_struct             cfg_scs;
		btlink_config_lss_struct             cfg_lss;
		btlink_config_rth_struct             cfg_rth;
} btlink_config_struct;

/***************************************************************************** 
* Extern Global Variable
*****************************************************************************/
//Current terminal configuration setting
extern btlink_config_struct g_btlink_config;
extern void btlink_cmd_process(btlink_parsed_dnlnk_frame_struct *dn_frame);

/*****************************************************************************************
//函数名称：btlink_print_reboot_log
//功能描述：打印开机log
//输入参数：
//输出参数：无
//返回参数：void
//补充说明：
******************************************************************************************/
extern void btlink_print_reboot_log(void);
#endif //_BTLINK_PROTOCOL_CMD_H_
