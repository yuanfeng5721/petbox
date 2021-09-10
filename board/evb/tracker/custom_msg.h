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
    CUSTOM_MSG_TYPE_LED,  
    CUSTOM_MSG_TYPE_MODEM,    
    CUSTOM_MSG_TYPE_GNSS,    
    CUSTOM_MSG_TYPE_POWER,
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

/** @} */ /* End of group APP_MSG_Exported_Types */

/** @} */ /* End of group APP_MSG */

#ifdef __cplusplus
}
#endif

#endif /* _CUSTOM_MSG_H_ */