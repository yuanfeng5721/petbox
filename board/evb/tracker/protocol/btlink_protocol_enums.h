/*
*******************************************************************************
*
*     Copyright (c) 2021 Btlink Ltd.
*
*******************************************************************************
*  file name:           btlink_protocol_enums.h
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


#ifndef __BTLINK_PROTOCOL_ENUMS_H__
#define __BTLINK_PROTOCOL_ENUMS_H__
/******************************************************************************
* Include Files
******************************************************************************/

/*****************************************************************************
* Enum
*****************************************************************************/
#define REBOOT_LOG "RDY CANJ1939 SW_Ver:A01V06"  /*can_message_assemble_enum.h文件中也需要修改软件版本号*/
#define CRLF			"\r\n"
#define COMMA_CHAR           (',')
#define MAX_SOCK_ADDR_LEN           (28)
#define IP_ADDR_LEN    (4)    
#define IP_ALL_MATCH(a, b)  (!((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2]) | (a[3] ^ a[3])))

/* Socket Type */
typedef enum
{
    SOC_SOCK_STREAM = 0,  /* stream socket, TCP */
    SOC_SOCK_DGRAM,       /* datagram socket, UDP */
    SOC_SOCK_SMS,         /* SMS bearer */
    SOC_SOCK_RAW          /* raw socket */
} socket_type_enum;

//Index of downlink frame header & uplink ACK frame header string
typedef enum
{
    BTLINK_FH_ID_DBG = 0,
    BTLINK_FH_ID_IPS,
    BTLINK_FH_ID_APN,
    BTLINK_FH_ID_SCS,
    BTLINK_FH_ID_LSS,
    BTLINK_FH_ID_RTH,
	
    BTLINK_FH_ID_NUM,          // Total number of frame header
    BTLINK_FH_ID_UNKNOWN       // Unknown frame header
} btlink_frame_header_index;

//Protocol parse step
typedef enum
{
    BTLINK_PARSE_STEP_TYPE,        //Parsing frame type
    BTLINK_PARSE_STEP_ARG,         //Parsing frame arg
    BTLINK_PARSE_STEP_SN,          //Parsing frame serial number
    BTLINK_PARSE_STEP_ACK,         //Parsing frame ACK
    BTLINK_PARSE_STEP_TAIL,        //Parsing frame tail character
    BTLINK_PARSE_STEP_UNKNOWN
} btlink_parse_step;

//Index of response uplink frame header string
typedef enum
{
    //Location
    BTLINK_FH_RSP_ID_PNL, 

    BTLINK_FH_RSP_ID_NUM        // Total number of response frame header
} btlink_rsp_frame_header_index;

/* IPS */
/* SRI Primary Server Mode */
typedef enum 
{
    BTLINK_IPS_PRI_IP = 0,        /*Primary server is set by IP address*/
    BTLINK_IPS_PRI_DN,            /*Primary server is set by domain name*/
    BTLINK_IPS_PRI_UNKNOWN
} btlink_ips_pri_mode;
#endif /*__BTLINK_PROTOCOL_ENUMS_H__*/

