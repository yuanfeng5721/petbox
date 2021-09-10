#ifndef _GNSS_INTERFACE_H_
#define _GNSS_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "qcloud_iot_import.h"

typedef struct{
	int hour;
    int minute;
    int sec;
}gnss_time;
typedef struct{
	int mode;
	float lat;
	float lon;
	float accuracy;
	float altitude;
	float alt_sea_level;
	float speed;
	float course;
	gnss_time time;
} gnss_report_t;

typedef enum{
	AT_GNSS_EVT_GET_FIX = 0,
	AT_GNSS_EVT_GET_TIME,
	AT_GNSS_EVT_GET_EXTRAS,
	AT_GNSS_EVT_MAX,
} at_gnss_event_t;

typedef void (*at_gnss_evt_cb_t)(at_gnss_event_t event, gnss_report_t *report);

void at_device_gnss_init(at_gnss_event_t event, at_gnss_evt_cb_t cb);

#ifdef __cplusplus
}
#endif
#endif /* _GNSS_INTERFACE_H_ */

