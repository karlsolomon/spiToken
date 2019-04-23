/*******************************************************************************
 * @file Template.c
 *
 * @brief c file from which other c files in this project should be made
 *
 * @author KSolomon
 * @date Jun 15, 2018
 * @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/

/******************************************************************************
 * Include Section
 ******************************************************************************/

// System Includes
#include <sys/time.h>
#include <stdio.h>
#include <wiringPi.h>

// Module Includes

// Utility Includes

// Driver Includes
#include "Timer.h"


/*******************************************************************************
 * Constants Declarations
 ******************************************************************************/

static time_t m_time;
static time_t m_timeStart;
static struct tm time;
static struct tm timeStart;
static timer_t sysTick;
struct timerspec timeVal;

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
    timer_create(CLOCK_MONOTONIC, SIGEV_NONE, &sysTick);
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
    clock_getTime(CLOCK_MONOTONIC, timeVal);
    return tv_sec*1000 + (tv_nsec * 1000000);
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
    uint32_t m_TimerTimeout = startTime + duration;
    bool expired = false;
    uint32_t current = Timer_GetTick();
    if( current > m_TimerTimeout)
    {
        expired = true;
    }
    if(m_TimerTimeout < startTime) // overflow
    {
        expired = (m_TimerTimeout < startTime) && expired;
    }
    return expired;
}
