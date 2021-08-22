/*
*******************************************************************************
*
*     Copyright (c) 2021 btlink Ltd.
*
*******************************************************************************
*  file name:           btlink_cmd_rmmi.c
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
#include "ctype.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "cmd_uart.h"
#include "btlink_trace.h"
#include "btlink_cmd_rmmi.h"
#include "btlink_protocol_util.h"
/*****************************************************************************
* Define
*****************************************************************************/

/*****************************************************************************
* Typedef
*****************************************************************************/

/*****************************************************************************
* Extern Variable
*****************************************************************************/

/*****************************************************************************
* Extern Function
*****************************************************************************/
extern T_CMD_PARSE_RESULT vgDEBUG(CommandLine_t *commandBuffer_p);
extern bool btlink_check_and_exec_protocol(uint8_t *msg_content, uint16_t msg_length, uint8_t *number);
extern T_CMD_PARSE_RESULT rmmi_protocol_cmd_entry(CommandLine_t *commandBuffer_p);

/*****************************************************************************
 * global variable
 *****************************************************************************/
const BtlinkAtCmdControl g_btlink_at_cmd_table[BTLINK_CMD_NUMS+1] =
{
	{(char*)"@DEBUG",   vgDEBUG},
	{(char*)"@IPS",     rmmi_protocol_cmd_entry},
	{(char*)"@APN",     rmmi_protocol_cmd_entry},
	{(char*)"@SCS",     rmmi_protocol_cmd_entry},
	{(char*)"@LSS",     rmmi_protocol_cmd_entry},
	{(char*)"@RTH",     rmmi_protocol_cmd_entry},
	{NULL,              NULL}
};

/*****************************************************************************
 * Local variable
 *****************************************************************************/
static uint8_t at_dnlnk_content[BTLINK_MAX_DNLNK_FRAME_SIZE+2];
static uint16_t dnlnk_len = 0;

/*****************************************************************************
* Local function
*****************************************************************************/
/******************************************************************************
* Function    : isAsciiCode
* 
* Author      : page.zhong
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
static bool isAsciiCode (char c)
{
	bool result = true;

	if ((c >= 'a') && (c <= 'z'))
	{
		;
	}
	else
	{
		if ((c >= 'A') && (c <= 'Z'))
		{
			;
		}
		else
		{
			result = false;
		}
	}

	return (result);
}

/******************************************************************************
* Function    : getDecimalValue
* 
* Author      : page.zhong
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
int32_t getDecimalValue (CommandLine_t *commandBuffer_p)
{
  int32_t value = 0;

  while ((commandBuffer_p->position < commandBuffer_p->length) 
				 && (isdigit (commandBuffer_p->character[commandBuffer_p->position])))
  {
    value *= 10;
    value += (commandBuffer_p->character[commandBuffer_p->position] - '0');
    commandBuffer_p->position++;
  }

  #if 0
  if (commandBuffer_p->is_negative == KAL_TRUE)
  {
      value = -value;
  }
  #endif
  
  return (value);
}

 /******************************************************************************
* Function    : btlink_getExtendedOperation
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
ExtendedOperation_t btlink_getExtendedOperation (CommandLine_t *commandBuffer_p)
{
	ExtendedOperation_t result;
	commandBuffer_p->position = commandBuffer_p->position+2;

	if (commandBuffer_p->position < commandBuffer_p->length)
	{
		switch (commandBuffer_p->character[commandBuffer_p->position])
		{
			case QUERY_CHAR:  /* AT@...? */
			{
				commandBuffer_p->position++;
				result = EXTENDED_QUERY;
				break;
			}
			case EQUALS_CHAR:  /* AT@...= */
			{
				commandBuffer_p->position++;
				if ((commandBuffer_p->position < commandBuffer_p->length) 
					&& (commandBuffer_p->character[commandBuffer_p->position] == QUERY_CHAR))
				{
					commandBuffer_p->position++;
					result = EXTENDED_RANGE;
				}
				else
				{
					result = EXTENDED_ASSIGN;
				}
				break;
			}
			default:  /*  */
			{
				result = EXTENDED_ACTION;
				break;
			}
		}
	}
	else
	{
		result = EXTENDED_ACTION;
	}

	return (result);
}

/*--------------------------------------------------------------------------
 *
 * Function:    getExtendedParameter
 *
 * Parameters:  (InOut) commandBuffer_p - a pointer to a custom_cmdLine
 *              structure with the position field indicating the start of
 *              where a decimal value may be.
 *              (Out) value_p - a pointer to an Int32 value where the
 *              equivalent to the decimal value if given, or the value
 *              of the thrid parameter if not, is to be stored.
 *              (In) inValue - an Int32 value to be used if the parameter
 *              is not supplied.
 *
 * Returns:     Boolean indicating the success or failure in obtaining an
 *              extended AT command's parameter value.
 *
 * Description: Attempts to obtain the extended AT command parameter at
 *              position, moving position to the point after the parameter
 *              and returning TRUE if the extended parameter value returned
 *              in *value_p is OK, else FALSE if it is not.
 *
 *-------------------------------------------------------------------------*/
bool getExtendedParameter (CommandLine_t *commandBuffer_p,
                               int32_t *value_p, int32_t inValue)
{
  bool result = true;
  bool useInValue = true;
  int32_t   value = 0;

	//cmd_uart_print("position: %d\r\n", commandBuffer_p->position);
	//cmd_uart_print("length: %d\r\n", commandBuffer_p->length);
	//cmd_uart_print("character: %s\r\n", commandBuffer_p->character);
	
  if (commandBuffer_p->position < commandBuffer_p->length)
  {
    if (commandBuffer_p->character[commandBuffer_p->position] == COMMA_CHAR)
    {
      commandBuffer_p->position++;
    }
    else
    {
      if (commandBuffer_p->character[commandBuffer_p->position] != SEMICOLON_CHAR)
      {
        if (commandBuffer_p->character[commandBuffer_p->position] == NEGATIVE_CHAR)
        {
            //commandBuffer_p->is_negative = KAL_TRUE;
            commandBuffer_p->position++;
        }
        else if (commandBuffer_p->character[commandBuffer_p->position] == PLUS_CHAR)
        {
            //commandBuffer_p->is_negative = KAL_FALSE;
            commandBuffer_p->position++;
        }
        
        if (isdigit (commandBuffer_p->character[commandBuffer_p->position]))
        {
					//cmd_uart_print("character[%d]: %c\r\n", commandBuffer_p->position, commandBuffer_p->character[commandBuffer_p->position]);
					
          value = getDecimalValue (commandBuffer_p);
          *value_p = value;
          useInValue = false;
          if (commandBuffer_p->position < commandBuffer_p->length)
          {
            if (commandBuffer_p->character[commandBuffer_p->position] == COMMA_CHAR)
            {
              commandBuffer_p->position++;
            }
          }
        }
        else
        {
          result = false;
        }
      }
    }
  }

  if (useInValue == true)
  {
    *value_p = inValue;
  }

  return (result);
}

/******************************************************************************
* Function    : parseCommandBuffer
* 
* Author      : page.zhong
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
T_CMD_PARSE_RESULT parseCommandBuffer (BtlinkAtCmdControl const *atCommandTable_p, 
                                 CommandLine_t *commandBuffer_p) 
{
	bool            commandFound = false;
	BtlinkAtCmdControl    const *currCmd = NULL;
	char            commandLength = 0;
	bool            compare = true;
	int32_t           commandIndex = 0;
	T_CMD_PARSE_RESULT        retcode = RESULT_SUCESS;

	if (commandBuffer_p->length > 0)
	{
		commandFound = false;
		for (currCmd = atCommandTable_p;(commandFound == false) 
				&& (currCmd->string != NULL);currCmd++)
		{
			commandLength = strlen(currCmd->string);
			compare = true;
			//cmd_uart_print("currCmd->string: %s\r\n", currCmd->string);
			//cmd_uart_print("commandBuffer_p->character: %s\r\n", commandBuffer_p->character);
			while (((commandIndex) < commandLength) && (compare == true))
			{
				if (toupper (currCmd->string[commandIndex]) 
				!= toupper (commandBuffer_p->character[commandIndex+2]))
				{
					compare = false;
					break;
				}
				else
				{
					commandIndex++;
				}
			}
						
			if (compare == true)
			{
				commandFound = true;
				break;
			}
		}

		if (commandFound == true)
		{
			commandBuffer_p->position += commandLength;
			retcode = (currCmd->procFunc)(commandBuffer_p);
		}
		else
		{
			retcode = RESULT_CMD_NOT_SUPPORT;
		}
	}
	else
	{
		retcode = RESULT_CMD_NOT_SUPPORT;
	}

	return retcode;
}

/******************************************************************************
* Function    : rmmi_is_btlink_cmd
* 
* Author      : Eric
* 
* Parameters  : 
* 
* Return      : KAL_TRUE    this is a command in Btlink's command table.
* 
* Description : check whether the command is in Btlink's command table.
******************************************************************************/
bool rmmi_is_btlink_cmd(uint8_t *string_ptr)
{
	bool         ret_val = false;
	uint32_t     cmd_index = 0;      /*Check from the first one*/
	uint32_t     cmd_length;
	uint32_t     str_index;
	bool         is_match;

	//cmd_uart_print("string_ptr: %s\r\n", string_ptr);

	while (NULL != g_btlink_at_cmd_table[cmd_index].string)
	{
		cmd_length  = strlen(g_btlink_at_cmd_table[cmd_index].string);
		is_match    = true;
		str_index   = 0;

		while (str_index < cmd_length)
		{
			//cmd_uart_print("btlink_at_cmd_table[%d].string[%d]:%02X\r\n", cmd_index, str_index, g_btlink_at_cmd_table[cmd_index].string[str_index]);
			//cmd_uart_print("string_ptr[%d]:%02X\r\n", str_index, string_ptr[str_index+2]);

			if (toupper(g_btlink_at_cmd_table[cmd_index].string[str_index])
					!= toupper(string_ptr[str_index+2]))
			{
				is_match = false;
				break;
			}
			str_index++;
		}

		//cmd_uart_print("is_match:%d\r\n", is_match);

		if (is_match) /*This means the command is what we find*/
		{
			ret_val = true;
			break;
		}

		cmd_index++;
	}

	//cmd_uart_print("rmmi_is_btlink_cmd: %d\r\n", ret_val);

	return ret_val;
}

/******************************************************************************
* Function    : rmmi_btlink_cmd_hdlr
* 
* Author      : Eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : The entry for the command of btlink
******************************************************************************/
bool rmmi_btlink_cmd_hdlr(CommandLine_t *command_line)
{
    bool        ret = true;
    T_CMD_PARSE_RESULT    ret_code = RESULT_ERR;
    char buffer[COMMAND_LINE_SIZE+1]={0};

		//cmd_uart_print("source_string_ptr, length: %d, content: %s, position: %d\r\n", 
                //command_line->length, command_line->character, command_line->position);

    ret_code = parseCommandBuffer(g_btlink_at_cmd_table, command_line);

    if (ret_code != RESULT_SUCESS)
    {
			cmd_uart_print("%s\r\n", "ERROR");
    }
    else
    {
			cmd_uart_print("%s\r\n", "OK");
    }

    return ret;
}

/******************************************************************************
* Function    : vgDEBUG
* 
* Author      : Eric.xu
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
T_CMD_PARSE_RESULT vgDEBUG(CommandLine_t *commandBuffer_p)
{
	T_CMD_PARSE_RESULT    result = RESULT_SUCESS;
	ExtendedOperation_t operation = btlink_getExtendedOperation (commandBuffer_p);
	int32_t              value=0;
	char   buffer[MAX_UART_LEN];
	static bool qfct_flag = false;

	memset(buffer, 0, sizeof(buffer));

	switch (operation)
	{
		case EXTENDED_ASSIGN: /* AT@DEBUG= */
		{
			while ((commandBuffer_p->position < commandBuffer_p->length) 
				&&(isxdigit (commandBuffer_p->character[commandBuffer_p->position])))
			{
				value *= 16;
				value += btlink_util_char_to_int(commandBuffer_p->character[commandBuffer_p->position]);
				commandBuffer_p->position++;
			}
			
			BTLINK_DEBUG_TRACE(DBG_QPROT, "value:%04X", value);
      btlink_set_log_mode(value & 0xffffffff);
			break;
		}

		case EXTENDED_RANGE:   /* AT@DEBUG=? */
		case EXTENDED_QUERY:   /* AT@DEBUG?  */
		case EXTENDED_ACTION:  /* AT@DEBUG  */
		default:
		{
			cmd_uart_print("%s\r\n", "@DEBUG=0x01(NMEA), 0x02(PROT), 0x04(SEND), 0x08(GPS), 0x10(IO)");
			break;
		}
	}
	
	return (result);
}

/******************************************************************************
* Function    : rmmi_protocol_cmd_entry
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
T_CMD_PARSE_RESULT rmmi_protocol_cmd_entry(CommandLine_t *commandBuffer_p)
{
    T_CMD_PARSE_RESULT  result = RESULT_SUCESS;
    ExtendedOperation_t operation = btlink_getExtendedOperation (commandBuffer_p);

    switch (operation)
    {
        case EXTENDED_RANGE:  /* AT@XXX=? */
        {
            result = RESULT_ERR;
            break;
        }

        case EXTENDED_ASSIGN: /* AT@XXX= */
        {
            uint16_t min_len = MIN_VALUE(commandBuffer_p->length, BTLINK_MAX_DNLNK_FRAME_SIZE+1);    /*include CR*/
						
            memset((void *)at_dnlnk_content, 0 , sizeof(at_dnlnk_content));
            strncpy((char*)at_dnlnk_content, (char*)commandBuffer_p->character, min_len);
            dnlnk_len = strlen((const char*)at_dnlnk_content) - 1; /*remove CR*/
 
            cmd_uart_print("at_dnlnk_sms_content: %s,dnlnk_sms_len: %d", at_dnlnk_content, dnlnk_len);

						if (!btlink_check_and_exec_protocol(at_dnlnk_content, dnlnk_len, NULL))
						{
								result = RESULT_ERR;
						}
  
            break;
        }

        case EXTENDED_QUERY:   /* AT@XXX?  */
        {
						#if 0
            char password[GPRT_FLD_LEN_PASSWORD+1] = {0};
            int16_t length;
            if (quec_getExtendedString (commandBuffer_p, password, GPRT_FLD_LEN_PASSWORD, &length) == KAL_TRUE) 
            {
                kal_char buffer[MAX_UART_LEN] = {0};
                
                if (0 < length && quec_util_password_match((kal_uint8*)password))
                {
									result = quec_do_protocol_query((kal_char*)commandBuffer_p->character);
                }
                else 
                {
                    sprintf(buffer, "Password error");
                    rmmi_write_to_uart((kal_uint8*)buffer, strlen(buffer), KAL_TRUE);
                    result = RESULT_CODE_ERROR;
                }
            }
            else 
            {
                result = RESULT_CODE_ERROR;
            }
						#endif
            break;
        }

        case EXTENDED_ACTION:  /* AT@XXX  */
        {
            result = RESULT_ERR;
            break;
        }    

        default:
        {
            result = RESULT_ERR;
            break;
        }
    }

    return (result);
}
