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
 *	btlink_protocol_util.h
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

#ifndef _BTLINK_PROTOCOL_UTIL_H_
#define _BTLINK_PROTOCOL_UTIL_H_

//#include "..\base\base.h"
#include "trace.h"
/***************************************************************************** 
* extern Local function
*****************************************************************************/
extern void btlink_set_report_no(unsigned int cnt);
extern unsigned int btlink_get_report_count(void);
extern bool btlink_utils_isalnum_buffer(uint8_t *buffer, uint8_t length);
extern bool btlink_utils_isalnum_and_connector_buffer(uint8_t * buffer, uint8_t length);
extern bool btlink_utils_isdigit_buffer(uint8_t *buffer, uint8_t length);
extern bool btlink_utils_isxdigit_buffer(uint8_t *buffer, uint8_t length);
extern bool btlink_utils_hasdigit_buffer(uint8_t *buffer, uint8_t length);
extern uint16_t gprt_ucs2_n_to_ascii(uint8_t * out_asc, uint8_t * in_ucs2, uint16_t len);
extern void btlink_utils_convert_hex_to_ascii(uint8_t * data);
extern int32_t btlink_utils_app_strnicmp(char *s, char *t, int n);
extern uint8_t btlink_applib_dt_dow(uint16_t y, uint8_t m, uint8_t d);
extern int32_t btlink_hex_string_2_int(char *str);
extern int btlink_utils_atoi_buffer(const char * str);
extern uint16_t btlink_assemble_dat_cmd_string(uint8_t *data, uint8_t *dat_cmd);
extern bool btlink_util_ip_all_zero(uint8_t* ip);
extern bool btlink_soc_ip_check(char *asci_addr, uint8_t *ip_addr, bool *ip_validity);
extern uint8_t btlink_util_char_to_int(uint8_t character);
#endif //_BTLINK_PROTOCOL_UTIL_H_
