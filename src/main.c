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
extern bool m_isStatusChanged;
static FILE* fp;
static uint8_t tmpBuf[256] = {0};

// Program the flash token
static void programToken(void);

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
        if(m_isInserted && m_isStatusChanged)
        {
            sem_wait(&g_tokenSem);
            m_isStatusChanged = false;
            sem_post(&g_tokenSem);
            programToken();
        }
        Timer_Sleep(TIMER_1SEC);
    }
    return 0;
}

/*******************************************************************************
 * @brief programToken
 *
 * Program the flash token
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static void programToken(void)
{
    digitalWrite(LED_INPROGRESS, 1);
    digitalWrite(LED_SUCCESS, 0);
    digitalWrite(LED_FAIL, 0);
    fp = fopen(FILE_PATH, "r+");
    uint16_t size = 0;
    uint32_t addr = 0;
    TokenFlash_Erase(0, TOKEN_FLASH_SECTOR_LEN);
    Timer_Sleep(TIMER_5SEC);
    bool passed = false;
    do
    {
        size = (uint16_t) fread(tmpBuf, 1, sizeof(tmpBuf), fp);
        passed = Test_WriteAndVerify(TOK_F_WRITE, TOK_F_READ, addr, size);
        if(!passed)
        {
            digitalWrite(LED_FAIL, 1);
            digitalWrite(LED_INPROGRESS, 0);
            digitalWrite(LED_SUCCESS, 0);
            printf("failed token write and verify");
            break;
        }
        addr += size;
    } while(size != 0);
    if(passed)
    {        
        digitalWrite(LED_FAIL, 0);
        digitalWrite(LED_INPROGRESS, 1);
        digitalWrite(LED_SUCCESS, 1);
        printf("passed token write and verify");
    }
}
