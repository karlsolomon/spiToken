#include <stdio.h>
#include "Timer.h"
#include "Debounce.h"
#include "Token.h"
#include <pthread.h>
#include <semaphore.h>
#include <wiringPi.h>
#include "TypeDefs.h"

sem_t g_tokenSem;
bool m_isInserted = false;
pthread_t debounceThread;

int main(void)
{
    wiringPiSetup();
    Timer_Init();
    pinMode(OUTPUT, SPI_CS_PIN);
    pinMode(INPUT, LOFO);
    sem_init(&g_tokenSem, 1, 1);
    pthread_create(&debounceThread, NULL, Debounce_Main, NULL);
    while(1)
    {
        uint32_t tick = Timer_GetTick();
        printf("time = %d\n", tick);
        Timer_Sleep(1000);
    }
    return 0;
}
