#include <stdio.h>

#include "board.h"
#include "gnss_interface.h"
#include "custom_log.h"


static void gnss_report_cb(at_gnss_event_t event, gnss_report_t *report)
{
	//do some thing to deal gnss report
	if(report) {
		LOG_I("GNSS: (lat:%f,lon:%f)(time:%d:%d:%d) \r\n", report->lat, report->lon, 
					report->time.hour, report->time.minute, report->time.sec);
	}
}

void gnss_init(void)
{
	at_device_gnss_init(AT_GNSS_EVT_GET_FIX, gnss_report_cb);	
}

void gnss_enable(void)
{
	at_device_gnss_set(true);
}

void gnss_disable(void)
{
	at_device_gnss_set(false);
}
