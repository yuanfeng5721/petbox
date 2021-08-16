/*
*******************************************************************************
*
*     Copyright (c) 2021 Btlink Ltd.
*
*******************************************************************************
*  file name:           btlink_protocol_def.h
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


#ifndef __BTLINK_PROTOCOL_DEF_H__
#define __BTLINK_PROTOCOL_DEF_H__
/******************************************************************************
* Include Files
******************************************************************************/

/*****************************************************************************
* Define
*****************************************************************************/
#define MAX_UART_LEN                128
#define MAX_CC_ADDR_LEN             42

#define BTLINK_GPRS_ACC_LID_REC         5

//The maximum of characters to pre-check if a sms is protocol specific
//"AT+GT" 5 characters
#define BTLINK_HEADER_CHKLEN            3     //Max SMS content check length
#define BTLINK_COMPLETE_HEADER_CHKLEN   8     //"AT+GTXXX" 8 characters
#define BTLINK_FLD_LEN_HEADER           3     //Max Header
#define BTLINK_FLD_LEN_PASSWORD         20     //Pincode
#define BTLINK_FLD_LEN_SEND_TIME        14    //send time
#define BTLINK_FLD_LEN_SERIAL_NUM       4     //Serial number
#define BTLINK_FLD_LEN_CONTENT          255   //Content
#define BTLINK_FLD_LEN_RESERVED         0     //Reserved arg

#define BTLINK_MAX_DNLNK_FRAME_SIZE     160 //1400   //Max length of donwlink frame (160 at most)

#define BTLINK_MAX_HBD_FRAME_SIZE       80      //Max length of heartbeat frame
#define BTLINK_MAX_ACK_FRAME_SIZE       128      //Max length of ACK frame
#define BTLINK_MAX_SMS_FRAME_SIZE       160
//#define BTLINK_MAX_ATRSP_FRAME_SIZE     512

//Definition for protocol message
#define BTLINK_DNLNK_HEADER         "AT@"     //Donwlink header string, captial case
#define BTLINK_DNLNK_HEADER_LC      "at@"     //Donwlink header string, lower case
#define BTLINK_ACK_HEADER           "BTACK:" //"+ACK:BT"   //ACK header string
#define BTLINK_CHR_SEPARATOR        0x2c        //Field separator char ','
#define BTLINK_CHR_ATSEP_HDR        0x3a        //AT response header separator char ':'
#define BTLINK_CHR_TAIL             0x23        //protocol tail char '#'
#define BTLINK_CHR_CRLF             "\r\n"
#define BTLINK_FLD_LEN_IMEI         15          //IMEI
#define BTLINK_ERROR_UID            "XXXXXXXXXXXXXXX" //default error UID
#define BTLINK_FLD_LEN_FLAG         1           //Flag for enable/disable
#define BTLINK_FLD_LEN_PARAMETER    128
#define BTLINK_FLD_LEN_SMS_GATE     20

#define BTLINK_TMZ_STRUCT_CORE \
    BOOL negative; \
    S8   hour; \
    S8   min; \
    BOOL dst;

/* IPS */
#define BTLINK_FLD_LEN_REPORT_MODE 1
#define BTLINK_FLD_LEN_BUFFER_MODE 1
#define BTLINK_FLD_LEN_HOST 60
#define BTLINK_FLD_LEN_PORT 5

#define BTLINK_MIN_IPS_RERORT_MODE 0
#define BTLINK_MAX_IPS_RERORT_MODE 2
#define BTLINK_MIN_IPS_BUFFER_MODE 0
#define BTLINK_MAX_IPS_BUFFER_MODE 2
#define BTLINK_MIN_IPS_PORT 0
#define BTLINK_MAX_IPS_PORT 65535

/* APN */
#define BTLINK_FLD_LEN_APN_NUMS  2
#define BTLINK_FLD_LEN_MCC_MNC   5
#define BTLINK_FLD_LEN_APN_NAME   40
#define BTLINK_FLD_LEN_APN_USER_NAME   30
#define BTLINK_FLD_LEN_APN_PASSWORD    30

#define BTLINK_MIN_APN_NUMS 0
#define BTLINK_MAX_APN_NUMS 10

#define MAX_MCC_MNC_LEN 7
#define MAX_APN_USER_NAME_LEN 32
#define MAX_APN_PASSWORD_LEN  32
#define MAX_APN_LEN       42

/* SCS */
#define BTLINK_FLD_LEN_DATA_ZONE_MASK  8

/* LSS */
#define BTLINK_FLD_LEN_MODE_SELECT         1
#define BTLINK_FLD_LEN_CONT_SEND_INTERVAL  4
#define BTLINK_FLD_LEN_START_MODE          1
#define BTLINK_FLD_LEN_SPEC_TIME_DAY       4
#define BTLINK_FLD_LEN_WAKEUP_INTERVAL     2
#define BTLINK_FLD_LEN_PSM_NET_HOLD_TIME   5
#define BTLINK_FLD_LEN_REPORT_FREG         3
#define BTLINK_FLD_LEN_GNSS_ENABLE         1
#define BTLINK_FLD_LEN_GNSS_FIX_DELAY      2
#define BTLINK_FLD_LEN_AGPS_MODE           1
#define BTLINK_FLD_LEN_BAT_LOW_PERCENT     2
#define BTLINK_FLD_LEN_SENSOR_ENABLE       1
#define BTLINK_FLD_LEN_NON_MOVE_DUR        3

#define BTLINK_MIN_LSS_MODE_SELECT    0
#define BTLINK_MAX_LSS_MODE_SELECT    2
#define BTLINK_MIN_LSS_CONT_SEND_INTERNVAL    0
#define BTLINK_MAX_LSS_CONT_SEND_INTERNVAL    1440
#define BTLINK_MIN_LSS_START_MODE    0
#define BTLINK_MAX_LSS_START_MODE    2
#define BTLINK_LSS_WAKEUP_INTERVAL_1    1
#define BTLINK_LSS_WAKEUP_INTERVAL_4    4
#define BTLINK_LSS_WAKEUP_INTERVAL_6    6
#define BTLINK_LSS_WAKEUP_INTERVAL_8    8
#define BTLINK_LSS_WAKEUP_INTERVAL_12   12
#define BTLINK_LSS_WAKEUP_INTERVAL_24   24
#define BTLINK_MIN_LSS_NET_HOLD_TIME     0
#define BTLINK_MAX_LSS_NET_HOLD_TIME     86400
#define BTLINK_MIN_LSS_REPORT_FREQ       1
#define BTLINK_MAX_LSS_REPORT_FREQ       100
#define BTLINK_MIN_LSS_GNSS_ENABLE       0
#define BTLINK_MAX_LSS_GNSS_ENABLE       1
#define BTLINK_MIN_LSS_GNSS_FIX_DELAY    5
#define BTLINK_MAX_LSS_GNSS_FIX_DELAY    60
#define BTLINK_MIN_LSS_AGPS_MODE         0
#define BTLINK_MAX_LSS_AGPS_MODE         1
#define BTLINK_MIN_LSS_BAT_LOW_PERCENT   0
#define BTLINK_MAX_LSS_BAT_LOW_PERCENT   30
#define BTLINK_MIN_LSS_SENSOR_ENABLE     0
#define BTLINK_MAX_LSS_SENSOR_ENABLE     1
#define BTLINK_MIN_LSS_NON_MOVE_DUR      1
#define BTLINK_MAX_LSS_NON_MOVE_DUR      100

#define MAX_SPEC_TIME_DAY_LEN                5

/* RTH */
#define BTLINK_FLD_LEN_SUB_COMMAND        1
#define BTLINK_MIN_RTH_SUB_COMMAND        0
#define BTLINK_MAX_RTH_SUB_COMMAND        7

#define BTLINK_FLD_LEN_NMEA_MASK   4
#define BTLINK_NMEA_PRINT_MASK_RMC                            0x0001
#define BTLINK_NMEA_PRINT_MASK_GSV                            0x0002
#define BTLINK_NMEA_PRINT_MASK_GSA                            0x0004
#define BTLINK_NMEA_PRINT_MASK_GGA                            0x0008
#define BTLINK_NMEA_PRINT_MASK_GLL                            0x0010
#define BTLINK_NMEA_PRINT_MASK_VTG                            0x0020
#define BTLINK_NMEA_PRINT_MASK_TXT                            0x0040
#define BTLINK_NMEA_PRINT_MASK_FILTER                         0x007F
#endif /*__BTLINK_PROTOCOL_DEF_H__*/
