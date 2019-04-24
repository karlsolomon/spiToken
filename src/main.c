#include <stdio.h>
#include "Timer.h"
#include "Debounce.h"
#include "Token.h"
#include <pthread.h>
#include <semaphore.h>
#include <wiringPi.h>
#include "TypeDefs.h"
#include "test.h"
#include "TokenFlash.h"

#define TEST_TOKEN_RW_SIZE    256

#define TOK_F_WRITE             ((WriteAndVerifyHook) TokenFlash_Write)
#define TOK_F_ERASE             ((EraseHook) TokenFlash_Erase)
#define TOK_F_READ              ((WriteAndVerifyHook) TokenFlash_Read)

#define TEST_TOKEN_START_ADDR   0



static TOKEN_t m_tokenType = TOKEN_NONE;


sem_t g_tokenSem;
bool m_isInserted = false;
pthread_t debounceThread;

// Verify token is connected and is of valid type
static void testToken_GetDeviceTypeTest(void);

// Verify token can read
static void testToken_flash_readTest(void);

// Verify token can write
static void testToken_flash_writeTest(void);


// Verify token can enable writing
static void testToken_flash_writeAll(void);

// Verify token can erase
static void testToken_flash_eraseTest(void);

// Verify token can erase all memory through erase cmd
static void testToken_flash_eraseAllTest(void);

// Verify token can erase chip
static void testToken_flash_eraseChipTest(void);

// Verify token can protect all region combinations
static void testToken_flash_protectTest(void);

int main(void)
{
    wiringPiSetup();
    Timer_Init();
    Token_Init();
    pinMode(OUTPUT, SPI_CS_PIN);
    pinMode(INPUT, LOFO);
    sem_init(&g_tokenSem, 1, 1);
    pthread_create(&debounceThread, NULL, Debounce_Main, NULL);
    uint32_t startTick = Timer_GetTick();
    while(1)
    {
        uint32_t tick = Timer_GetTick();
        printf("time = %d\n", tick);
        testToken_GetDeviceTypeTest();
        Timer_Sleep(1000);
	    testToken_GetDeviceTypeTest();
        testToken_flash_readTest();
        testToken_flash_writeTest();
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
    TOKEN_t type = Token_GetDeviceType();
    if(type == TOKEN_NONE)
    {
        printf("No Token Connected\n");
    }
}

/*******************************************************************************
 * @brief testToken_flash_writeEnable
 *
 * Verify token can read
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static void testToken_flash_readTest(void)
{
    uint8_t tmpReadBuffer[TOKEN_FLASH_PAGE_LEN] = {0};
    TOKEN_ErrCode_t err = (TOKEN_ErrCode_t) TokenFlash_Read(TEST_TOKEN_START_ADDR, tmpReadBuffer, TOKEN_FLASH_PAGE_LEN);
    if(err != TOKEN_ERR_OK)
    {
        printf("err = %d. Read failed\n", err);
    }
}

/*******************************************************************************
 * @brief testToken_flash_writeEnable
 *
 * Verify token can write
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static void testToken_flash_writeTest(void)
{
    TOKEN_ErrCode_t err = TokenFlash_Erase(TEST_TOKEN_START_ADDR, TOKEN_FLASH_SECTOR_LEN);
    if(!Test_WriteAndVerify(TOK_F_WRITE, TOK_F_READ, TEST_TOKEN_START_ADDR, TOKEN_FLASH_SECTOR_LEN))
    {
        printf("err = %d. Verify failed\n", err);
    }
    TOK_F_ERASE(TEST_TOKEN_START_ADDR, TOKEN_FLASH_SECTOR_LEN);
}

/*******************************************************************************
 * @brief testToken_flash_writeEnable
 *
 * Verify token can enable writing
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static void testToken_flash_writeAll(void)
{
    TOKEN_ErrCode_t err = TokenFlash_Erase(0, TOKEN_FLASH_MEM_SIZE);
    if(!Test_WriteAndVerify(TOK_F_WRITE, TOK_F_READ, 0, TOKEN_FLASH_MEM_SIZE))
    {
        printf("err = %d. Verify failed\n", err);
    }
    TOK_F_ERASE(0, TOKEN_FLASH_MEM_SIZE);
}

/*******************************************************************************
 * @brief testToken_flash_writeEnable
 *
 * Verify token can erase
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static void testToken_flash_eraseTest(void)
{
    TOKEN_ErrCode_t err = TokenFlash_Erase(TEST_TOKEN_START_ADDR, TOKEN_FLASH_SECTOR_LEN);
    if(err != TOKEN_ERR_OK)
    {
        errCount++;
        printf("err = %d. Erase failed\n", err);
    }
    if(!Test_VerifyErased(TOK_F_READ, TEST_TOKEN_START_ADDR, TOKEN_FLASH_SECTOR_LEN))
    {
        errCount++;
        printf("err = %d. Verify failed\n", err);
    }
}

/*******************************************************************************
 * @brief testToken_flash_eraseAllTest
 *
 * Verify token can erase all memory through erase cmd
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static void testToken_flash_eraseAllTest(void)
{
    TOKEN_ErrCode_t err = TokenFlash_Erase(TEST_TOKEN_START_ADDR, TOKEN_FLASH_MEM_SIZE);
    if(err != TOKEN_ERR_OK)
    {
        printf("err = %d\n", err);
    }
    if(!Test_VerifyErased(TOK_F_READ, 0, TOKEN_FLASH_MEM_SIZE))
    {
        printf("err = %d. Verify failed\n", err);
    }
}

/*******************************************************************************
 * @brief testToken_flash_eraseChipTest
 *
 * Verify token can erase chip
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static void testToken_flash_eraseChipTest(void)
{
    int errCount = 0;
    TOKEN_ErrCode_t err = TokenFlash_EraseAll();
    Timer_Sleep(TOKEN_FLASH_ERASE_ALL_TIME);
    if(err != TOKEN_ERR_OK)
    {
        errCount++;
        printf("err = %d", err);
    }
    if(!Test_VerifyErased(TOK_F_READ, 0, TOKEN_FLASH_MEM_SIZE))
    {
        errCount++;
        printf("err = %d. Verify failed\n", err);
    }
}

/*******************************************************************************
 * @brief testToken_flash_protectTest
 *
 * Verify token can protect all region combinations
 *
 * @param  None
 *
 * @return int
 *
 ******************************************************************************/
static void testToken_flash_protectTest(void)
{
    uint8_t protectedRegion = 0;
    TOKEN_ErrCode_t err = TOKEN_ERR_OK;
    for(uint8_t i = 0; i < (uint8_t) TOKEN_FLASH_PROTECT_COUNT; i++)
    {
        err = TokenFlash_ProtectRegion((TOKEN_FlashProtect_t) i);
        protectedRegion = (uint8_t) TokenFlash_GetProtectedRegion();
        if(i != protectedRegion)
        {
            printf("err = %d. Protection failed. Expected = %d, actual = %d\n", err, i, protectedRegion);
            break;
        }
    }
    TokenFlash_ProtectRegion(TOKEN_FLASH_PROTECT_NONE);
}

