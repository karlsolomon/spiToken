#include <semaphore.h>
#include "TypeDefs.h"
#include <wiringPi.h>
#include "Token.h"
#include "Timer.h"
#include <sched.h>

extern sem_t g_tokenSem;
extern bool m_isInserted;

#define DEBOUNCE_TIME_MS 50
#define DEBOUNCE_TIMEOUT 200

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
    while(1)
    {
        if(digitalRead(TOKEN_LOFO_PIN) && !m_isInserted)
        {
            debounce_inserting();
        }
        else if(!digitalRead(TOKEN_LOFO_PIN) && m_isInserted)
        {
            debounce_removing();
        }
        sched_yield();
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
    uint32_t startTime = Timer_GetTick();
    for(uint32_t i = 0; i < DEBOUNCE_TIME_MS; i++)
    {
        if(!digitalRead(TOKEN_LOFO_PIN))
        {
            i = 0; // restart debounce
        }
        if(Timer_TimeoutExpired(startTime, DEBOUNCE_TIMEOUT))
        {
            break;
        }
        Timer_Sleep(1);
    }
    if(!Timer_TimeoutExpired(startTime, DEBOUNCE_TIMEOUT) && !digitalRead(TOKEN_LOFO_PIN))
    {
        sem_wait(&g_tokenSem);
        m_isInserted = true;
        sem_post(&g_tokenSem);
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
    uint32_t startTime = Timer_GetTick();
    for(uint32_t i = 0; i < DEBOUNCE_TIME_MS; i++)
    {
        if(digitalRead(TOKEN_LOFO_PIN))
        {
            i = 0; // restart debounce
        }
        if(Timer_TimeoutExpired(startTime, DEBOUNCE_TIMEOUT))
        {
            break;
        }
        Timer_Sleep(1);
    }
    if(!Timer_TimeoutExpired(startTime, DEBOUNCE_TIMEOUT) && digitalRead(TOKEN_LOFO_PIN))
    {
        sem_wait(&g_tokenSem);
        m_isInserted = false;
        sem_post(&g_tokenSem);
    }
}
