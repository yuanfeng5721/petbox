/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <string.h>
#include "os_mem.h"
#include "os_sched.h"
#include "cmd_task.h"
#include "os_task.h"
#include "os_msg.h"
#include "custom_log.h"
#include "custom_msg.h"

#include "rtl876x_hal_bsp.h"
#include "network_interface.h"
#include "gnss.h"

void *report_task_handle;   //!< APP Task handle
void *report_queue_handle;   //!< report queue handle

int8_t report_collect()
{
	
}

void report_handle_msg(T_CUSTOM_MSG cus_msg)
{
    uint16_t sub_event = cus_msg.subtype;

    switch (sub_event)
    {
		case CUSTOM_MSG_REPORT_BATTERY_DATA:
			
			break;
		
		case CUSTOM_MSG_REPORT_GPS_DATA:
			
			break;
		
		case CUSTOM_MSG_REPORT_NETWORK_DATA:
			
			break;
		
		case CUSTOM_MSG_REPORT_OTHER:
			
			break;
		
		default:
			break;
    }
}

bool check_report_is_ok()
{
	return true;
}

void report_task(void *p_param)
{
    uint8_t event;
	os_msg_queue_create(&report_queue_handle, CUSTOM_MSG_QUEUE_MAX_NUM, sizeof(T_CUSTOM_MSG));
	
    while (true)
    {
		T_CUSTOM_MSG cus_msg;
		if (os_msg_recv(report_queue_handle, &cus_msg, 0) == true) {
			if(cus_msg.type == CUSTOM_MSG_TYPE_REPORT) {
				report_handle_msg(cus_msg);
				if(check_report_is_ok()) {
					//repor data ok, send report
					//do send data
				}
			}
		}
    }
}

void report_task_init(void)
{
	os_task_create(&report_task_handle, "report_task", report_task, 0, 256 * 5, 1);
}

