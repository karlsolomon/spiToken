/*******************************************************************************
 *  @file Token.c
 *
 *  @brief Utils For both EEPROM & Flash Tokens
 *
 *  @author KSolomon
 *  @date Nov 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/


/******************************************************************************
 * Include Section
 ******************************************************************************/

// System Includes
#include <wiringPi.h>
#include <semaphore.h>
#include <pthread.h>
#include "TypeDefs.h"
#include <stdio.h>

// Module Includes
#include "Token.h"
#include "TokenFlash.h"

// Utility Includes

// Driver Includes
#include "spi.h"
#include "Debounce.h"
#include "Timer.h"


/*******************************************************************************
 * Constants Declarations
 ******************************************************************************/

#define TOKEN_READY_BIT                         0x01
#define TOKEN_WREN_BIT                          0x02

sem_t g_tokenSem;
bool m_isInserted = false;
bool m_isStatusChanged = false;
pthread_t debounceThread;


/*******************************************************************************
 * Data Types Declarations
 ******************************************************************************/


/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

// Disables writing to token
static void token_writeDisable(void);

// Determines if Status Register suggests that the Token is ready to write/erase
static bool token_isReady(void);

// Determines if Status Register suggests that the Token is write-enabled
static bool token_isWriteEnabled(void);


/*******************************************************************************
 * Public Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief Token_Init
 *
 * Initialize Token SPI port. Call once @ project startup
 *
 * @param  > None
 *
 * @return None
 *
 ******************************************************************************/
void Token_Init(void)
{    
    SPI_Init();    
    pthread_create(&debounceThread, NULL, Debounce_Main, NULL);
    sem_init(&g_tokenSem, 1, 1);
    Timer_Sleep(TIMER_1SEC); // recognize if token is inserted @ startup
}


/*******************************************************************************
 * @brief Token_WriteEnable
 *
 * Enable Writing. Must be called before any write/erase operation
 *
 * @param  > None
 *
 * @return TOKEN_ErrCode_t
 *
 ******************************************************************************/
TOKEN_ErrCode_t Token_WriteEnable(void)
{
    TOKEN_ErrCode_t err = TOKEN_ERR_TIMEOUT;
    if(Token_WaitUntilReady())
    {
        uint8_t opCode = (uint8_t) TOKEN_OPCODE_WRITE_ENABLE;
        err = (TOKEN_ErrCode_t) SPI_Write(&opCode, 1);
    }
    else
    {
        printf("Error, timeout trying to write enable\n");
    }
    return err;
}

/*******************************************************************************
 * @brief Token_IsInserted
 *
 * Polling function to determine if the Token is inserted.
 *
 * @param  > None
 *
 * @return bool : true if Token is inserted + debounce time, false otherwise
 *
 ******************************************************************************/
bool Token_IsInserted(void)
{
    return m_isInserted;
}

/*******************************************************************************
 * @brief Token_WaitUntilReady
 *
 * Waits until the Token is ready for another write/erase operation, or until
 * a timeout was hit.
 *
 * @param  > None
 *
 * @return bool : true if Token is ready, false if timeout reached
 *
 ******************************************************************************/
bool Token_WaitUntilReady(void)
{
    return Token_WaitUntilReady_time(TOKEN_TIMEOUT_SMALL);
}

/*******************************************************************************
 * @brief Token_WaitUntilReady_time
 *
 * Waits until the Token is ready for another write/erase operation, or until
 * a timeout was hit.
 *
 * @param  > None
 *
 * @return bool : true if Token is ready, false if timeout reached
 *
 ******************************************************************************/
bool Token_WaitUntilReady_time(uint32_t time)
{
    bool ready = true;
    uint32_t startTime = Timer_GetTick();
    while(!token_isReady())
    {
        if(Timer_TimeoutExpired(startTime, time) || !Token_IsInserted())
        {
            ready = false;
            break;
        }
    }
    return ready;
}

/*******************************************************************************
 * @brief Token_WriteStatusRegister
 *
 * Writes new status register
 *
 * @param  > uint8_t : new status register
 *
 * @return TOKEN_ErrCode_t
 *
 ******************************************************************************/
TOKEN_ErrCode_t Token_WriteStatusRegister(uint8_t sr)
{
    TOKEN_ErrCode_t err = Token_WriteEnable();
    if(err == TOKEN_ERR_OK)
    {
        uint8_t opCode = TOKEN_OPCODE_WRITE_SR;
        uint8_t instr[2] = {opCode, sr};
        err = (TOKEN_ErrCode_t) SPI_Write(instr, sizeof(instr));
    }
    return err;
}

/*******************************************************************************
 * @brief Token_ReadStatusRegister
 *
 * Reads status register
 *
 * @param  > None
 *
 * @return uint8_t statusRegister
 *
 ******************************************************************************/
uint8_t Token_ReadStatusRegister(void)
{
    uint8_t statusRegister = 0;
    uint8_t opcode = TOKEN_OPCODE_READ_SR;
    SPI_WriteRead(&opcode, 1, &statusRegister, 1);
    return statusRegister;
}

/*******************************************************************************
 * @brief Token_GetDeviceType
 *
 * Get Token Device Type
 *
 * @param  > None
 *
 * @return TOKEN_t
 *
 ******************************************************************************/
TOKEN_t Token_GetDeviceType(void)
{
    TOKEN_t tokenType = TOKEN_NONE;
    uint32_t size = 0;
    TOKEN_ErrCode_t err = TokenFlash_GetDeviceSize(&size);
    if(err == TOKEN_ERR_OK)
    {
        if(size != 0)
        {
            tokenType = TOKEN_FLASH;
            printf("flash token size = %d\n", size);
        }
        else
        {
            printf("eeprom token. Invalid for programmer application!!\n");
            tokenType = TOKEN_EEPROM;
        }
    }
    else
    {
        printf("err = %d", err);
    }
    return tokenType;
}


/*******************************************************************************
 * Private Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief token_writeDisable
 *
 * Disables writing to token
 *
 * @param  > None
 *
 * @return None
 *
 ******************************************************************************/
static void token_writeDisable(void)
{
    uint8_t opCode = (uint8_t) TOKEN_OPCODE_WRITE_DISABLE;
    SPI_Write(&opCode, 1);
}

/*******************************************************************************
 * @brief token_isReady
 *
 * Determines if Status Register suggests that the Token is ready to write/erase
 *
 * @param  > None
 *
 * @return bool: true if Token is ready to write/erase, false otherwise
 *
 ******************************************************************************/
static bool token_isReady(void)
{
    return !(Token_ReadStatusRegister() & TOKEN_READY_BIT);
}

/*******************************************************************************
 * @brief token_isWriteEnabled
 *
 * Determines if Status Register suggests that the Token is write-enabled
 *
 * @param  > None
 *
 * @return bool: true if Token is ready to write-enabled, false otherwise
 *
 ******************************************************************************/
static bool token_isWriteEnabled(void)
{
    return Token_ReadStatusRegister() & TOKEN_WREN_BIT;
}

// EOF
