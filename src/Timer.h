/*******************************************************************************
 *  @file Timer.h
 *
 *  @brief  This module handles setup and control for Timers
 *          Timer2 is main system timer. Generates interrupt every 1ms.
 *
 *  @author KSolomon
 *  @date Jun 15, 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "TypeDefs.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

#define TIMER_0MS    (0)
#define TIMER_1MS    (1)
#define TIMER_3MS    (3 * TIMER_1MS)
#define TIMER_5MS    (5 * TIMER_1MS)
#define TIMER_10MS   (10 * TIMER_1MS)
#define TIMER_25MS   (25 * TIMER_1MS)
#define TIMER_50MS   (50 * TIMER_1MS)
#define TIMER_100MS  (100 * TIMER_1MS)
#define TIMER_200MS  (200 * TIMER_1MS)
#define TIMER_250MS  (250 * TIMER_1MS)
#define TIMER_500MS  (500 * TIMER_1MS)
#define TIMER_1SEC   (1000 * TIMER_1MS)
#define TIMER_2SEC   (2000 * TIMER_1MS)
#define TIMER_3SEC   (3000 * TIMER_1MS)
#define TIMER_4SEC   (4000 * TIMER_1MS)
#define TIMER_5SEC   (5000 * TIMER_1MS)
#define TIMER_6SEC   (6000 * TIMER_1MS)
#define TIMER_10SEC  (10000 * TIMER_1MS)
#define TIMER_15SEC  (15000 * TIMER_1MS)
#define TIMER_30SEC  (30000 * TIMER_1MS)
#define TIMER_1MIN   (60000 * TIMER_1MS)
#define TIMER_2MIN   (120000 * TIMER_1MS)
#define TIMER_5MIN   (300000 * TIMER_1MS)
#define TIMER_10MIN  (600000 * TIMER_1MS)

typedef enum
{
    TIMER_ERR_OK,
    TIMER_ERR_GENERAL,
    TIMER_ERR_COUNT
} TIMER_ErrCode_t;

/*******************************************************************************
 * Public Declarations
 ******************************************************************************/

// Start Internal interrupt-driven timer. Increments every 1ms.
void Timer_Init(void);

// Return current system time (in mSec)
uint32_t Timer_GetTick(void);

// Spin loop (busy wait) for mSec milliseconds
void Timer_Sleep(uint32_t mSec);

// Verify that a time hasn't passed. Use in loop to verify an event occurs
// before some defined time that is longer than expected. True if duration has
// expired
bool Timer_TimeoutExpired(uint32_t startTime, uint32_t duration);

#endif   /* _TIMER_H_  */
