/*********************************************************************************************************
*/
/*============================================================================*
  *                     Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _CUSTOM_LOG_H_
#define _CUSTOM_LOG_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "trace.h"
#ifdef __cplusplus
extern "C" {
#endif

#define LOG_OFF             0 // close all log
#define LOG_ERROR           1
#define LOG_WARN            2
#define LOG_DEBUG           3
#define LOG_INFO            4

#define LOG_USED_CMD_UART

extern int g_current_dbg_level;
void custom_printf(const char *fmt, ...);

#ifndef LOG_TAG
#define LOG_TAG "LOG"
#endif

#define LOG(level, format, ...) \
		{\
			if(level<=g_current_dbg_level)\
			{\
				custom_printf("%s: " format, LOG_TAG, ##__VA_ARGS__);\
			}\
		}\

#define LOG_E(format, ...) LOG(LOG_ERROR, format, ##__VA_ARGS__);
#define LOG_W(format, ...) LOG(LOG_WARN,  format, ##__VA_ARGS__);
#define LOG_D(format, ...) LOG(LOG_DEBUG, format, ##__VA_ARGS__);
#define LOG_I(format, ...) LOG(LOG_INFO,  format, ##__VA_ARGS__);

		
#ifdef __cplusplus
}
#endif

#endif /* _CUSTOM_LOG_H_ */
