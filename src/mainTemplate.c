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
        // do stuff
    }
    return 0;
}
