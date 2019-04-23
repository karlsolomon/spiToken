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

// Verify token is connected and is of valid type
static int testToken_GetDeviceTypeTest(void);

int main(void)
{
    wiringPiSetup();
    Timer_Init();
    pinMode(OUTPUT, SPI_CS_PIN);
    pinMode(INPUT, LOFO);
    sem_init(&g_tokenSem, 1, 1);
    pthread_create(&debounceThread, NULL, Debounce_Main, NULL);
    uint32_t startTick = Timer_GetTick();
    while(1)
    {
        uint32_t tick = Timer_GetTick();
        printf("time = %d\n", tick);
        // testToken_GetDeviceTypeTest();
        Timer_Sleep(1000);
        if(Timer_TimeoutExpired(startTick, 5000))
        {
            printf("timeout expired! start = %d, current = %d", startTick, Timer_GetTick());
            startTick = Timer_GetTick();
        }
    }
    return 0;
}

/*******************************************************************************
 * @brief testToken_GetDeviceTypeTest
 *
 * Verify token is connected and is of valid type
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static void testToken_GetDeviceTypeTest(void)
{
    // TOKEN_t type = Token_GetDeviceType();
    if(type == TOKEN_NONE)
    {
        errCount++;
        printf("No Token Connected\n");
    }
}
