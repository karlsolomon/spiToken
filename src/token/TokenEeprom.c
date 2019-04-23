/*******************************************************************************
 *  @file TokenEeprom.c
 *
 *  @brief Utils For EEPROM Tokens
 *
 *  @author KSolomon
 *  @date Nov 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/


/******************************************************************************
 * Include Section
 ******************************************************************************/

// System Includes

// Module Includes
#include "Token.h"
#include "TokenEeprom.h"

// Utility Includes
#include "Config.h"

// Driver Includes
#include "SPI.h"
#include "Debug.h"
#include "Timer.h"


/*******************************************************************************
 * Constants Declarations
 ******************************************************************************/

#define TOKEN_EEPROM_INSTRUCTION_SIZE    2


/*******************************************************************************
 * Data Types Declarations
 ******************************************************************************/


/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

// Get the formatted instruction w/ opcode and address. This transaction must be
// sent MSB/MSb first where Opcode is the MSB of the 4 byte transaction
static void tokenEeprom_getInstruction(uint8_t* instruction, uint32_t address, TOKEN_Opcode_t opCode);

// Write bufLen bytes from buf to given address of Eeprom Token
static TOKEN_ErrCode_t tokenEeprom_writePage(uint32_t address, uint8_t* buf, uint32_t bufLen);

// Determines if address is valid in memory
static bool tokenEeprom_isValidAddress(uint32_t address);


/*******************************************************************************
 * Public Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief TokenEeprom_Erase
 *
 * Erase Token - sets all bytes to 0xFF
 * Erase granularity = Sector (TOKEN_EEPROM_SECTOR_LEN)
 * This will erase whole sectors (incl. below given address if it isn't sector start)
 *
 * @param  > uint32_t : address to start erasing.
 *         > uint8_t* : buffer to write from
 *         > uint32_t : length to write
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
TOKEN_ErrCode_t TokenEeprom_Erase(uint32_t address, uint32_t len)
{
    TOKEN_ErrCode_t err = TOKEN_ERR_INVALID_INPUT;
    if(tokenEeprom_isValidAddress(address + len - 1))
    {
        err = TOKEN_ERR_OK;
        uint8_t erasePage[TOKEN_EEPROM_PAGE_LEN];
        uint32_t eraseLen;

        for(uint8_t i = 0; i < TOKEN_EEPROM_PAGE_LEN; i++)
        {
            erasePage[i] = TOKEN_EEPROM_ERASE_BYTE;
        }
        while(len > 0 && err == TOKEN_ERR_OK)
        {
            eraseLen = MIN(TOKEN_EEPROM_PAGE_LEN, len);
            err = TokenEeprom_Write(address, erasePage, eraseLen);
            address += eraseLen;
            len -= eraseLen;
        }
    }
    return err;
}

/*******************************************************************************
 * @brief TokenEeprom_EraseAll
 *
 * Erase entire Eeprom Token
 *
 * @param  > None
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
TOKEN_ErrCode_t TokenEeprom_EraseAll(void)
{
    TOKEN_ErrCode_t err = TOKEN_ERR_OK;
    for(uint32_t address = 0; address < TOKEN_EEPROM_MEM_SIZE; address += TOKEN_EEPROM_PAGE_LEN)
    {
        err = TokenEeprom_Erase(address, TOKEN_EEPROM_PAGE_LEN);
    }
    return err;
}

/*******************************************************************************
 * @brief TokenEeprom_Write
 *
 * Write to Token. Can only program (write) 0s. Thus, for data to be valid,
 * caller should erase this section first.
 * Write granularity = Page (TOKEN_EEPROM_PAGE_LEN)
 * Erase granularity = Sector (TOKEN_EEPROM_SECTOR_LEN)
 *
 * @param  > uint32_t : address to start writing to
 *         > uint8_t* : buffer to write from
 *         > uint32_t : length to write
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
TOKEN_ErrCode_t TokenEeprom_Write(uint32_t startAddress, uint8_t* buf, uint32_t len)
{
    TOKEN_ErrCode_t err = TOKEN_ERR_INVALID_INPUT;
    if(tokenEeprom_isValidAddress(startAddress + len - 1))
    {
        err = TOKEN_ERR_OK;
        uint32_t writeLen;
        uint32_t address = startAddress;
        uint32_t startTime = Timer_GetTick();
        while(len != 0 && err == TOKEN_ERR_OK)
        {
            if(Timer_TimeoutExpired(startTime, TOKEN_TIMEOUT_SMALL))
            {
                err = TOKEN_ERR_TIMEOUT;
                break;
            }
            // min (remainder in page, remainder in buffer)
            writeLen = MIN(TOKEN_EEPROM_PAGE_LEN - (address % TOKEN_EEPROM_PAGE_LEN), len);
            err = tokenEeprom_writePage(address, buf, writeLen);
            len -= writeLen;
            buf += writeLen;
            address += writeLen;
        }
    }
    return err;
}

/*******************************************************************************
 * @brief TokenEeprom_Read
 *
 * Read from Token
 *
 * @param  > uint32_t : address to start reading from
 *         > uint8_t* : buffer to read into
 *         > uint32_t : length to read
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
TOKEN_ErrCode_t TokenEeprom_Read(uint32_t address, uint8_t* buf, uint32_t len)
{
    TOKEN_ErrCode_t err = TOKEN_ERR_TIMEOUT;
    if(Token_WaitUntilReady())
    {
        uint8_t instruction[TOKEN_EEPROM_INSTRUCTION_SIZE];
        tokenEeprom_getInstruction(instruction, address, TOKEN_OPCODE_READ);
        err = (TOKEN_ErrCode_t) SPI_writeRead(TOKEN_SPI_PORT, instruction, sizeof(instruction), buf, len);
    }
    return err;
}

/*******************************************************************************
 * @brief TokenEeprom_ProtectRegion
 *
 * Protect a given region of EEPROM token. This will protect the highest region.
 * So if TOKEN_EEPROM_PROTECT_QUARTER is passed, only the highest quarter of
 * memory will be protected.
 * Where lowest address = 0, highest address = memSize - 1
 *
 * @param  > TOKEN_EepromProtect_t : region to protect
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
TOKEN_ErrCode_t TokenEeprom_ProtectRegion(TOKEN_EepromProtect_t region)
{
    TOKEN_ErrCode_t err = TOKEN_ERR_OK;
    Token_WaitUntilReady();
    if(region < TOKEN_EEPROM_PROTECT_COUNT)
    {        
        uint8_t sr = Token_ReadStatusRegister() & TOKEN_PROTECT_ANTIMASK;
        sr |= (region << TOKEN_PROTECT_OFFSET);
        Token_WriteStatusRegister(sr);
    }
    else
    {
        err = TOKEN_ERR_INVALID_INPUT;
    }
    Token_WaitUntilReady();
    return err;
}

/*******************************************************************************
 * @brief TokenEeprom_GetProtectedRegion
 *
 * Get region protected. Protected area is last/highest of returned region. So 
 * if TOKEN_EEPROM_PROTECT_QUARTER is returned, the last quarter of memory will
 * be protected
 *
 * @param  > TOKEN_EepromProtect_t* : protected region
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
TOKEN_EepromProtect_t TokenEeprom_GetProtectedRegion(void)
{
    Token_WaitUntilReady();
    TOKEN_EepromProtect_t region = (TOKEN_EepromProtect_t) ((Token_ReadStatusRegister() >> TOKEN_PROTECT_OFFSET) & 0x03);
    return region;
}


/*******************************************************************************
 * Private Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief tokenEeprom_getInstruction
 *
 * Get the formatted instruction w/ opcode and address. This transaction must be
 * sent MSB/MSb first where Opcode is the MSB of the 4 byte transaction
 *
 * @param  > uint8_t* : instruction
 *         > uint32_t : address
 *         > TOKEN_Opcode_t : opcode
 *
 * @return None
 ******************************************************************************/
static void tokenEeprom_getInstruction(uint8_t* instruction, uint32_t address, TOKEN_Opcode_t opCode)
{
    uint8_t instr = ((address & 0x100) >> 5) | opCode; // weird, but ok
    uint8_t addr = address & 0xFF;
    instruction[0] = instr;
    instruction[1] = addr;
}

/*******************************************************************************
 * @brief tokenEeprom_writePage
 *
 * Write bufLen bytes from buf to given address of Eeprom Token
 *
 * @param  > uint32_t : address to start writing
 *         > uint8_t* : buffer to write
 *         > uint32_t : length to write
 *
 * @return None
 ******************************************************************************/
static TOKEN_ErrCode_t tokenEeprom_writePage(uint32_t address, uint8_t* buf, uint32_t bufLen)
{
    TOKEN_ErrCode_t err = TOKEN_ERR_TIMEOUT;
    if(Token_WaitUntilReady())
    {
        Token_WriteEnable();
        uint8_t instruction[TOKEN_EEPROM_INSTRUCTION_SIZE];
        tokenEeprom_getInstruction(instruction, address, TOKEN_OPCODE_WRITE);
        err = (TOKEN_ErrCode_t) SPI_write2(TOKEN_SPI_PORT, instruction, sizeof(instruction), buf, bufLen);
    }
    return err;
}

/*******************************************************************************
 * @brief tokenEeprom_isValidAddress
 *
 * Determines if address is valid in memory
 *
 * @param  > uint32_t : address
 *
 * @return bool: true if address is in memory, false otherwise
 ******************************************************************************/
static bool tokenEeprom_isValidAddress(uint32_t address)
{
    return address < TOKEN_EEPROM_MEM_SIZE;
}

// EOF
