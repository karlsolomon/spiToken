/*******************************************************************************
 *  @file test.c
 *
 *  @brief c file to run pluto debug tests
 *
 *  @author KSolomon
 *  @date Sep 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/

/******************************************************************************
 * Include Section
 ******************************************************************************/

// System Includes
#include "TypeDefs.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// Module Includes
#include "test.h"
#include "TestToken.h"

// Utility Includes

// Driver Includes

// System includes


/*******************************************************************************
 * Constants Declarations
 ******************************************************************************/

#define TEST_MANUAL_TIMEOUT         Timer_10MIN
#define TEST_BUFFER_SIZE            256
#define TEST_RETRY_CNT              2

static uint8_t m_bufWrite[TEST_BUFFER_SIZE];
static uint8_t m_bufRead[TEST_BUFFER_SIZE];
static uint8_t m_bufErase[TEST_BUFFER_SIZE];

static bool m_isInitialized = false;

/*******************************************************************************
 * Data Types Declarations
 ******************************************************************************/


/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

// Initialize necessary test buffers
static void test_inits(void);


/*******************************************************************************
 * Public Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief Test_WriteAndVerify
 *
 * Wrapper to Write and Verify a peripheral's memory
 *
 * @param   > WriteAndVerifyHook: write function call
 *          > WriteAndVerifyHook: read function call
 *          > uint32_t: address to start from
 *          > uint32_t: length to write/verify. If > TEST_BUFFER_SIZE then we write the
 *                      same message repeatedly until len is written to
 *
 * @return bool: true if all steps passed, false otherwise
 *
 ******************************************************************************/
bool Test_WriteAndVerify(WriteAndVerifyHook write, WriteAndVerifyHook read, uint32_t addr, uint32_t len)
{
    uint32_t currentLen = 0;
    uint8_t writeResult = 0;
    uint8_t readResult = 0;
    bool passed = true;
    uint32_t startAddr = addr;
    if(!m_isInitialized)
    {
        test_inits();
    }

    while(len > 0)
    {
        currentLen = MIN(TEST_BUFFER_SIZE, len);
        writeResult = write(addr, m_bufWrite, currentLen);
        Timer_Sleep(10);
        if(writeResult)
        {
            passed = false;
            if(TEST_DEBUG_FULL)
            {
                printf("Failed Write at addr 0x%08X. Err code = %d\n", addr, writeResult);
            }
            break;
        }
        else
        {
            readResult = read(addr, m_bufRead, currentLen);
            if(readResult)
            {
                passed = false;
                if(TEST_DEBUG_FULL)
                {
                    printf("Failed Read at addr 0x%08X. Err code = %d\n", addr, readResult);
                }
                break;
            }
            else if(memcmp(m_bufWrite, m_bufRead, currentLen))
            {
                passed = false;
                if(TEST_DEBUG_FULL)
                {
                    printf("Failed Verify Write at addr 0x%08X\n", addr);
                }
                break;
            }
            else
            {
                len -= currentLen;
                addr += currentLen;
            }
        }
    }
    if(passed)
    {
	printf("passed write & verify from 0x%08X to 0x%08X\n", startAddr, addr);
    }
    return passed;
}

/*******************************************************************************
 * @brief Test_Verify
 *
 * Wrapper to Read & Verify a peripheral's memory
 *
 * @param   > WriteAndVerifyHook: read function call
 *          > uint32_t: address to start from
 *          > uint8_t*: buffer of expected data
 *          > uint8_t*: buffer to read into
 *          > uint32_t: length of buffer (assumed same)
 *          > uint32_t: length to write/verify. If > bufLen then we write the
 *                      same message repeatedly until len is written to
 *
 * @return bool: true if all steps passed, false otherwise
 *
 ******************************************************************************/
bool Test_Verify(WriteAndVerifyHook read, uint32_t addr, uint8_t* expectedBuf, uint32_t bufLen, uint32_t len)
{
    uint32_t currentLen = 0;
    uint8_t readResult = 0;
    bool passed = true;
    uint32_t startAddr = addr;
    while(len > 0)
    {
        currentLen = MIN(bufLen, len);
        readResult = read(addr, m_bufRead, currentLen);
        Timer_Sleep(10);
        if(readResult)
        {
            passed = false;
            if(TEST_DEBUG_FULL)
            {
                printf("\tFailed Read at addr 0x%08X. Err code = %d\n", addr, readResult);
            }
            break;
        }
        else if(memcmp(expectedBuf, m_bufRead, currentLen))
        {
            passed = false;
            if(TEST_DEBUG_FULL)
            {
                printf("Failed Verify at addr 0x%08X\n", addr);
            }
            break;
        }
        else
        {
            len -= currentLen;
            addr += currentLen;
        }
    }
    if(passed)
    {
        printf("passed verify from 0x%08X to 0x%08X\n", startAddr, addr);
    }
    return passed;
}

/*******************************************************************************
 * @brief Test_Verify
 *
 * Wrapper to Read & Verify a peripheral's memory
 *
 * @param   > WriteAndVerifyHook: read function call
 *          > uint32_t: address to start from
 *          > uint8_t*: buffer of expected data
 *          > uint8_t*: buffer to read into
 *          > uint32_t: length of buffer (assumed same)
 *          > uint32_t: length to write/verify. If > TEST_BUFFER_SIZE then we write the
 *                      same message repeatedly until len is written to
 *
 * @return bool: true if all steps passed, false otherwise
 *
 ******************************************************************************/
bool Test_VerifyErased(WriteAndVerifyHook read, uint32_t addr, uint32_t len)
{
    uint32_t currentLen = 0;
    uint8_t readResult = 0;
    bool passed = true;
    uint32_t startAddr = addr;

    if(!m_isInitialized)
    {
        test_inits();
    }

    while(len > 0)
    {
        currentLen = MIN(TEST_BUFFER_SIZE, len);
        readResult = read(addr, m_bufRead, currentLen);
        Timer_Sleep(10);
        if(readResult)
        {
            passed = false;
            if(TEST_DEBUG_FULL)
            {
                printf("Failed Read at addr 0x%08X. Err code = %d\n", addr, readResult);
            }
            break;
        }
        else if(memcmp(m_bufErase, m_bufRead, currentLen))
        {
            passed = false;
            if(TEST_DEBUG_FULL)
            {
                printf("Failed Verify Erase at addr 0x%08X\n", addr);
            }
            break;
        }
        else
        {
            len -= currentLen;
            addr += currentLen;
        }
    }
    if(passed)
    {
        printf("passed verifyErase from 0x%08X to 0x%08X\n", startAddr, addr);
    }
    return passed;
}


/*******************************************************************************
 * @brief Test_ReadAndPrint
 *
 * Wrapper to Read and Print a peripheral's memory
 *
 * @param   > WriteAndVerifyHook: read function call
 *          > uint32_t: address to start from
 *          > uint8_t*: buffer to read into
 *          > uint32_t: length of buffer
 *          > uint32_t: length to read/print
 *
 * @return bool: true if read passed, false otherwise
 *
 ******************************************************************************/
bool Test_ReadAndPrint(WriteAndVerifyHook read, uint32_t addr, uint32_t len)
{
    bool passed = true;
    uint32_t currentLen = 0;
    uint8_t readFailed = 0;
    uint32_t startAddr = addr;
    while(len > 0)
    {
        currentLen = MIN(TEST_BUFFER_SIZE, len);
        readFailed = read(addr, m_bufRead, currentLen);
        Timer_Sleep(10);
        if(readFailed)
        {
            passed = false;
            if(TEST_DEBUG_FULL)
            {
                printf("Failed Read at addr 0x%08X. Err code = %d\n", addr, readFailed);
            }
            break;
        }
        else
        {
            printf("Addr = 0x%08X. Len = 0x%08X\n", addr, currentLen);
            for(uint32_t i = 0; i < currentLen; i++)
            {
                printf("%02X ", m_bufRead[i]);
            }
            printf("\n");
            len -= currentLen;
            addr += currentLen;
        }
    }
    if(passed)
    {
        printf("passed readAndPrint from 0x%08X to 0x%08X\n", startAddr, addr);
    }
    return passed;
}

/*******************************************************************************
 * @brief test_inits
 *
 * Initialize necessary test buffers
 *
 * @param   > None
 *
 * @return  None
 *
 ******************************************************************************/
static void test_inits(void)
{
    for(uint32_t i = 0; i < TEST_BUFFER_SIZE; i++)
    {
        m_bufWrite[i] = i % 0x100;
        m_bufErase[i] = 0xFF;
    }
    m_isInitialized = true;
}

// EOF

