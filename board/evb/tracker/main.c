/**
*****************************************************************************************
*     Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      main.c
   * @brief     Source file for BLE BT5 central project, mainly used for initialize modules
   * @author    berni
   * @date      2018-04-27
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2018 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <string.h>
#include "os_mem.h"
#include "os_sched.h"
#include "gap.h"
#include "gap_bond_le.h"
#include "gap_msg.h"
#include "gap_config.h"
#include "gap_ext_scan.h"
#include "cmd_task.h"
#include "bt5_central_app.h"
#include "link_mgr.h"
#include "otp_config.h"
#include "os_task.h"
#include "custom_log.h"
#include "rtc.h"
#include "sensor.h"

#include "rtl876x_hal_bsp.h"
#include "network_interface.h"
#include "gnss.h"
#include "nv.h"
#include "btlink_protocol_util.h"
#include "btlink_nv.h"

uint8_t network_status = 0;
/** @defgroup  BT5_CENTRAL_DEMO_MAIN BT5 Central Main
    * @brief Main file to initialize hardware and BT stack and start task scheduling
    * @{
    */
void *app_network_task_handle;   //!< APP Task handle
void *app_main_task_handle;
/*============================================================================*
 *                              Functions
 *============================================================================*/
/**
  * @brief  Config bt stack related feature
  * @return void
  */
#ifdef BT_STACK_CONFIG_ENABLE
#include "app_section.h"

APP_FLASH_TEXT_SECTION void bt_stack_config_init(void)
{
    gap_config_ae_parameter();
}
#endif

/**
  * @brief  Initialize central, gap bond manager and LE Advertising Extensions
            related parameters
  * @return void
  */
#if 0
void app_le_gap_init(void)
{
    /* Device name and device appearance */
    uint8_t  device_name[GAP_DEVICE_NAME_LEN] = "BLE_BT5_Central";
    uint16_t appearance = GAP_GATT_APPEARANCE_UNKNOWN;

    uint16_t max_data_len_tx_oct = 251;
#if LE_CODED_PHY_S8
    uint16_t max_data_len_tx_time = 17040;
#else
    uint16_t max_data_len_tx_time = 4542;
#endif

    /* GAP Bond Manager parameters */
    uint8_t  auth_pair_mode = GAP_PAIRING_MODE_PAIRABLE;
    uint16_t auth_flags = GAP_AUTHEN_BIT_BONDING_FLAG | GAP_AUTHEN_BIT_SC_FLAG;
    T_GAP_IO_CAP  auth_io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t  auth_oob = false;
    uint8_t  auth_use_fix_passkey = false;
    uint32_t auth_fix_passkey = 0;
    uint8_t  auth_sec_req_enable = false;
    uint16_t auth_sec_req_flags = GAP_AUTHEN_BIT_BONDING_FLAG | GAP_AUTHEN_BIT_SC_FLAG;

    /* LE Advertising Extensions parameters */
    bool use_extended = true;

    /* Allocate memory for recombining advertising data */
#if APP_RECOMBINE_ADV_DATA
    ext_adv_data = os_mem_zalloc(RAM_TYPE_DATA_ON, sizeof(T_EXT_ADV_DATA));
    ext_adv_data->flag = false;
#endif

    le_set_gap_param(GAP_PARAM_DEFAULT_DATA_LEN_MAX_TX_OCTETS, sizeof(max_data_len_tx_oct),
                     &max_data_len_tx_oct);
    le_set_gap_param(GAP_PARAM_DEFAULT_DATA_LEN_MAX_TX_TIME, sizeof(max_data_len_tx_time),
                     &max_data_len_tx_time);

    /* Set device name and device appearance */
    le_set_gap_param(GAP_PARAM_DEVICE_NAME, GAP_DEVICE_NAME_LEN, device_name);
    le_set_gap_param(GAP_PARAM_APPEARANCE, sizeof(appearance), &appearance);

    /* Setup the GAP Bond Manager */
    gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(auth_pair_mode), &auth_pair_mode);
    gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(auth_flags), &auth_flags);
    gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(auth_io_cap), &auth_io_cap);
    gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(auth_oob), &auth_oob);
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY, sizeof(auth_fix_passkey), &auth_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE, sizeof(auth_use_fix_passkey),
                      &auth_use_fix_passkey);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enable), &auth_sec_req_enable);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags),
                      &auth_sec_req_flags);

    /* Use LE Advertising Extensions */
    le_set_gap_param(GAP_PARAM_USE_EXTENDED_ADV, sizeof(use_extended), &use_extended);

    /* Register gap message callback */
    le_register_app_cb(app_gap_callback);
}
#endif
/**
 * @brief    Contains the initialization of pinmux settings and pad settings
 * @note     All the pinmux settings and pad settings shall be initiated in this function,
 *           but if legacy driver is used, the initialization of pinmux setting and pad setting
 *           should be peformed with the IO initializing.
 * @return   void
 */
void board_init(void)
{
	board_gpio_init();
	leds_init();
	sys_3v8_power(1);
}



void network_test()
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
 * @brief    Contains the initialization of peripherals
 * @note     Both new architecture driver and legacy driver initialization method can be used
 * @return   void
 */
void driver_init(void)
{
	//RTC init
	rtc_init();
	
	//Gsensor init
	sensor_init();
	
	//gnss init
	gnss_init();
	
	// init at device and network
	network_status = (!network_init())?1:0;	
}

/**
 * @brief    Contains the power mode settings
 * @return   void
 */
void pwr_mgr_init(void)
{
}

/**
 * @brief        App test task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
void network_task(void *p_param)
{
    uint8_t event;
	
	driver_init();
	network_test();
	
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
void network_task_init()
{
    os_task_create(&app_network_task_handle, "network_task", network_task, 0, 256 * 7, 1);
}
#pragma pack (1)
typedef struct{
	uint8_t  data8;
	uint32_t data32;
	uint16_t data16;
}st_nv_test;
#pragma pack ()
  
void test_nv(void)
{
	st_nv_test nv_test = {
		0x99,
		0x5a5a5a5a,
		0x1234
	};
	st_nv_test nv_test_r;
	
	nv_item_write_string("SOFTWART_VERSION", "V1.0.2");
	LOG_I("softwart version: %s ",nv_item_read_string("SOFTWART_VERSION"));
	
	nv_item_write("struct_nv", (uint8_t *)&nv_test, sizeof(nv_test));
	nv_item_read("struct_nv", (uint8_t *)&nv_test_r, sizeof(nv_test_r));
	LOG_I("struct_nv:data32 = %d, data16 = %d, data8 = %d", nv_test_r.data32, nv_test_r.data16, nv_test_r.data8);
	
}
void imei_nv_test(void)
{
	char *imei;
	if(!nv_item_read_string("IMEI"))
	{
		imei = at_device_get_imei();
		if(imei)
		{
			nv_item_write_string("IMEI", imei);
			LOG_I("device imei: %s ",nv_item_read_string("IMEI"));
		}
	}
}
/**
 * @brief        App test task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
void app_main_task(void *p_param)
{
	//init base module
	//nv module initialize
	nv_item_init();
	
	test_nv();
	
	btlink_pro_nv_init();
	
	//
    while (true)
    {
		//led ctl code
		os_delay(5000);
		imei_nv_test();
    }
}
void main_task_init(void)
{
	os_task_create(&app_main_task_handle, "app_main", app_main_task, 0, 256 * 5, 1);
}
/**
 * @brief    Contains the initialization of all tasks
 * @note     There is only one task in BT5 BLE Central APP, thus only one APP task is init here
 * @return   void
 */
void task_init(void)
{
	cmd_task_init();
	main_task_init();
	network_task_init();
}

/**
 * @brief    Entry of APP code
 * @return   int (To avoid compile warning)
 */
int main(void)
{
    board_init();
#if(TEST_DRIVER == 0)
//    le_gap_init(APP_MAX_LINKS);
//    gap_lib_init();
//    app_le_gap_init();
#endif
    pwr_mgr_init();
    btlink_util_protocol_init();
    task_init();
    os_sched_start();

    return 0;
}
/** @} */ /* End of group BT5_CENTRAL_DEMO_MAIN */


