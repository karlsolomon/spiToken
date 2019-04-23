/*******************************************************************************
 *  @file SPI.h
 *
 *  @brief  SPI Low Level Driver Interface. See SPI_initialize for configuration
 *  options(e.g. Slave/Master, Port speed, CPOL, etc.)
 *
 *  @author KSolomon
 *  @date Apr 2019
 *  @copyright 2019 Stryker Corporation. All rights reserved.
 ******************************************************************************/

#ifndef __SPI_H__
#define __SPI_H__


/*******************************************************************************
 * Includes
 ******************************************************************************/

// System Includes
#include <stdint.h>

// Module Includes

// Utility Includes

// Driver Includes


/*******************************************************************************
 * Macros
 ******************************************************************************/

#define SPI_CHANNEL 0


/*******************************************************************************
 * Public Declarations
 ******************************************************************************/

typedef enum {
    SPI_ERR_OK = 0,
    SPI_ERR_GENERAL,
    SPI_ERR_TIMEOUT,
    SPI_ERR_INVALID_INPUT,
    SPI_ERR_COUNT
} SPI_ErrCode_t;

// Initialized the SPI Port
void SPI_Init(void);

// Writes len bytes from buf to the SPI slave.
// In Master mode this will trigger a transaction w/ the connected slave
// In Slave mode this will simply populate a ring buffer in preparation for the
// next time the master initiates a transaction
SPI_ErrCode_t SPI_Write(uint8_t* buf, uint32_t len);

// Write 2 Buffers in 1 Transaction
SPI_ErrCode_t SPI_Write2(uint8_t* buf1, uint32_t len1, uint8_t* buf2, uint32_t len2);

// Reads len bytes into buf from the SPI slave.
// In Master mode this will trigger a transaction w/ the connected slave.
// This happens by writing len inconsequential (dummy) bytes to the slave
// In Slave mode this will simply de-populate a ring buffer with the assumption
// that a master has written into that ring buffer.
SPI_ErrCode_t SPI_Read(uint8_t* buf, uint32_t len);

// Read 2 Buffers in 1 Transaction
SPI_ErrCode_t SPI_Read2(uint8_t* buf1, uint32_t len1, uint8_t* buf2, uint32_t len2);

// Write first buffer then read second buffer in one SPI transaction
SPI_ErrCode_t SPI_WriteRead(uint8_t* bufWrite, uint32_t lenWrite, uint8_t* bufRead, uint32_t lenRead);

// Write first 2 buffers then read third buffer in one SPI transaction
SPI_ErrCode_t SPI_Write2Read(uint8_t* bufWrite1, uint32_t lenWrite1, uint8_t* bufWrite2, uint32_t lenWrite2, uint8_t* bufRead, uint32_t lenRead);

// Write 3 Buffers in 1 Transaction
SPI_ErrCode_t SPI_Write3(uint8_t* buf1, uint32_t len1, uint8_t* buf2, uint32_t len2, uint8_t* buf3, uint32_t len3);

#endif // __SPI_H__
