/*
*******************************************************************************
*
*     Copyright (c) 2021 Btlink Ltd.
*
*******************************************************************************
*  file name:           btlink_assem_msg_struct.h
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
#ifndef __BTLINK_ASSEM_MSG_STRUCT_H__
#define __BTLINK_ASSEM_MSG_STRUCT_H__

/******************************************************************************
* Include Files
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/***************************************************************************** 
* Types
*****************************************************************************/ 
typedef union 
{ 
	uint16_t num;
	uint8_t c[2]; 
} btlink_u16_u8_union;

typedef union 
{ 
	uint32_t num;
	uint8_t  c[4]; 
} btlink_u32_u8_union;

typedef union 
{ 
	int32_t num;
	uint8_t  c[4]; 
} btlink_s32_u8_union;

typedef union 
{ 
	float num;
	uint8_t    c[4]; 
} btlink_float_u8_union;

#endif /* __BTLINK_ASSEM_MSG_STRUCT_H__ */
