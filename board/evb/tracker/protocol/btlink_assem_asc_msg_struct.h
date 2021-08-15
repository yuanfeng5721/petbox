/*
*******************************************************************************
*
*     Copyright (c) 2021 Btlink Ltd.
*
*******************************************************************************
*  file name:           btlink_assem_asc_msg_struct.h
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
#ifndef __BTLINK_ASSEM_ASC_MSG_STRUCT_H__
#define __BTLINK_ASSEM_ASC_MSG_STRUCT_H__

/******************************************************************************
* Include Files
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "btlink_assem_asc_msg_enums.h"
/***************************************************************************** 
* Struct
*****************************************************************************/ 
typedef struct
{
    uint16_t nYear;
    uint8_t nMonth;
    uint8_t nDay;
    uint8_t nHour;
    uint8_t nMin;
    uint8_t nSec;
    uint8_t DayIndex; /* 0=Sunday */
} btlink_time_struct;

typedef struct
{
	uint16_t mcc;
	uint16_t mnc;
	uint16_t lac;
	uint32_t cell_id;
	uint8_t  csq;
} btlink_cell_info_struct;

typedef struct 
{
    uint8_t fix;
    uint8_t speed    [BTLINK_FLD_LEN_SPEED_GPS + 1];   /*km/h*/
    uint8_t heading  [BTLINK_FLD_LEN_HEADING_GPS + 1];
    uint8_t altitude [BTLINK_FLD_LEN_ALTITUDE_GPS + 1];
    uint8_t accuracy [BTLINK_FLD_LEN_ACCURACY_GPS + 1];
    uint8_t lng      [BTLINK_FLD_LEN_LONGITUDE + 1];
    uint8_t lat      [BTLINK_FLD_LEN_LATITUDE + 1 ];
    uint8_t time     [BTLINK_FLD_LEN_TIME_GPS + 1 ];
    uint8_t corner;
} btlink_gps_info_struct;
#endif /* __BTLINK_ASSEM_ASC_MSG_STRUCT_H__ */
