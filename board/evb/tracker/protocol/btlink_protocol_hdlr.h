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
 *	btlink_protocol_hdlr.h
 *
 * Project:
 * --------
 *  
 *
 * Description:
 * ------------
 *   AVL GPS tracker protocol message definition
 *
 * Author:
 * -------
 * -------
 *
 *******************************************************************************/

#ifndef _BTLINK_PROTOCOL_HDLR_H_
#define _BTLINK_PROTOCOL_HDLR_H_

#include "trace.h"
#include "btlink_protocol_def.h"
#include "btlink_protocol_enums.h"

/***************************************************************************** 
* Downlink Frame enums 
*****************************************************************************/
#define CMD_IGNORE      0
#define CMD_ERROR       1
#define CMD_ACCEPT      2

/***************************************************************************** 
* Typedef 
*****************************************************************************/
//Caution:  If the max downlink frame size > 256, change 3 definitions:
//GPRT_MAX_DNLNK_FRAM_SIZE
//last_position & lenght should be uint16_t or larger
typedef struct {
    uint8_t position;                             //Next operation start position
    uint8_t length;                               //Length of the raw frame, exclude the end null
    uint8_t data[BTLINK_MAX_DNLNK_FRAME_SIZE];      //Frame content
} btlink_raw_dnlnk_frame_struct;               //Raw donwlink frame via SMS

typedef struct {
    uint32_t dbg_mode;
} btlink_arg_dbg_struct;

typedef struct {
    uint32_t  send_interval;
    uint8_t   new_password[1+BTLINK_LEN_PASSWORD];
    uint8_t   dat_cmd_mode;
		uint8_t   fc_report_mode;
		uint16_t  fc_report_valid_time;
} btlink_arg_cfg_struct;

/* socket address structure */
typedef struct 
{
    socket_type_enum	sock_type; /* socket type */
    uint16_t	addr_len; /* address length */
    uint16_t	port; /* port number */
    uint8_t	addr[MAX_SOCK_ADDR_LEN];
    /* IP address. For keep the 4-type boundary, 
     * please do not declare other variables above "addr"
    */
} sockaddr_struct;

typedef struct {
    uint8_t   report_mode;
    uint8_t   buffer_mode;
		/* Main Server IP & DNS */
    uint8_t   pri_mode;
    uint8_t 	pri_host[BTLINK_LEN_IPS_HOST];
    sockaddr_struct pri;
		/* Backup Server IP & DNS */
		uint8_t   sec_mode;
    uint8_t 	sec_host[BTLINK_LEN_IPS_HOST];
    sockaddr_struct sec;
} btlink_arg_ips_struct;

typedef struct {
    uint8_t apn_quantity;
    uint8_t mcc_mnc[BTLINK_MAX_APN_NUMS][MAX_MCC_MNC_LEN];
		uint8_t apn_name[BTLINK_MAX_APN_NUMS][MAX_APN_LEN];
		uint8_t apn_user_name[BTLINK_MAX_APN_NUMS][MAX_APN_USER_NAME_LEN];
		uint8_t apn_password[BTLINK_MAX_APN_NUMS][MAX_APN_PASSWORD_LEN];
} btlink_arg_apn_struct;

typedef struct {
		uint8_t   dev_password[MAX_APN_PASSWORD_LEN];
    uint32_t  data_zone_mask;
} btlink_arg_scs_struct;

typedef struct {
    uint8_t   mode_selection;
		uint16_t  continue_send_interval;
		uint8_t   start_mode;
		uint8_t   spec_time_of_day[MAX_SPEC_TIME_DAY_LEN];  /*HHMM*/
		uint8_t   wakeup_interval;
		uint32_t  psm_net_hold_time;
		uint8_t   report_freq; 
		uint8_t   gnss_enable;
		uint8_t   gnss_fix_delay;
		uint8_t   agps_mode;
		uint8_t   bat_low_percent;
		uint8_t   sensor_enable;
		uint8_t   non_move_duration;
} btlink_arg_lss_struct;

typedef struct {
    uint8_t   sub_command;
} btlink_arg_rth_struct;

typedef union {
		btlink_arg_ips_struct    ips;                       //[AT@IPS]
		btlink_arg_apn_struct    apn;                       //[AT@APN]
		btlink_arg_scs_struct    scs;                       //[AT@SCS]
		btlink_arg_lss_struct    lss;                       //[AT@LSS]
		btlink_arg_rth_struct    rth;                       //[AT@RTH]
} btlink_dnlnk_arg_union;

/***************************************************************************** 
* ACK Frame Typedef 
*****************************************************************************/
typedef struct {
    btlink_frame_header_index  type;
    uint8_t                       length;
    uint8_t                       data[BTLINK_MAX_ACK_FRAME_SIZE+1];
    uint8_t                       oa_number[BTLINK_LEN_SMS_GATE+1];
} btlink_ack_frame_struct;

//Parsed downlink frame
typedef struct {
    //Data
    btlink_frame_header_index  type;
    uint8_t  password[1+BTLINK_LEN_SCS_PASSWORD];
    btlink_dnlnk_arg_union   arg;
    bool                     ack;
    uint8_t                  oa_number [ 1+MAX_CC_ADDR_LEN ];
    //U8				     				 send_time [ 1 + GPRT_FLD_LEN_SEND_TIME ];
    uint8_t				           serial_number[ 1 + BTLINK_LEN_SERIAL_NUM];
    uint8_t                  para_string[ 1+ BTLINK_LEN_PARAMETER ];

    //Status
    bool                     valid;
    btlink_parse_step        step;       //current parse step
} btlink_parsed_dnlnk_frame_struct;

typedef struct {
    btlink_rsp_frame_header_index  type;
    uint16_t                       length;
    uint8_t                        data[BTLINK_MAX_DNLNK_FRAME_SIZE+1];
    uint16_t                       pdata_len;
    uint8_t *  pdata;
} btlink_rsp_frame_struct;

typedef struct
{
    uint16_t  position;
    uint16_t  length;
    char  character[BTLINK_MAX_DNLNK_FRAME_SIZE + 1];
} btlink_parsed_at_cmd_struct;

extern btlink_parsed_at_cmd_struct   parsed_at_cmd;
extern int btlink_cmd_parse(char *cmd, int16_t cmd_len);
extern bool btlink_assemble_ack_frame(btlink_parsed_dnlnk_frame_struct *dn_frame);
extern bool btlink_is_protocol_format(uint8_t* msg_content, uint16_t msg_length);
#endif //_BTLINK_PROTOCOL_HDLR_H_
