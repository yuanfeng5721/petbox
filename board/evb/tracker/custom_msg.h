/*********************************************************************************************************
*/
/*============================================================================*
  *                     Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _CUSTOM_MSG_H_
#define _CUSTOM_MSG_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup APP_MSG APP Message
  * @brief message definition for user application task
  * @{
  */

/*============================================================================*
  *                                   Types
  *============================================================================*/
/** @defgroup APP_MSG_Exported_Types APP Message Exported Types
  * @{
    */

/**  @brief CUSTOM type definitions for IO message, may extend as requested */
typedef enum
{
    CUSTOM_MSG_TYPE_LED = 0x01,  
    CUSTOM_MSG_TYPE_MODEM = 0x02,    
    CUSTOM_MSG_TYPE_GNSS = 0x03,    
    CUSTOM_MSG_TYPE_POWER = 0x04,
	CUSTOM_MSG_TYPE_REPORT = 0x05,
} T_CUSTOM_MSG_TYPE;

/**  @brief CUSTOM subtype definitions for @ref T_CUSTOM_MSG_LED type */
typedef enum
{
    CUSTOM_MSG_LED_BLUE_OFF,
    CUSTOM_MSG_LED_BLUE_ON,
    CUSTOM_MSG_LED_BLUE_BLINK,
    CUSTOM_MSG_LED_GREEN_OFF,
    CUSTOM_MSG_LED_GREEN_ON,
    CUSTOM_MSG_LED_GREEN_BLINK,
} T_CUSTOM_MSG_LED;

/**  @brief CUSTOM subtype definitions for @ref T_CUSTOM_MSG_MODEM type */
typedef enum
{
    CUSTOM_MSG_MODED_INITING,
    CUSTOM_MSG_MODED_SUCCESS,
	CUSTOM_MSG_MODED_INIT_ERROR,
} T_CUSTOM_MSG_MODEM;

typedef enum
{
    CUSTOM_MSG_GNSS_FIX,
    CUSTOM_MSG_GNSS_TIME,
	CUSTOM_MSG_GNSS_EXTRAS,
} T_CUSTOM_MSG_GNSS;

typedef enum
{
    CUSTOM_MSG_REPORT_BATTERY_DATA,
    CUSTOM_MSG_REPORT_GPS_DATA,
    CUSTOM_MSG_REPORT_NETWORK_DATA,
    CUSTOM_MSG_REPORT_OTHER,
} T_CUSTOM_MSG_REPORT;

/**  @brief custom message definition for communications between tasks*/
typedef struct
{
    uint16_t type;
    uint16_t subtype;
    union
    {
        uint32_t  param;
        void     *buf;
    } messag;
	uint16_t msg_size;
} T_CUSTOM_MSG;

#define CUSTOM_MSG_QUEUE_MAX_NUM   10
/** @} */ /* End of group APP_MSG_Exported_Types */

/** @} */ /* End of group APP_MSG */


typedef enum {
	CUSTOM_TIMER_BATTERY = 0x10,
	CUSTOM_TIMER_ALARM = 0x20,
	CUSTOM_TIMER_ID_END = 0x30,
} T_CUSTOM_TIMER_ID;

#ifdef __cplusplus
}
#endif

#endif /* _CUSTOM_MSG_H_ */
