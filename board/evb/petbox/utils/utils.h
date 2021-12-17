/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     utils.h
* @brief    Data uart operations for testing multilink.
* @details  Data uart init and print data through data uart.
* @author   wangkun
* @date     2021-12-17
* @version  v0.1
*********************************************************************************************************
*/
#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */


//typedef struct Timer 
//{
//	uint32_t xTicksToWait;
//	TimeOut_t xTimeOut;
//} Timer;

/**
 * Define timer structure, platform dependant
 */
struct Timer {
#if defined(__linux__) && defined(__GLIBC__)
    struct timeval end_time;
#else
    uintptr_t end_time;
#endif
};

typedef struct Timer Timer;

/**
 * @brief Check if timer expires or not
 *
 * @param timer     reference to timer
 * @return          true = expired, false = not expired yet
 */
bool HAL_Timer_expired(Timer *timer);

/**
 * @brief Set the countdown/expired value for the timer
 *
 * @param timer         reference to timer
 * @param timeout_ms    countdown/expired value (unit: millisecond)
 */
void HAL_Timer_countdown_ms(Timer *timer, unsigned int timeout_ms);

/**
 * @brief Set the countdown/expired value for the timer
 *
 * @param timer         reference to timer
 * @param timeout       countdown/expired value (unit: second)
 */
void HAL_Timer_countdown(Timer *timer, unsigned int timeout);

/**
 * @brief Check the remain time of the timer
 *
 * @param timer     reference to timer
 * @return          0 if expired, or the left time in millisecond
 */
int HAL_Timer_remain(Timer *timer);

/**
 * @brief Init the timer
 *
 * @param timer reference to timer
 */
void HAL_Timer_init(Timer *timer);

/**
 * @brief Get local time in format: %y-%m-%d %H:%M:%S
 *
 * @return string of formatted time
 */
char *HAL_Timer_current(char *time_str);

/**
 * @brief Get timestamp in second
 *
 * @return   timestamp in second
 */
long HAL_Timer_current_sec(void);

/**
 * @brief Set timestamp in second to systime/rtc
 *
 * @return   0 is success other failed
 */
int HAL_Timer_set_systime_sec(size_t timestamp_sec);

/**
 * @brief Set timestamp in millsecond to systime/rtc
 *
 * @return   0 is success other failed
 */
int HAL_Timer_set_systime_ms(size_t timestamp_ms);


/**
 * @brief Check if a timer is expired
 *
 * Call this function passing in a timer to check if that timer has expired.
 *
 * @param timer - pointer to the timer to be checked for expiration
 * @return bool - true = timer expired, false = timer not expired
 */
bool expired(Timer *timer);

/**
 * @brief Create a timer (milliseconds)
 *
 * Sets the timer to expire in a specified number of milliseconds.
 *
 * @param timer - pointer to the timer to be set to expire in milliseconds
 * @param timeout_ms - set the timer to expire in this number of milliseconds
 */
void countdown_ms(Timer *timer, unsigned int timeout_ms);

/**
 * @brief Create a timer (seconds)
 *
 * Sets the timer to expire in a specified number of seconds.
 *
 * @param timer - pointer to the timer to be set to expire in seconds
 * @param timeout - set the timer to expire in this number of seconds
 */
void countdown(Timer *timer, unsigned int timeout);

/**
 * @brief Check the time remaining on a give timer
 *
 * Checks the input timer and returns the number of milliseconds remaining on the timer.
 *
 * @param timer - pointer to the timer to be set to checked
 * @return int - milliseconds left on the countdown timer
 */
int left_ms(Timer *timer);

/**
 * @brief Initialize a timer
 *
 * Performs any initialization required to the timer passed in.
 *
 * @param timer - pointer to the timer to be initialized
 */
void InitTimer(Timer *timer);


char* Get_Compile_Date(void);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif


