/*
*******************************************************************************
*
*     Copyright (c) 2021 Btlink Ltd.
*
*******************************************************************************
*  file name:           btlink_assem_asc_msg_enums.h
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

#ifndef __BTLINK_ASSEM_ASC_MSG_ENUMS_H__
#define __BTLINK_ASSEM_ASC_MSG_ENUMS_H__

/*****************************************************************************
* Include Files
******************************************************************************/

/*****************************************************************************
* Enum
*****************************************************************************/
#define ASC_ACK_HEAD   "+ACK:"
#define ASC_NACK_HEAD  "+NACK:"
#define ASC_SACK_HEAD  "+SACK:"
#define ASC_HBD_HEAD   "+HBD:"
#define ASC_SHBD_HEAD  "+SHBD:"
#define ASC_QRY_HEAD   "+QRY:"
#define ASC_ALL_HEAD   "+ALL:"
#define ASC_QNI_HEAD   "+QNI:"
#define ASC_VER_HEAD   "+VER:"

#define BTLINK_FLD_LEN_SEND_TIME          14   /* send time */
#define BTLINK_FLD_LEN_SERIAL_NUM         4   /* Serial number */

/* GPS Position Info */
#define BTLINK_MAX_POS                        15   /*  Max number of GPS info. to rpeort in one message */
#define BTLINK_MAX_POS_SMS                    1   /* Max number of GPS info. to rpeort in one SMS */
#define BTLINK_MAX_POS_SMS_HEX                3   /* Max number of GPS info. to rpeort in one SMS in Hex format */
#define BTLINK_FLD_LEN_GPSFIX                 1
#define BTLINK_FLD_LEN_ACCURACY_GPS           2
#define BTLINK_FLD_LEN_SPEED_GPS              5
#define BTLINK_FLD_LEN_HEADING_GPS            3  
#define BTLINK_FLD_LEN_ALTITUDE_GPS           8  
#define BTLINK_FLD_LEN_LONGITUDE              11
#define BTLINK_FLD_LEN_LATITUDE               10
#define BTLINK_FLD_LEN_TIME_GPS               14
#define BTLINK_FLD_LEN_GSM_MCC                4
#define BTLINK_FLD_LEN_GSM_MNC                4
#define BTLINK_FLD_LEN_GSM_LAC                4
#define BTLINK_FLD_LEN_GSM_CELL_ID            4
#define BTLINK_FLD_LEN_GSM_TA                 2
#define BTLINK_HEX_LNGLAT_PRECISION           (1000000)
/*****************************************************************************
* Typedef
*****************************************************************************/

#endif /*__BTLINK_ASSEM_ASC_MSG_ENUMS_H__*/
