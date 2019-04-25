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
extern bool m_isInserted;

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
    wiringPiSetupGpio();
    Timer_Init();
    pinMode(SPI_CS_PIN, OUTPUT);
    pinMode(LED_TOKEN, OUTPUT);
    pinMode(LED_INPROGRESS, OUTPUT);
    pinMode(LED_FAIL, OUTPUT);
    pinMode(LED_SUCCESS, OUTPUT);
    pinMode(LOFO, INPUT);
    Token_Init();   
    while(1)
    {
        Timer_Sleep(100);
            // do stuff
    }
    return 0;
}


static void programToken(void)
{
    if(m_isInserted)
    {
        // program and verify
    }
}
