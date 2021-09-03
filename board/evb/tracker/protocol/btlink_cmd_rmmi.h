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


#ifndef __BTLINK_CMD_RMMI_H__
#define __BTLINK_CMD_RMMI_H__
/******************************************************************************
* Include Files
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "cmd_parse.h"
#include "btlink_protocol_def.h"

/*****************************************************************************
* Define
*****************************************************************************/
#define SEMICOLON_CHAR       (';')
#define QUOTES_CHAR          ('\"')
#define COMMA_CHAR           (',')
#define QUERY_CHAR           ('?')
#define EQUALS_CHAR          ('=')
#define PLUS_CHAR            ('+')
#define NEGATIVE_CHAR        ('-')
#define BTLINK_GTCMD_HEADER       "AT@"
#define BTLINK_GTCMD_HEADER_LEN   3
#define BTLINK_ATRSP_HEADER       "+QRY"   /*AT command response header string*/

#define MIN_VALUE(a, b) ((a) < (b)) ? (a) : (b)
#define MAX_VALUE(a, b) ((a) > (b)) ? (a) : (b)

#define COMMAND_LINE_SIZE    (180)

typedef enum ExtendedOperationTag
{
  INVALID_EXTENDED_OPERATION,
  EXTENDED_ACTION,
  EXTENDED_QUERY,
  EXTENDED_ASSIGN,
  EXTENDED_RANGE,
  NUM_OF_EXTENDED_OPERATIONS
} ExtendedOperation_t;

/*****************************************************************************
* Typedef
*****************************************************************************/
typedef struct CommandLineTag
{
  //l4c_source_id_enum src_id;
  uint16_t  position;
  uint16_t length;
  bool  is_negative;
  char  character[COMMAND_LINE_SIZE+1];
} CommandLine_t;

typedef T_CMD_PARSE_RESULT (*VgQProcFunc)(CommandLine_t *commandBuffer_p);
typedef struct BtlinkAtCmdControlTag
{
  const char      *string;        /* Command string */
  const VgQProcFunc   procFunc;   /* Command line processing function */
} BtlinkAtCmdControl;

/*****************************************************************************
* extern function
*****************************************************************************/
extern bool rmmi_is_btlink_cmd(uint8_t *string_ptr);
extern bool rmmi_btlink_cmd_hdlr(CommandLine_t *command_line);
#endif /* __BTLINK_CMD_RMMI_H__ */

