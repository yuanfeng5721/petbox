/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-01-16
 */

#include <easyflash.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flash_device.h"
#include "os_sync.h"
#include "trace.h"
#include "cmd_uart.h"
/* default environment variables set for user */
//static const ef_env default_env_set[] = {

//};
extern const ef_env default_env_set[EF_DEFAULT_ENV_ITEM];

void *p_flash_mutex = NULL;
/**
 * Flash port for hardware initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV size
 *
 * @return result
 */
EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size) {
    EfErrCode result = EF_NO_ERR;

    *default_env = default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);
	
	if (!os_mutex_create(&p_flash_mutex))
    {
        result = EF_ENV_INIT_FAILED;
    }
    return result;
}

/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;

    //EF_ASSERT(size % 4 == 0);

    /* You can add your code under here. */
	//EF_ASSERT(addr % 4 == 0);
	if(!flash_read_locked(addr, size, (uint8_t *)buf))
		result = EF_READ_ERR;
    return result;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size) {
    EfErrCode result = EF_NO_ERR;
	uint32_t erase_count = 0, i;

    /* make sure the start address is a multiple of EF_ERASE_MIN_SIZE */
    EF_ASSERT(addr % EF_ERASE_MIN_SIZE == 0);

    /* You can add your code under here. */
	erase_count = size/EF_ERASE_MIN_SIZE;
	if(size%EF_ERASE_MIN_SIZE != 0)
		erase_count++;
	for(i=0; i<erase_count; i++)
	{
		if(!flash_erase_locked(FLASH_ERASE_SECTOR, addr+EF_ERASE_MIN_SIZE*i))
		{
			result = EF_ERASE_ERR;
			break;
		}
	}
    return result;
}
/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;

    EF_ASSERT(size % 4 == 0);
    
    /* You can add your code under here. */
	//EF_ASSERT(addr % 4 == 0);
	if(!flash_write_locked(addr, size, (uint8_t *)buf))
	{
		result = EF_WRITE_ERR;
	}
    return result;
}

/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void) {
    
    /* You can add your code under here. */
    os_mutex_take(p_flash_mutex, 0xFFFFFFFF);
}

/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void) {
    
    /* You can add your code under here. */
    os_mutex_give(p_flash_mutex);
}


/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...) {

#define filename(x) strrchr(x,'\\')?strrchr(x,'\\')+1:x
#ifdef PRINT_DEBUG
#ifdef MY_PRINT_DEBUG
	va_list list;
	char buf[256]={0};
	
	va_start(list, format);
	vsnprintf(buf,sizeof(buf), format, list);
	va_end(list);
#ifdef DEBUG_USE_CMD_UART
	cmd_uart_print("[Flash](%s:%ld):%s", filename(file), line, buf);
#else
    DBG_DIRECT("[Flash](%s:%ld):%s", filename(file), line, buf);
#endif
#else
	va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
    
    va_end(args);
#endif
#endif
}

/**
 * This function is print flash routine info.
 *
 * @param format output format
 * @param ... args
 */
void ef_log_info(const char *format, ...) {
    va_list args;
	char log_buf[128];
    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
	vsprintf(log_buf, format, args);
#ifdef DEBUG_USE_CMD_UART
	cmd_uart_print(log_buf);
#else
    DBG_DIRECT(log_buf);
#endif
    va_end(args);
}
/**
 * This function is print flash non-package info.
 *
 * @param format output format
 * @param ... args
 */
void ef_print(const char *format, ...) {
    va_list args;
	char log_buf[128];
    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
	vsprintf(log_buf, format, args);
#ifdef DEBUG_USE_CMD_UART
	cmd_uart_print(log_buf);
#else
    DBG_DIRECT(log_buf);
#endif
    va_end(args);
}
