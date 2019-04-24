/*******************************************************************************
 *  @file test.h
 *
 *  @brief Low Level I2C Driver
 *
 *  @author KSolomon
 *  @date Nov 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/

#ifndef _TEST_H_
#define _TEST_H_


/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "TypeDefs.h"


/*******************************************************************************
 * Macros
 ******************************************************************************/

typedef enum
{
    TEST_ERR_NONE,
    TEST_PASSED = TEST_ERR_NONE,
    TEST_ERR_GENERAL,
    TEST_FAILED = TEST_ERR_GENERAL,
    TEST_ERR_COUNT
} TEST_ErrCode_t;


#define TEST_DEBUG_FULL 1


/*******************************************************************************
 * Public Declarations
 ******************************************************************************/

// Type to allow a generic
typedef uint8_t (*WriteAndVerifyHook)(uint32_t, uint8_t*, uint32_t);
typedef uint8_t (*EraseHook)(uint32_t, uint32_t);
typedef uint8_t (*EraseChipHook)();

// Wrapper to Write and Verify a peripheral's memory
bool Test_WriteAndVerify(WriteAndVerifyHook write, WriteAndVerifyHook read, uint32_t addr, uint32_t len);

// Wrapper to Read & Verify a peripheral's memory
bool Test_Verify(WriteAndVerifyHook read, uint32_t addr, uint8_t* expectedBuf, uint32_t bufLen, uint32_t len);

// Wrapper to Read & Verify a peripheral's memory
bool Test_VerifyErased(WriteAndVerifyHook read, uint32_t addr, uint32_t len);

#endif /* _TEST_H_ */
