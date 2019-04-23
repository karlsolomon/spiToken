/*******************************************************************************
 *  @file Token.h
 *
 *  @brief Utils For both EEPROM & Flash Tokens
 *
 *  @author KSolomon
 *  @date Nov 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/

#ifndef _TOKEN_H_
#define _TOKEN_H_


/*******************************************************************************
 * Includes
 ******************************************************************************/

// System Includes
#include "TypeDefs.h"
#include "Timer.h"

// Module Includes

// Utility Includes

// Driver Includes



/*******************************************************************************
 * Macros
 ******************************************************************************/

#define TOKEN_TIMEOUT_LARGE         TIMER_1MIN
#define TOKEN_TIMEOUT_SMALL         TIMER_10SEC

#define TOKEN_PROTECT_OFFSET        2
#define TOKEN_PROTECT_ANTIMASK      3

#define TOKEN_REMOVED               0
#define TOKEN_INSERTED              1
#define TOKEN_UNPROGRAMMED_VALUE    0xFF


/*******************************************************************************
 * Public Declarations
 ******************************************************************************/

typedef enum
{
    TOKEN_NONE,
    TOKEN_EEPROM,
    TOKEN_FLASH,
    TOKEN_COUNT
} TOKEN_t;

typedef enum
{
    TOKEN_ERR_OK = 0,
    TOKEN_ERR_TIMEOUT,
    TOKEN_ERR_INVALID_INPUT,
    TOKEN_ERR_COUNT
} TOKEN_ErrCode_t;

typedef enum
{
    TOKEN_OPCODE_NONE                   = 0x00,
    TOKEN_OPCODE_WRITE_SR               = 0x01,
    TOKEN_OPCODE_WRITE                  = 0x02,
    TOKEN_OPCODE_READ                   = 0x03,
    TOKEN_OPCODE_WRITE_DISABLE          = 0x04,
    TOKEN_OPCODE_READ_SR                = 0x05,
    TOKEN_OPCODE_WRITE_ENABLE           = 0x06,
    TOKEN_OPCODE_FLASH_FAST_READ        = 0x0B,
    TOKEN_OPCODE_FLASH_SECTOR_ERASE     = 0xD8,
    TOKEN_OPCODE_FLASH_CHIP_ERASE       = 0xC7,
    TOKEN_OPCODE_FLASH_DEEP_POWER_DOWN  = 0xB9,
    TOKEN_OPCODE_FLASH_READ_E_SIGNATURE = 0xAB
} TOKEN_Opcode_t; // EEPROM Commands are 8 bit, Flash are 16 bit

// Initialize Token SPI port. Call once @ project startup
void Token_Init(void);

// Enable Writing. Must be called before any write/erase operation
TOKEN_ErrCode_t Token_WriteEnable(void);

// Callback for Token Insertion
void Token_LofoISR(bool isInserted);

// Callback for Token Insertion
void Token_Callback(void);

// Polling function to determine if the Token is inserted.
bool Token_IsInserted(void);

// Waits until the Token is ready for another write/erase operation, or until
// a timeout was hit.
bool Token_WaitUntilReady(void);

// Waits until token is ready with a desired timeout.
bool Token_WaitUntilReady_time(uint32_t time);

// Writes new status register
TOKEN_ErrCode_t Token_WriteStatusRegister(uint8_t sr);

// Reads status register
uint8_t Token_ReadStatusRegister(void);

// Get Token Device Type
TOKEN_t Token_GetDeviceType(void);

// ISR to handle debounce
void Token_DebounceCallback(void);

// Determine if token was just inserted/removed
bool Token_GetIsStateChanged(void);

// Set if token was just inserted/removed
void Token_SetIsStateChanged(bool isJustChanged);

#endif /* _TOKEN_H_  */
