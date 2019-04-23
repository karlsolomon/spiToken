/*******************************************************************************
 *  @file TokenEeprom.h
 *
 *  @brief Utils For EEPROM Token
 * 
 *  @author KSolomon
 *  @date Nov 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/

#ifndef _TOKEN_EEPROM_H_
#define _TOKEN_EEPROM_H_


/*******************************************************************************
 * Includes
 ******************************************************************************/


/*******************************************************************************
 * Macros
 ******************************************************************************/

#define TOKEN_EEPROM_PAGE_LEN           0x08
#define TOKEN_EEPROM_MEM_SIZE           0x100
#define TOKEN_EEPROM_ERASE_BYTE         0xFF
#define TokenEeprom_getDeviceSize()     TOKEN_EEPROM_MEM_SIZE


/*******************************************************************************
 * Public Declarations
 ******************************************************************************/

typedef enum
{
    TOKEN_EEPROM_PROTECT_NONE,
    TOKEN_EEPROM_PROTECT_QUARTER,
    TOKEN_EEPROM_PROTECT_HALF,
    TOKEN_EEPROM_PROTECT_ALL,
    TOKEN_EEPROM_PROTECT_COUNT
} TOKEN_EepromProtect_t;

// Erase Token - sets all bytes to 0xFF
// Erase granularity = Sector (TOKEN_EEPROM_SECTOR_LEN)
// This will erase whole sectors (incl. below given address if it isn't sector start)
TOKEN_ErrCode_t TokenEeprom_Erase(uint32_t address, uint32_t len);

// Erase entire Eeprom Token
TOKEN_ErrCode_t TokenEeprom_EraseAll(void);

// Write to Token. Can only program (write) 0s. Thus, for data to be valid, 
// caller should erase this section first.
// Write granularity = Page (TOKEN_EEPROM_PAGE_LEN)
// Erase granularity = Sector (TOKEN_EEPROM_SECTOR_LEN)
TOKEN_ErrCode_t TokenEeprom_Write(uint32_t startAddress, uint8_t* buf, uint32_t len);

// Read from Token
TOKEN_ErrCode_t TokenEeprom_Read(uint32_t address, uint8_t* buf, uint32_t len);

// Protect a given region of EEPROM token. This will protect the highest region. 
// So if TOKEN_EEPROM_PROTECT_QUARTER is passed, only the highest quarter of 
// memory will be protected.
TOKEN_ErrCode_t TokenEeprom_ProtectRegion(TOKEN_EepromProtect_t region);

// Get region protected. Protected area is last/highest of returned region. So 
// if TOKEN_EEPROM_PROTECT_QUARTER is returned, the last quarter of memory will
// be protected
TOKEN_EepromProtect_t TokenEeprom_GetProtectedRegion(void);

#endif /* _TOKEN_EEPROM_H_  */
