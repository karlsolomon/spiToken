/*******************************************************************************
 *  @file TestToken.h
 *
 *  @brief  header file to run SPI API functional tests
 *
 *  @author KSolomon
 *  @date Nov 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/

#ifndef _TEST_TOKEN_H_
#define _TEST_TOKEN_H_


/*******************************************************************************
 * Includes
 ******************************************************************************/


/*******************************************************************************
 * Macros
 ******************************************************************************/


/*******************************************************************************
 * Debug defines
 ******************************************************************************/


/*******************************************************************************
 * Public Declarations
 ******************************************************************************/

// Run Functional Test & Debugging for TOKEN. Setup such that tests run for 
// either EEPROM/Flash token pending on which is inserted.
int TestToken_Menu(void);

// Runs all TOKEN Tests.
int TestToken_RunAll(void);

// Runs functional test
int TestToken_FunctionalTest(void);

#endif /* _TEST_TOKEN_H_ */
