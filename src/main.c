#include <stdio.h>
#include "Timer.h"
#include "Token.h"
#include <pthread.h>
#include <semaphore.h>
#include <wiringPi.h>
#include "TypeDefs.h"
#include "test.h"
#include "TokenFlash.h"

#define STARTUP()       digitalWrite(LED_INPROGRESS, 0); digitalWrite(LED_SUCCESS, 0); digitalWrite(LED_FAIL, 0); digitalWrite(LED_TOKEN, 0)
#define INPROGRESS()    digitalWrite(LED_INPROGRESS, 1); digitalWrite(LED_SUCCESS, 0); digitalWrite(LED_FAIL, 0)
#define PASSED()        digitalWrite(LED_INPROGRESS, 0); digitalWrite(LED_SUCCESS, 1); digitalWrite(LED_FAIL, 0)
#define FAILED()        digitalWrite(LED_INPROGRESS, 0); digitalWrite(LED_SUCCESS, 0); digitalWrite(LED_FAIL, 1)

extern sem_t g_tokenSem;
extern bool m_isStatusChanged;
extern bool m_isInserted;
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
    STARTUP();
    Token_Init();
    while(1)
    {
        if(m_isStatusChanged)
        {
            if(m_isInserted)
            {
                sem_wait(&g_tokenSem);
                m_isStatusChanged = false;
                sem_post(&g_tokenSem);
                programToken();
            }
        }
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
    printf("Entering program token\n");
    fp = fopen(FILE_PATH, "r+");
    uint16_t size = 0;
    uint32_t addr = 0;
    INPROGRESS();
    TokenFlash_Erase(0, TOKEN_FLASH_SECTOR_LEN);
    Timer_Sleep(TOKEN_FLASH_ERASE_SECTOR_TIME);
    bool passed = false;
    bool eraseSector = false;
    uint32_t lastSector = 0;
    uint32_t currentSector = 0;
    do
    {
        size = (uint16_t) fread(tmpBuf, 1, sizeof(tmpBuf), fp);
        if(size != 0)
        {
            lastSector = (addr - 1) / TOKEN_FLASH_SECTOR_LEN;
            currentSector = (addr + size) / TOKEN_FLASH_SECTOR_LEN;
            if(currentSector > lastSector)
            {
                TokenFlash_Erase(currentSector*TOKEN_FLASH_SECTOR_LEN, TOKEN_FLASH_SECTOR_LEN);
                Timer_Sleep(TOKEN_FLASH_ERASE_SECTOR_TIME);
            }
            passed = Test_WriteAndVerify(TOK_F_WRITE, TOK_F_READ, addr, size);
            if(!passed)
            {
                printf("failed token write and verify\n");
                FAILED();
                break;
            }
            addr += size;            
        }
    } while(size != 0);
    if(passed)
    {
        PASSED();
        printf("passed token write and verify\n");
    }
    fclose(fp);
}
