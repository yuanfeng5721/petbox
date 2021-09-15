/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <string.h>
#include "os_mem.h"
#include "os_sched.h"
#include "os_task.h"
#include "os_timer.h"

#include "rtl876x_hal_bsp.h"
#include "leds_blink.h"
#include "custom_log.h"
  
void *leds_task_handle;
void *leds_blink_timer_handle;

// Timer callback function.
void leds_timer_callback(void *p_handle)
{
    uint32_t timer_id;

    // Which timer expired?
    os_timer_id_get(&p_handle, &timer_id);

    if (timer_id == LEDS_TIMER_ID)
    {
         //do something
    }
}


int leds_blink_timer(void)
{
    if (os_timer_create(&leds_blink_timer_handle, "leds_blink", LEDS_TIMER_ID,
                       LEDS_BLINK_PERIOD, false, leds_timer_callback) == true)
    {
        // Timer created successfully, start the timer.
		return 0;
    }
    else
    {
        // Timer failed to create.
        return -1;
    }

}
/**
 * @brief        Leds task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
void leds_task(void *p_param)
{
	LedEvent event = BLUE_LED_OFF|GREEN_LED_OFF;
    while (true)
    {
		//led ctl code
		os_delay(5000);
		
		
    }
}
void leds_task_init(void)
{
	os_task_create(&leds_task_handle, "leds_ctl", leds_task, 0, 256 * 1, 1);
	if(leds_blink_timer() < 0) {
		LOG_E("leds blink timer create error!!");
	}
}
