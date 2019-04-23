/*******************************************************************************
 *  @file TokenFlash.h
 *
 *  @brief Utils for Flash tokens
 *
 *  @author KSolomon
 *  @date Nov 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/

#ifndef _TOKEN_FLASH_H_
#define _TOKEN_FLASH_H_


/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "TypeDefs.h"
#include "Token.h"


/*******************************************************************************
 * Macros
 ******************************************************************************/

#define TOKEN_FLASH_PAGE_LEN     0x100
#define TOKEN_FLASH_SECTOR_LEN   0x10000
#define TOKEN_FLASH_MEM_SIZE     0x100000

// From Datasheet Table 8: AC Characteristics
#define TOKEN_FLASH_ERASE_ALL_TIME (161*TIMER_1SEC) // Datasheet says 20 seconds for bulk erase for 8Mb; 64Mb will take 8x longer, so 160 seconds.
#define TOKEN_FLASH_ERASE_SECTOR_TIME (3*TIMER_1SEC)


/*******************************************************************************
 * Public Declarations
 ******************************************************************************/

typedef enum
{
    TOKEN_FLASH_PROTECT_NONE,
    TOKEN_FLASH_PROTECT_SIXTEENTH,
    TOKEN_FLASH_PROTECT_EIGTH,
    TOKEN_FLASH_PROTECT_QUARTER,
    TOKEN_FLASH_PROTECT_HALF,
    TOKEN_FLASH_PROTECT_ALL,
    TOKEN_FLASH_PROTECT_COUNT
} TOKEN_FlashProtect_t;

// Erase Token - sets all bytes to 0xFF
// Erase granularity = Sector (TOKEN_FLASH_SECTOR_LEN)
// This will erase whole sectors (incl. below given address if it isn't sector start)
TOKEN_ErrCode_t TokenFlash_Erase(uint32_t address, uint32_t len);

// Erase entire Flash Token
TOKEN_ErrCode_t TokenFlash_EraseAll(void);

// Same, but blocking.
TOKEN_ErrCode_t TokenFlash_EraseAllBlocking(void);

// Write to Token. Can only program (write) 0s. Thus, for data to be valid, 
// caller should erase this section first.
// Write granularity = Page (TOKEN_FLASH_PAGE_LEN)
// Erase granularity = Sector (TOKEN_FLASH_SECTOR_LEN)
TOKEN_ErrCode_t TokenFlash_Write(uint32_t startAddress, uint8_t* buf, uint32_t len);

// Read from Token
TOKEN_ErrCode_t TokenFlash_Read(uint32_t address, uint8_t* buf, uint32_t len);

// Protect a given region of FLASH token. This will protect the highest region. 
// So if TOKEN_FLASH_PROTECT_QUARTER is passed, only the highest quarter of 
// memory will be protected.
TOKEN_ErrCode_t TokenFlash_ProtectRegion(TOKEN_FlashProtect_t region);

// Protect a given region of FLASH token. This will protect the highest region. 
// So if TOKEN_FLASH_PROTECT_QUARTER is passed, only the highest quarter of 
// memory will be protected.
TOKEN_FlashProtect_t TokenFlash_GetProtectedRegion(void);

// Get Token Device Size
TOKEN_ErrCode_t TokenFlash_GetDeviceSize(uint32_t* size);

#endif /* _TOKEN_FLASH_H_  */
