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

#define TEST_TOKEN_RW_SIZE      256
#define TOK_F_WRITE             ((WriteAndVerifyHook) TokenFlash_Write)
#define TOK_F_ERASE             ((EraseHook) TokenFlash_Erase)
#define TOK_F_READ              ((WriteAndVerifyHook) TokenFlash_Read)
#define TEST_TOKEN_START_ADDR   0

extern sem_t g_tokenSem;
extern bool m_isInserted = false;

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
    uint32_t startTick = Timer_GetTick();
    uint32_t tick = Timer_GetTick();
    printf("time = %d\n", tick);    
    while(1)
    {
        testToken_GetDeviceTypeTest();
        tick = Timer_GetTick();
        printf("elapsedTime = %d GetDeviceType\n", tick - startTick);

        testToken_flash_readTest();
        tick = Timer_GetTick();
        printf("elapsedTime = %d Read\n", tick - startTick);
        startTick = tick;

        testToken_flash_writeTest();
        tick = Timer_GetTick();
        printf("elapsedTime = %d Write\n", tick - startTick);
        startTick = tick;

        testToken_flash_eraseTest();
        tick = Timer_GetTick();
        printf("elapsedTime = %d Erase\n", tick - startTick);
        startTick = tick;

        testToken_flash_protectTest();
        tick = Timer_GetTick();
        printf("elapsedTime = %d Protect\n", tick - startTick);
        startTick = tick;

        testToken_flash_writeAll();
        tick = Timer_GetTick();
        printf("elapsedTime = %d WriteAll\n", tick - startTick);
        startTick = tick;

        testToken_flash_eraseAllTest();
        tick = Timer_GetTick();
        printf("elapsedTime = %d EraseAll\n", tick - startTick);
        startTick = tick;

        testToken_flash_eraseChipTest();
        tick = Timer_GetTick();
        printf("elapsedTime = %d EraseChip \n", tick - startTick);
        startTick = tick;
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
        printf("err = %d. testToken_flash_readTest failed\n", err);
    }
    else
    {
	printf("readTest passed\n");
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
        printf("err = %d. testToken_flash_writeTest failed\n", err);
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
        printf("err = %d. testToken_flash_writeAll failed\n", err);
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
        printf("err = %d. testToken_flash_eraseTest erase failed\n", err);
    }
    else if(!Test_VerifyErased(TOK_F_READ, TEST_TOKEN_START_ADDR, TOKEN_FLASH_SECTOR_LEN))
    {
        printf("err = %d. testToken_flash_eraseTest verify failed\n", err);
    }
    else
    {
	printf("eraseTest passed\n");
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
        printf("err = %d. testToken_flash_eraseAllTest erase failed \n", err);
    }
    else if(!Test_VerifyErased(TOK_F_READ, 0, TOKEN_FLASH_MEM_SIZE))
    {
        printf("err = %d. testToken_flash_eraseAllTest verify failed\n", err);
    }
    else
    {
	printf("eraseAllTest passed\n");
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
    TOKEN_ErrCode_t err = TokenFlash_EraseAll();
    Timer_Sleep(TOKEN_FLASH_ERASE_ALL_TIME);
    if(err != TOKEN_ERR_OK)
    {
        printf("err = %d. testToken_flash_eraseChipTest erase failed\n", err);
    }
    else if(!Test_VerifyErased(TOK_F_READ, 0, TOKEN_FLASH_MEM_SIZE))
    {
        printf("err = %d. testToken_flash_eraseChipTest verify failed\n", err);
    }
    else
    {
	printf("eraseChipTest passed\n");
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
	Timer_Sleep(TIMER_10MS);
	protectedRegion = (uint8_t) TokenFlash_GetProtectedRegion();
        if(i != protectedRegion)
        {
            printf("err = %d. Protection failed. Expected = %d, actual = %d\n", err, i, protectedRegion);
            break;
        }
    }
    TokenFlash_ProtectRegion(TOKEN_FLASH_PROTECT_NONE);
}

