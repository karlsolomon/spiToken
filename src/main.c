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
    Timer_Sleep(10);
    pinMode(SPI_CS_PIN, OUTPUT);
    Timer_Sleep(10);
    pinMode(LED_TOKEN, OUTPUT);
    Timer_Sleep(10);
    pinMode(LED_INPROGRESS, OUTPUT);
    Timer_Sleep(10);
    pinMode(LED_FAIL, OUTPUT);
    Timer_Sleep(10);
    pinMode(LED_SUCCESS, OUTPUT);
    Timer_Sleep(10);
    pinMode(LOFO, INPUT);
    //Token_Init();   
    while(1)
    {
        printf("SPI_CS_PIN = %d\n", digitalRead(SPI_CS_PIN));
        digitalWrite(SPI_CS_PIN, !digitalRead(SPI_CS_PIN));
        printf("LOFO = %d\n", digitalRead(LOFO));
        digitalWrite(LOFO, !digitalRead(LOFO));
        printf("LED_TOKEN = %d\n", digitalRead(LED_TOKEN));
        digitalWrite(LED_TOKEN, !digitalRead(LED_TOKEN));
        printf("LED_INPROGRESS = %d\n", digitalRead(LED_INPROGRESS));
        digitalWrite(LED_INPROGRESS, !digitalRead(LED_INPROGRESS));
        printf("LED_FAIL = %d\n", digitalRead(LED_FAIL));
        digitalWrite(LED_FAIL, !digitalRead(LED_FAIL));
        printf("LED_SUCCESS = %d\n", digitalRead(LED_SUCCESS));
        digitalWrite(LED_SUCCESS, !digitalRead(LED_SUCCESS));
        Timer_Sleep(TIMER_1SEC);
        // do stuff
    }
    return 0;
}
