/*******************************************************************************
 *  @file TokenFlash.c
 *
 *  @brief Utils For Flash Tokens
 *
 *  @author KSolomon
 *  @date Nov 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/


/******************************************************************************
 * Include Section
 ******************************************************************************/

// System Includes
#include "TypeDefs.h"

// Module Includes
#include "TokenFlash.h"
#include "Token.h"

// Utility Includes

// Driver Includes
#include "spi.h"


/*******************************************************************************
 * Constants Declarations
 ******************************************************************************/

#define TOKEN_FLASH_INSTRUCTION_SIZE    4


/*******************************************************************************
 * Data Types Declarations
 ******************************************************************************/


/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

// Erase Sector, this is smallest resolution of erase
static TOKEN_ErrCode_t tokenFlash_eraseSector(uint32_t address);

// Get the formatted instruction w/ opcode and address. This transaction must be
// sent MSB/MSb first where Opcode is the MSB of the 4 byte transaction
static void tokenFlash_getInstruction(uint8_t* instruction, uint32_t address, TOKEN_Opcode_t opCode);

// Write bufLen bytes from buf to given address of Flash Token
static TOKEN_ErrCode_t tokenFlash_writePage(uint32_t address, uint8_t* buf, uint32_t bufLen);

// Determines if address is valid in memory
static bool tokenFlash_isValidAddress(uint32_t address);


/*******************************************************************************
 * Public Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief TokenFlash_Erase
 *
 * Erase Token - sets all bytes to 0xFF
 * Erase granularity = Sector (TOKEN_FLASH_SECTOR_LEN)
 * This will erase whole sectors (incl. below given address if it isn't sector start)
 *
 * @param  > uint32_t : address to start erasing
 *         > uint32_t : length to write
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
TOKEN_ErrCode_t TokenFlash_Erase(uint32_t address, uint32_t len)
{
    TOKEN_ErrCode_t err = TOKEN_ERR_INVALID_INPUT;
    if(tokenFlash_isValidAddress(address + len - 1))
    {
        err = TOKEN_ERR_OK;
        uint32_t end = address + len;
        while(address < end && err == TOKEN_ERR_OK)
        {
            err = tokenFlash_eraseSector(address);
            address += TOKEN_FLASH_SECTOR_LEN;
        }
    }
    return err;
}

/*******************************************************************************
 * @brief TokenFlash_EraseAll
 *
 * Erase entire Flash Token
 *
 * @param  > None
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
TOKEN_ErrCode_t TokenFlash_EraseAll(void)
{
    TOKEN_ErrCode_t err = Token_WriteEnable();
    uint8_t opCode = TOKEN_OPCODE_FLASH_CHIP_ERASE;
    err = (TOKEN_ErrCode_t) SPI_write(&opCode, sizeof(uint8_t));
    return err;
}

/*******************************************************************************
 * @brief TokenFlash_EraseAllBlocking
 *
 * Erase entire Flash Token (blocking)
 *
 * @param  > None
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
TOKEN_ErrCode_t TokenFlash_EraseAllBlocking(void)
{
    TOKEN_ErrCode_t err = TokenFlash_EraseAll();
    if (err == TOKEN_ERR_OK)
    {
        if (Token_WaitUntilReady_time(TOKEN_FLASH_ERASE_ALL_TIME))
        {
            err = TOKEN_ERR_OK;
        }
        else
        {
            err = TOKEN_ERR_TIMEOUT;
        }
    }
    return err;
}

/*******************************************************************************
 * @brief TokenFlash_Write
 *
 * Write to Token. Can only program (write) 0s. Thus, for data to be valid,
 * caller should erase this section first.
 * Write granularity = Page (TOKEN_FLASH_PAGE_LEN)
 * Erase granularity = Sector (TOKEN_FLASH_SECTOR_LEN)
 *
 * @param  > uint32_t : address to start writing to
 *         > uint8_t* : buffer to write from
 *         > uint32_t : length to write
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
TOKEN_ErrCode_t TokenFlash_Write(uint32_t startAddress, uint8_t* buf, uint32_t len)
{
    uint32_t writeLen;
    uint32_t address = startAddress;
    uint32_t startTime = Timer_GetTick();
    TOKEN_ErrCode_t err = TOKEN_ERR_INVALID_INPUT;

    if(tokenFlash_isValidAddress(startAddress + len - 1) && (buf != NULL))
    {
        err = TOKEN_ERR_OK;
        while(len != 0 && err == TOKEN_ERR_OK)
        {
            if(Timer_TimeoutExpired(startTime, TOKEN_TIMEOUT_LARGE))
            {
                err = TOKEN_ERR_TIMEOUT;
                break;
            }

            // min(remainder in page, remainder in buffer)
            writeLen = MIN(TOKEN_FLASH_PAGE_LEN - (address % TOKEN_FLASH_PAGE_LEN), len);
            err = tokenFlash_writePage(address, buf, writeLen);
            len -= writeLen;
            buf += writeLen;
            address += writeLen;
        }
    }
    return err;
}

/*******************************************************************************
 * @brief TokenFlash_Read
 *
 * Read from Token
 *
 * @param  > uint32_t : address to start reading from
 *         > uint8_t* : buffer to read into
 *         > uint32_t : length to read
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
TOKEN_ErrCode_t TokenFlash_Read(uint32_t address, uint8_t* buf, uint32_t len)
{
    TOKEN_ErrCode_t err = TOKEN_ERR_INVALID_INPUT;
    if(tokenFlash_isValidAddress(address + len - 1) && (buf != NULL))
    {
        err = TOKEN_ERR_OK;
        if(Token_WaitUntilReady())
        {
            uint8_t instruction[TOKEN_FLASH_INSTRUCTION_SIZE];
            tokenFlash_getInstruction(instruction, address, TOKEN_OPCODE_READ);
            err = (TOKEN_ErrCode_t) SPI_writeRead(instruction, TOKEN_FLASH_INSTRUCTION_SIZE, buf, len);
        }
        else
        {
            err = TOKEN_ERR_TIMEOUT;
        }
    }
    return err;
}

/*******************************************************************************
 * @brief TokenFlash_ProtectRegion
 *
 * Protect a given region of FLASH token. This will protect the highest region.
 * So if TOKEN_FLASH_PROTECT_QUARTER is passed, only the highest quarter of
 * memory will be protected.
 * Where lowest address = 0, highest address = memSize - 1
 *
 * @param  > TOKEN_FlashProtect_t : region to protect
 *
 * @return None
 ******************************************************************************/
TOKEN_ErrCode_t TokenFlash_ProtectRegion(TOKEN_FlashProtect_t region)
{
    Token_WaitUntilReady();
    TOKEN_ErrCode_t err = TOKEN_ERR_OK;
    if(region < TOKEN_FLASH_PROTECT_COUNT)
    {
        uint8_t sr = Token_ReadStatusRegister() & TOKEN_PROTECT_ANTIMASK;
        sr |= (region << TOKEN_PROTECT_OFFSET);
        Token_WriteStatusRegister(sr);
    }
    else
    {
        err = TOKEN_ERR_INVALID_INPUT;
    }
    return err;
}

/*******************************************************************************
 * @brief TokenFlash_GetProtectedRegion
 *
 * Get protected region
 *
 * @param  > TOKEN_FlashProtect_t* : protected region
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
TOKEN_FlashProtect_t TokenFlash_GetProtectedRegion(void)
{
    Token_WaitUntilReady();
    TOKEN_FlashProtect_t region = (TOKEN_FlashProtect_t) ((Token_ReadStatusRegister() >> TOKEN_PROTECT_OFFSET) & 0x07);
    return region;
}

/*******************************************************************************
 * @brief TokenFlash_GetDeviceSize
 *
 * Get Token Device Size
 *
 * @param  > uint32_t* : mem size (Mb)
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
TOKEN_ErrCode_t TokenFlash_GetDeviceSize(uint32_t* size)
{
    TOKEN_ErrCode_t err = TOKEN_ERR_OK;
    if(Token_WaitUntilReady())
    {
        uint8_t signature = 0;
        uint8_t instruction[TOKEN_FLASH_INSTRUCTION_SIZE];
        tokenFlash_getInstruction(instruction, 0, TOKEN_OPCODE_FLASH_READ_E_SIGNATURE);
        err = (TOKEN_ErrCode_t) SPI_writeRead(instruction, TOKEN_FLASH_INSTRUCTION_SIZE, &signature, sizeof(uint8_t));
        if(signature != 0)
        {
            signature &= 0x0F;
            *size = (1 << signature);
        }
    }
    else {
        err = TOKEN_ERR_TIMEOUT;
    }
    return err;
}


/*******************************************************************************
 * Private Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief tokenFlash_eraseSector
 *
 * Erase Sector, this is smallest resolution of erase
 *
 * @param  > uint32_t : address
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
static TOKEN_ErrCode_t tokenFlash_eraseSector(uint32_t address)
{
    TOKEN_ErrCode_t err = Token_WriteEnable();
    if(err == TOKEN_ERR_OK)
    {
        uint8_t instruction[TOKEN_FLASH_INSTRUCTION_SIZE];
        tokenFlash_getInstruction(instruction, address, TOKEN_OPCODE_FLASH_SECTOR_ERASE);
        err = (TOKEN_ErrCode_t) SPI_write(instruction, sizeof(uint32_t));
    }
    return err;
}

/*******************************************************************************
 * @brief tokenFlash_getInstruction
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
static void tokenFlash_getInstruction(uint8_t* instruction, uint32_t address, TOKEN_Opcode_t opCode)
{
    instruction[0] = (uint8_t) (opCode & 0xFF);
    instruction[1] = (uint8_t) (address >> 16) & 0xFF;
    instruction[2] = (uint8_t) (address >> 8) & 0xFF;
    instruction[3] = (uint8_t) (address & 0xFF);
}

/*******************************************************************************
 * @brief tokenFlash_writePage
 *
 * Write bufLen bytes from buf to given address of Flash Token
 *
 * @param  > uint32_t : address to start writing
 *         > uint8_t* : buffer to write
 *         > uint32_t : length to write
 *
 * @return TOKEN_ErrCode_t
 ******************************************************************************/
static TOKEN_ErrCode_t tokenFlash_writePage(uint32_t address, uint8_t* buf, uint32_t bufLen)
{
    TOKEN_ErrCode_t err = Token_WriteEnable();
    if(err == TOKEN_ERR_OK)
    {
        uint8_t instruction[TOKEN_FLASH_INSTRUCTION_SIZE];
        tokenFlash_getInstruction(instruction, address, TOKEN_OPCODE_WRITE);
        err = (TOKEN_ErrCode_t) SPI_write2(instruction, sizeof(instruction), buf, bufLen);
    }
    return err;
}

/*******************************************************************************
 * @brief tokenFlash_isValidAddress
 *
 * Determines if address is valid in memory
 *
 * @param  > uint32_t : address
 *
 * @return bool: true if address is in memory, false otherwise
 ******************************************************************************/
static bool tokenFlash_isValidAddress(uint32_t address)
{
    return address < TOKEN_FLASH_MEM_SIZE;
}


// EOF
