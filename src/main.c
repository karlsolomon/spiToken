#include <stdio.h>
#include "Timer.h"
#include "Token.h"
#include <pthread.h>
#include <semaphore.h>
#include <wiringPi.h>
#include "TypeDefs.h"
#include "test.h"
#include "TokenFlash.h"

extern sem_t g_tokenSem;
extern bool m_isInserted = false;

/*******************************************************************************
 * @brief main
 *
 * Run main
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
int main(void)
{
    Token_Init();   
    while(1)
    {
        digitalWrite(LED_TOKEN, !digitalRead(LED_TOKEN));
        digitalWrite(LED_INPROGRESS, !digitalRead(LED_INPROGRESS));
        digitalWrite(LED_FAIL, !digitalRead(LED_FAIL));
        digitalWrite(LED_SUCCESS, !digitalRead(LED_SUCCESS));
        Timer_Sleep(TIMER0_1SEC);
        // do stuff
    }
    return 0;
}
