/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "custom_log.h"

int g_current_dbg_level = LOG_INFO;

void custom_printf(const char *fmt, ...)
{
	va_list list;
	char buf[256]={0};
	
	va_start(list, fmt);
	vsnprintf(buf,sizeof(buf), fmt, list);
	va_end(list);

    DBG_DIRECT("%s", buf);
}
