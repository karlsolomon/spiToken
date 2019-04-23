/*******************************************************************************
 * @file Timer.c
 *
 * @brief Timer 
 *
 * @author KSolomon
 * @date Jun 15, 2018
 * @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/

/******************************************************************************
 * Include Section
 ******************************************************************************/

// System Includes
#include "/usr/include/time.h"
#include <wiringPi.h>

// Module Includes

// Utility Includes

// Driver Includes
#include "Timer.h"


/*******************************************************************************
 * Constants Declarations
 ******************************************************************************/

struct timespec timeVal;
uint64_t startTime = 0;


/*******************************************************************************
 * Data Types Declarations
 ******************************************************************************/


/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Public Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief Timer_Init
 *
 * Initialize Timer. Interrupt-Driven counter that increments every 1ms
 *
 * @param  > None
 *
 * @return None
 *
 ******************************************************************************/
void Timer_Init(void)
{
    struct timespec startTimeVal;
    clock_gettime(CLOCK_MONOTONIC, &startTimeVal);
    startTime = startTimeVal.tv_sec*1000 + (timeVal.tv_nsec / 1000000);
}

/*******************************************************************************
 * @brief Timer_GetTick
 *
 * Return 1ms Counter
 *
 * @param > None
 *
 * @return uint32_t: 1ms Coutner value
 *
 ******************************************************************************/
uint32_t Timer_GetTick(void)
{
    clock_gettime(CLOCK_MONOTONIC, &timeVal);
    uint64_t time = timeVal.tv_sec*1000 + (timeVal.tv_nsec / 1000000);
    return (uint32_t) time - startTime;
}

/*******************************************************************************
 * @brief Timer_Sleep
 *
 * Spin loop (busy wait) for mSec milliseconds
 *
 * @param  > uint32_t: mSec to sleep
 *
 * @return None
 *
 ******************************************************************************/
void Timer_Sleep(uint32_t mSec)
{
    delay(mSec);
}

/*******************************************************************************
 * @brief Timer_TimeoutExpired
 *
 * Verify that a time hasn't passed. Use in loop to verify an event occurs
 * before some defined time that is longer than expected. True if duration has
 * expired
 *
 * @param  > uint32_t: startTime of timeout
 *         > uint32_t: duration of timeout
 *
 * @return bool: True if timeout has been reached, false otherwise
 *
 ******************************************************************************/
bool Timer_TimeoutExpired(uint32_t startTime, uint32_t duration)
{
    return (Timer_GetTick() - startTime) > duration;
}
