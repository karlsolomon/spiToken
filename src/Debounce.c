#include <semaphore.h>
#include "TypeDefs.h"
#include <wiringPi.h>
#include "Token.h"
#include "Timer.h"
#include <stdio.h>
#include <sched.h>

extern sem_t g_tokenSem;
extern bool m_isInserted;

#define DEBOUNCE_TIME_MS TIMER_50MS
#define DEBOUNCE_TIMEOUT TIMER_200

// Writes new status register
static void debounce_inserting(void);

// Handles debounce if token is being removed
static void debounce_removing(void);

/*******************************************************************************
 * @brief Debounce_Main
 *
 * Run Debounce thread
 *
 * @param  > None
 *
 * @return None
 *
 ******************************************************************************/
void* Debounce_Main(void* a)
{
    printf("Entering Debounce_Main\n");
    while(1)
    {
        if(!digitalRead(LOFO) && !m_isInserted)
        {
            debounce_inserting();
        }
        else if(digitalRead(LOFO) && m_isInserted)
        {
            debounce_removing();
        }
    }
    return 0;
}

/*******************************************************************************
 * @brief debounce_inserting
 *
 * Writes new status register
 *
 * @param  > None
 *
 * @return None
 *
 ******************************************************************************/
static void debounce_inserting(void)
{
    printf("inserting\n");
    uint32_t startTime = Timer_GetTick();
    for(uint32_t i = 0; i < DEBOUNCE_TIME_MS; i++)
    {
        if(digitalRead(LOFO))
        {
            i = 0; // restart debounce
        }
        if(Timer_TimeoutExpired(startTime, DEBOUNCE_TIMEOUT))
        {
            break;
        }
        Timer_Sleep(TIMER_1MS);
    }
    if(!Timer_TimeoutExpired(startTime, DEBOUNCE_TIMEOUT) && !digitalRead(LOFO))
    {
        sem_wait(&g_tokenSem);
        m_isInserted = true;
        sem_post(&g_tokenSem);
	printf("inserted\n");
    }
    else
    {
        printf("insert failed\n");
    }
}

/*******************************************************************************
 * @brief debounce_removing
 *
 * Handles debounce if token is being removed
 *
 * @param  > None
 *
 * @return None
 *
 ******************************************************************************/
static void debounce_removing(void)
{
    printf("removing\n");
    uint32_t startTime = Timer_GetTick();
    for(uint32_t i = 0; i < DEBOUNCE_TIME_MS; i++)
    {
        if(!digitalRead(LOFO))
        {
            i = 0; // restart debounce
        }
        if(Timer_TimeoutExpired(startTime, DEBOUNCE_TIMEOUT))
        {
            break;
        }
        Timer_Sleep(TIMER_1MS);
    }
    if(!Timer_TimeoutExpired(startTime, DEBOUNCE_TIMEOUT) && digitalRead(LOFO))
    {
        sem_wait(&g_tokenSem);
        m_isInserted = false;
        sem_post(&g_tokenSem);
    	printf("removed\n");
    }
    else
    {
        printf("remove failed");
    }
}
