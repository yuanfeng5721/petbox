/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <string.h>
#include "os_mem.h"
#include "os_sched.h"
#include "cmd_task.h"
#include "os_task.h"
#include "custom_log.h"

#include "rtl876x_hal_bsp.h"
#include "network_interface.h"
#include "gnss.h"

uint8_t network_status = 0;
/** @defgroup  BT5_CENTRAL_DEMO_MAIN BT5 Central Main
    * @brief Main file to initialize hardware and BT stack and start task scheduling
    * @{
    */
void *app_network_task_handle;   //!< APP Task handle

void network_test(void)
{
#define TEST_NETWORK 1
#if TEST_NETWORK
	int socket_fd;
	sockaddr_t addr;
	char *domain = "www.163.com";
	
	addr.port = 6522;
	addr.type = NETWORK_TCP;
	
	if(!network_prase_domain(domain, &addr))
		socket_fd = network_connect(&addr);
#endif
}

/**
 * @brief        network test task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
void network_task(void *p_param)
{
    uint8_t event;
	
	//gnss init
	gnss_init();
	
	// init at device and network
	network_status = (!network_init())?1:0;	
	
	//network_test();
	
    while (true)
    {
		os_delay(1000);
		led_ctl_tab[network_status](1);
		os_delay(100);
		led_ctl_tab[network_status](0);
    }
}
/**
 * @brief  Initialize App task
 * @return void
 */
void network_task_init(void)
{
    os_task_create(&app_network_task_handle, "network_task", network_task, 0, 256 * 7, 1);
}