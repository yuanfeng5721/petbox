/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <string.h>
#include <time.h>
#include "rtl876x_nvic.h"
#include "rtl876x_rcc.h"
#include "rtl876x_rtc.h"
#include "custom_log.h"


time_t time (time_t *_timer)
{
    struct tm ts;
	time_t time;
	
	//need get local time
	
	time = mktime(&ts);
	
	if(_timer != NULL)
    {
		*_timer = time;
    }
    return time;
}