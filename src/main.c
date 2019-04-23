#include <stdio.h>
#include <stdint.h>
#include "Timer.h"
#include "Debounce.h"
#include "Token.h"
#include <pthread.h>
#include <semaphore.h>
#include <wiringPi.h>

#define SPI_CS_PIN                  0

sem_t g_tokenSem;
bool m_isInserted = false;
pthread_t debounceThread;

int main(void)
{
    wiringPiSetup();
    Timer_Init();
    pinMode(OUTPUT, SPI_CS_PIN);
    pinMode(INPUT, TOKEN_LOFO_PIN);
    pullUpDnControl(TOKEN_LOFO_PIN, PUD_UP);

    pthread_create(&debounceThread, NULL, &Debounce_Main, NULL);
    while(1)
    {
        uint32_t tick = Timer_GetTick();
        
        // toggle CS pin
	digitalWrite(SPI_CS_PIN, !digitalRead(SPI_CS_PIN));
	Timer_Sleep(1);
        printf("time = %d, CS = %s LOFO = %s\n", tick, digitalRead(SPI_CS_PIN) ? "HI" : "LO" , digitalRead(TOKEN_LOFO_PIN) ? "HI" : "LO");
        Timer_Sleep(99);
    }
    return 0;
}
