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
 *	btlink_assem_hex_msg.h
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

#ifndef _BTLINK_ASSEM_HEX_MSG_H_
#define _BTLINK_ASSEM_HEX_MSG_H_

#include "trace.h"
#include "btlink_protocol_def.h"
#include "btlink_protocol_enums.h"
#include "btlink_assem_asc_msg_struct.h"

extern void btlink_assemm_hex_prt_msg_hdlr(uint8_t* buff, uint16_t event_type, btlink_gps_info_struct *pos);
#endif