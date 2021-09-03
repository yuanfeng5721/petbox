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

#ifndef _BTLINK_ASSEM_ASC_MSG_H_
#define _BTLINK_ASSEM_ASC_MSG_H_

#include "trace.h"
#include "btlink_protocol_def.h"
#include "btlink_protocol_enums.h"

extern uint16_t btlink_prot_print_cfg_head_ascii(uint8_t *buff, uint16_t buf_len, uint8_t type);
extern uint16_t btlink_prot_print_cfg_tail_ascii(uint8_t* buff, uint16_t buf_len);
#endif
