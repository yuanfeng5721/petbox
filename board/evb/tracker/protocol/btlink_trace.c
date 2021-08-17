/*
*******************************************************************************
*
*     Copyright (c) 2021 btlink Ltd.
*
*******************************************************************************
*  file name:           btlink_trace.c
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

/******************************************************************************
* Include Files
******************************************************************************/
#include "btlink_trace.h"

/*****************************************************************************
* Define
*****************************************************************************/

/*****************************************************************************
* Typedef
*****************************************************************************/
typedef struct btlink_trace_header_tag
{
    const char  *hdr_str;
}btlink_trace_header_struct;

/*****************************************************************************
* Extern Variable
*****************************************************************************/

/*****************************************************************************
* Extern Function
*****************************************************************************/

/*****************************************************************************
 * global variable
 *****************************************************************************/
const btlink_trace_header_struct trace_module_table[DBG_TYPE_NUM + 1] =
{
    "NMEA",  /*0 DBG_QNMEA*/
    "PROT",  /*1 DBG_QPROT*/
    "OTA",   /*2 DBG_QOTA*/
    "GPS",   /*3 DBG_QGPS*/
    "IO",    /*4 DBG_QIO*/
		"NULL"
};
/*****************************************************************************
 * Local variable
 *****************************************************************************/
 static char g_trace_buff[QUEC_TRACE_BUFF_SIZE];

/*****************************************************************************
 * Local function
 *****************************************************************************/
/******************************************************************************
* Function    : btlink_trace
* 
* Author      : Eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : The internal function to debug for queclink.
******************************************************************************/
void btlink_trace(uint8_t dbg_idx, char *file_ptr,
                            uint32_t line, const char *fmt,...)
{
    uint16_t  str_len = 0;
    char    *name_ptr = NULL;
    va_list     va;

    str_len = sprintf (g_trace_buff, "[%s]: ", trace_module_table[dbg_idx].hdr_str);

    /*Add the detailed trace information after the header*/    
    va_start (va, fmt);
    str_len += vsnprintf ((g_trace_buff + str_len), (QUEC_TRACE_BUFF_SIZE - str_len), fmt, va);
    va_end (va);

    if (g_btlink_config.cfg_dbg.dbg_mode)
    {
        /*Add the debug end, including file name and line number information*/
        name_ptr = strrchr (file_ptr, '\\');    /*Find the last '\' to get the pure name*/
        if (NULL == name_ptr)
        {
            name_ptr = file_ptr;
        }
        else
        {
            name_ptr++;                         /*Skip the character '\'*/
        }
        str_len += snprintf ((g_trace_buff + str_len), (QUEC_TRACE_BUFF_SIZE - str_len),
                    " F-%s, L-%d", name_ptr, line);

        //rmmi_write_to_uart((kal_uint8*)g_trace_buff, strlen(g_trace_buff), KAL_TRUE);
    }
		else		
    {
			 DBG_DIRECT("%s", g_trace_buff);
    }
}



