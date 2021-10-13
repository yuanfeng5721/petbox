/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "custom_log.h"

int g_current_dbg_level = LOG_INFO;

#ifdef LOG_USED_CMD_UART
extern void cmd_uart_print(const char *fmt, ...);
#endif

void custom_printf(const char *fmt, ...)
{
	va_list list;
	char buf[256]={0};
	
	va_start(list, fmt);
	vsnprintf(buf,sizeof(buf), fmt, list);
	va_end(list);
#ifdef LOG_USED_CMD_UART
	cmd_uart_print("%s", buf);
#else
    DBG_DIRECT("%s", buf);
#endif
}
