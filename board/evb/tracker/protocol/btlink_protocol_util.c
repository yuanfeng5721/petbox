/*
******************************************************************************
*
*     Copyright (c) 2021 btlink Ltd.
*
*******************************************************************************
*  file name:           btlink_protocol_util.c
*  author:              Eric
*  version:             1.00
*  file description:    AVL GPS tracker protocol utilities function
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
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "btlink_trace.h"
#include "btlink_protocol_enums.h"
#include "btlink_protocol_def.h"
#include "btlink_protocol_util.h"
#include "btlink_protocol_cmd.h"
//#include "can_message_assemble_enum.h"
#include "stdio.h"
/*****************************************************************************
*  Variable define
*****************************************************************************/
//report count
static unsigned int g_send_cnt = 0;
#define INT_MAX ((int)0x7FFFFFFF)
 
/*****************************************************************************
* Extern Variable
*****************************************************************************/

/*****************************************************************************
* Extern Function
*****************************************************************************/
extern void btlink_set_pro_verno(void);
extern void btlink_set_imei(void);

/******************************************************************************
* Process
******************************************************************************/
unsigned int btlink_get_report_no(void)
{
    return (g_send_cnt);
}

void btlink_set_report_no(unsigned int cnt)
{
    g_send_cnt = cnt;
}

/******************************************************************************
* Function    : btlink_get_report_count
*
* Author      : Eric
*
* Parameters  :
*
* Return      :
*
* Description :
******************************************************************************/
unsigned int btlink_get_report_count(void)
{
    unsigned int report_count = g_send_cnt;
    g_send_cnt++;
    if (g_send_cnt > 0xffff)
    {
        g_send_cnt = 0;
    }

    return report_count;
}

/*****************************************************************************
 * Function    : btlink_utils_isalnum_buffer
 *
 * Author      : Eric
 *
 * Parameters  : Test whether  a buffer caintains only digits and alpha
 *
 * Return      :
 *
 * Description :

 *****************************************************************************/
bool btlink_utils_isalnum_buffer(uint8_t *buffer, uint8_t length)
{
    uint8_t i;
    bool ret = true;

    for (i = 0; i < length; i++)
    {
        //End of buffer
        if (buffer[i] == '\0')
        {
            if (i == 0) ret = false;
            break;
        }

        if (!isalnum(buffer[i]))
        {
            ret = false;
            break;
        }
    }

    return ret;
}

// Added for supporting '-' and '_' in the device name string, by Gavin on 2012.03.19 ...... start
bool btlink_utils_isalnum_and_connector_buffer(uint8_t *buffer, uint8_t length)
{
    uint8_t i;
    bool ret = true;

    for (i = 0; i < length; i++)
    {
        //End of buffer
        if (buffer[i] == '\0')
        {
            if (i == 0) ret = false;
            break;
        }

        if  ((buffer[i] == '-') || (buffer[i] == '_'))
            continue;

        if (!isalnum(buffer[i]))
        {
            ret = false;
            break;
        }
    }

    return ret;
}
// Added for supporting '-' and '_' in the device name string, by Gavin on 2012.03.19 ...... over

/******************************************************************************
* Function    : btlink_btlink_utils_isdigit_buffer
*
* Author      : Eric
*
* Parameters  :
*
* Return      :
*
* Description :
******************************************************************************/
bool btlink_utils_isdigit_buffer(uint8_t *buffer, uint8_t length)
{
    uint8_t i;
    bool ret = true;

    for (i = 0; i < length; i++)
    {
        //End of buffer
        if (buffer[i] == '\0')
        {
            if (i == 0) ret = false;
            break;
        }

		#if 0
        if (!isdigit(buffer[i]))
        {
            ret = FALSE;
            break;
        }
		#else
        if (('0' <= buffer[i]) && (buffer[i] <= '9'))
		{
		}
		else
		{
			ret = false;
			break;
		}
		#endif
    }

    return ret;
}

/******************************************************************************
* Function    : btlink_btlink_utils_isxdigit_buffer
*
* Author      : Eric
*
* Parameters  :
*
* Return      :
*
* Description :
******************************************************************************/

bool btlink_utils_isxdigit_buffer(uint8_t *buffer, uint8_t length)
{
    uint8_t i;
    bool ret = true;

    for (i = 0; i < length; i++)
    {
        //End of buffer
        if (buffer[i] == '\0')
        {
            if (i == 0) ret = false;
            break;
        }

        if (!isxdigit(buffer[i]))
        {
            ret = false;
            break;
        }
    }

    return ret;
}

/******************************************************************************
* Function    : btlink_utils_hasdigit_buffer
*
* Author      : Eric
*
* Parameters  :
*
* Return      :
*
* Description :
******************************************************************************/
bool btlink_utils_hasdigit_buffer(uint8_t *buffer, uint8_t length)
{
    uint8_t i;
    bool ret = false;

    for (i = 0; i < length; i++)
    {
        //End of buffer
        if (buffer[i] == '\0')
        {
            if (i == 0) ret = false;
            break;
        }

        if (isdigit(buffer[i]))
        {
            ret = true;
            break;
        }
    }

    return ret;
}
/*****************************************************************************
 * FUNCTION
 *  gprt_ucs2_n_to_ascii
 *
 * DESCRIPTION
 *  Convert N character UCS2 encoding string to ASCII.  The byte order of UCS2
 *  character(input param) is big endian(raw data from l4 SMS data).
 *
 * PARAMETERS
 *  out_asc     [in]  Destination ASCII string
 *  in_ucs2     [in]  UCS2 source string
 *  len         [in]  Totoal UCS2 character to convert
 *
 * RETURNS
 *  Bytes that converted
 *
 * GLOBALS AFFECTED
 *
 *****************************************************************************/
uint16_t gprt_ucs2_n_to_ascii(uint8_t * out_asc, uint8_t * in_ucs2, uint16_t len)
{
    uint16_t count = 0;

    while ((len) && (!((*in_ucs2 == 0) && (*(in_ucs2 + 1) == 0))))
    {
        *out_asc = *(in_ucs2 + 1);

        in_ucs2 += 2;
        out_asc++;
        count++;
        len -= 2;
    }

    *out_asc = '\0';

    return count;
}

/******************************************************************************
* Function    : btlink_util_char_to_int
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
uint8_t btlink_util_char_to_int(uint8_t character)
{
	uint8_t i = 0;

	if (character >= '0' && character <= '9')
	{
		i = character - '0';
	}
	else if (character >= 'A' && character <= 'F')
	{
		i = character - 'A' + 10;
	}
	else if (character >= 'a' && character <= 'f')
	{
		i = character - 'a' + 10;
	}

	return i;
}

/******************************************************************************
* Function    : btlink_utils_convert_hex_to_ascii
*
* Author      : Eric
*
* Parameters  :
*
* Return      :
*
* Description :
******************************************************************************/
void btlink_utils_convert_hex_to_ascii(uint8_t *data)
{
    int i, length = 0;
    uint8_t tmp_data[128];
    uint8_t tmp_low = 0;
    uint8_t tmp_high = 0;

    memset(tmp_data, 0, sizeof(tmp_data));
    strcpy((char *)tmp_data, (char *)data);

    length = strlen((char *)tmp_data);

    for (i = 0; i < length; i++)
    {
        tmp_high = ((tmp_data[i] & 0xF0) >> 4);
        tmp_low = tmp_data[i] & 0x0F;

        if (tmp_high <= 9)
        {
            data[i * 2] = tmp_high + '0';
        }
        else if (tmp_high >= 0x0A && tmp_high <= 0x0F)
        {
            data[i * 2] = (tmp_high - 10) + 'A';
        }

        if (tmp_low <= 9)
        {
            data[i * 2 + 1] = tmp_low + '0';
        }
        else if (tmp_low >= 0x0A && tmp_low <= 0x0F)
        {
            data[i * 2 + 1] = (tmp_low - 10) + 'A';
        }

    }
}


/*****************************************************************************
 * FUNCTION
 *  app_strnicmp
 * DESCRIPTION
 *  Case-insensitive string comparison for up to n characters.
 * PARAMETERS
 *  s            [IN]
 *  t            [IN]
 *  n            [IN]    number of compared characters
 * RETURNS
 *  return <0 if s<t, 0 if s==t, or >0 if s>t.
 *****************************************************************************/
int32_t btlink_utils_app_strnicmp(char *s, char *t, int n)
{
    if (n < 0)
        return n;

    for ( ; --n >= 0 && tolower(*s) == tolower(*t); s++, t++)
        if (*s == '\0')
            return 0;

    if (n < 0)
        return 0;
    else
        return tolower(*s) - tolower(*t);
}


//app datetime
uint8_t btlink_applib_dt_dow(uint16_t y, uint8_t m, uint8_t d)
{
    if (m < 3)
    {
        m += 13;
        y--;
    }
    else
    {
        m++;
    }

    return (d + 26 * m / 10 + y + y / 4 - y / 100 + y / 400 + 6) % 7;
}

/*****************************************************************************
 * FUNCTION
 *  gprt_hex_string_2_int
 *
 * DESCRIPTION
 *  Convert hex format string to int32_t(int)
 *
 * PARAMETERS
 *  str     [in]  source string
 *
 * RETURNS
 *  int32_t value that converted
 *
 * GLOBALS AFFECTED
*****************************************************************************/
int32_t btlink_hex_string_2_int(char *str)
{
    int32_t value = 0;
    int32_t sign  = 1;
    int32_t radix = 16;

    if (*str == '-')
    {
        sign = -1;
        str++;
    }

    if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X'))
    {
        str += 2;
    }

    while (*str == '0')
    {
        str++;
    }

    while (*str)
    {
        if (*str >= '0' && *str <= '9')
            value = value * radix + *str - '0';
        else
            value = value * radix + (*str | 0x20) - 'a' + 10;

        str++;
    }

    return  sign * value;
}

/******************************************************************************
* Function    : btlink_utils_atoi_buffer
*
* Author      : Eric
*
* Parameters  :
*
* Return      :
*
* Description :
******************************************************************************/
int btlink_utils_atoi_buffer(const char * str)
{
	bool minus = false;
	long long result = 0;
	//bool valid = false;
	
	if (str == NULL) return 0;
	
	while(*str == ' ') str++;
	
	if (*str == '-')
	{
		minus = true;
		str++;
	}
	else if (*str == '+')
	{
		str++;
	}
	
	if (*str<'0' || *str>'9') return 0;

	//valid = true;
	
	while(*str>='0' && *str<='9')
	{
		result = result*10+*str-'0';
		if((minus && result>INT_MAX + 1LL) || (!minus && result>INT_MAX))
		{
			//valid = false;
			return 0;
		}

		str++;
	}

	if (minus) result*=-1;
	
	return (int)result;
}

/******************************************************************************
* Function    : btlink_assemble_dat_cmd_string
*
* Author      : Eric
*
* Parameters  :
*
* Return      :
*
* Description :
******************************************************************************/
uint16_t btlink_assemble_dat_cmd_string(uint8_t *data, uint8_t *dat_cmd)
{
	uint16_t len = 0;
	
	#if 0
	len = sprintf((char *)dat_cmd, 
										"%s%s%s%c%d%c%c" /*AT+GTDAT Head*/
										"%s"           /* Data */
										"%s%s",        /*AT+GTDAT Tail*/
										CRLF,
										BTLINK_DAT_RSP_HEADER, g_btlink_config.cfg_glob.Password,
										COMMA_CHAR, g_btlink_config.cfg_glob.dat_cmd_mode,
										COMMA_CHAR,
										COMMA_CHAR,
										data,
										",,,,,0001$", CRLF);
	#endif
	return len;
}

bool btlink_soc_ip_check(char *asci_addr, uint8_t *ip_addr, bool *ip_validity)
{
    uint8_t len;
    uint8_t i,octet_cnt;
    int32_t ip_digit;

    len = strlen(asci_addr);

    for (i = 0 ; i < len ; i++)
    {
        if (!isdigit((int)*(asci_addr+i)) && *(asci_addr+i) != '.' )
            return false;
    }

    *ip_validity = true;

    /* Extract the IP adress from character array */
    for (octet_cnt = 0 ; octet_cnt < 4 ; octet_cnt++)
    {
        if (*asci_addr == '\0') /* in case of "1.2.3." */
        {
            *ip_validity = false;
            return false;
        }

        ip_digit = atoi(asci_addr);
				

        if (ip_digit < 0 || ip_digit > 255)
        {
            *ip_validity = false;
            return false;
        }

        ip_addr[octet_cnt] = (uint8_t)ip_digit;

        if (octet_cnt == 3)
            continue;

        asci_addr = strstr(asci_addr,".");
        if (asci_addr)
            asci_addr++;
        else
            break;
    }

    if (octet_cnt != 4)
    {
        return false;
    }

    if (ip_addr[0] == 0 && ip_addr[1] == 0 && ip_addr[2] == 0 && ip_addr[3] == 0)
    {
        *ip_validity = false;
    }

    return true;
}


/******************************************************************************
* Function    : btlink_util_ip_all_zero
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
bool btlink_util_ip_all_zero(uint8_t* ip)
{
    return (bool)(0==strncmp("0.0.0.0", (char *)ip, 7));
}

/******************************************************************************
* Function    : btlink_util_password_match
* 
* Author      : eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
bool btlink_util_password_match(uint8_t *password)
{
    return (bool)(strncmp((char*)g_btlink_config.cfg_scs.data_zone_mask, 
                              (char*)password, BTLINK_LEN_SCS_PASSWORD) == 0 
             ||strncmp((char*)SUPER_PASSWORD, (char*)password, BTLINK_LEN_SCS_PASSWORD) == 0);
}

/*****************************************************************************
 * FUNCTION
 *  btlink_util_isalnum_buffer
 *
 * DESCRIPTION
 *  Test whether a buffer caintians only digits and alpha.
 *
 * PARAMETERS
 *  buffer    [in]    Buffer to be verify
 *  length    [in]    Max length of the buffer
 *
 * RETURNS
 *  kal_bool
 *
 * GLOBALS AFFECTED
 *  
 *****************************************************************************/
bool btlink_util_isalnum_buffer(uint8_t *buffer, uint8_t length)
{
    uint8_t i;
    bool ret = true;

    for (i = 0; i < length; i++) 
    {
        /* End of buffer */
        if (buffer[i] == '\0') 
        {
            if (i == 0) ret = false;
            break;
        }

        if (!isalnum(buffer[i])) 
        {
            ret = false;
            break;
        }
    }

    return ret;
}

/******************************************************************************
* Function    : btlink_util_is_letter
* 
* Author      : Eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : Judge whether the input char is a letter
******************************************************************************/
bool btlink_util_is_letter(const char ch)
{
	if (('a' <= ch && 'z' >= ch)
	|| ('A' <= ch && 'Z' >= ch))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
* Function    : btlink_util_is_same_str_ignore_lc
* 
* Author      : Eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : Judge whether the input two string is the same ignore lowercase
******************************************************************************/
bool btlink_util_is_same_str_ignore_lc(const char *str1, const char *str2)
{
	uint16_t i = 0;
	bool ret = true;

	if (str1 == NULL || str2 == NULL)
	{
		 return false;
	}
	
	if (strlen(str1) != strlen(str2))
	{
		ret = false;
	}
	else
	{
		uint16_t len = strlen(str1);

		for (i=0; i<len; i++)
		{
			if (btlink_util_is_letter(str1[i]) && btlink_util_is_letter(str2[i]))
			{
				if (toupper(str1[i]) != toupper(str2[i]))
				{
					ret = false;
					break;
				}
			}
			else
			{
				if (str1[i] != str2[i])
				{
					ret = false;
					break;
				}
			}
		}
	}

	return ret;
}

/******************************************************************************
* Function    : btlink_util_protocol_init
* 
* Author      : Eric
* 
* Parameters  : 
* 
* Return      : 
* 
* Description : 
******************************************************************************/
void btlink_util_protocol_init(void)
{
	btlink_set_pro_verno();
	btlink_set_imei();
}
