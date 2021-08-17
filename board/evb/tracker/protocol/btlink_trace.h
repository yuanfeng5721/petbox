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
 *	btlink_trace.h
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

#ifndef _BTLINK_TRACE_H_
#define _BTLINK_TRACE_H_

/******************************************************************************
* Include Files
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <trace.h>

#include "btlink_protocol_cmd.h"
/*****************************************************************************
* Define
*****************************************************************************/
#define QUEC_TRACE_BUFF_SIZE    160

typedef enum
{
    DBG_QNMEA,     /*0x0001*/
    DBG_QPROT,     /*0x0002*/ 
    DBG_QOTA,      /*0x0004*/
    DBG_QGPS,      /*0x0008*/
    DBG_QIO,       /*0x0010*/
    DBG_TYPE_NUM
}btlink_debug_type_e;

/******************************************************************************
* Global functions
******************************************************************************/
extern void btlink_trace(uint8_t dbg_idx, char *file_ptr,
                            uint32_t line, const char *fmt,...);

#define BTLINK_DEBUG_TRACE(dBG_iDX, ...)   \
    if ((dBG_iDX < DBG_TYPE_NUM) && (g_btlink_config.cfg_dbg.dbg_mode & (1 << dBG_iDX))) \
        btlink_trace(dBG_iDX, __FILE__, __LINE__, __VA_ARGS__);

#endif /* _BTLINK_TRACE_H_ */
