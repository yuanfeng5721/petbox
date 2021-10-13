/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Wireless Solutions.
*
*****************************************************************************/
/*******************************************************************************
 *
 * Filename:
 * ---------
 *	btlink_timer.h
 *
 * Project:
 * --------
 *  
 *
 * Description:
 * ------------
 *   AVL GPS tracker protocol message definition
 *
 * Author:
 * -------
 * -------
 *
 *******************************************************************************/

#ifndef _BTLINK_TIMER_H_
#define _BTLINK_TIMER_H_

/******************************************************************************
* Include Files
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <trace.h>

/*****************************************************************************
* Define
*****************************************************************************/

/******************************************************************************
* Global functions
******************************************************************************/
extern int btink_min_timer_init(void);
extern int btlink_start_min_timer(void);
extern int btlink_stop_min_timer(void);

#endif /* #ifndef _BTLINK_TIMER_H_ */