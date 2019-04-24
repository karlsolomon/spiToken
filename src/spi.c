/*******************************************************************************
 *  @file SPI.c
 *
 *  @brief  SPI Low Level Driver Interface. See SPI_initialize for configuration
 *  options(e.g. Slave/Master, Port speed, CPOL, etc.)
 *
 *  @author KSolomon
 *  @date Oct 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/


/******************************************************************************
 * Include Section
 ******************************************************************************/

// System Includes
#include <wiringPi.h>
#include "TypeDefs.h"
#include <string.h>

// Module Includes
#include <wiringPiSPI.h>

// Utility Includes

// Driver Includes
#include "spi.h"


/*******************************************************************************
 * Constants Declarations
 ******************************************************************************/

// send this from Master whenever you are trying to read, but don't have
// anything to write
#define SPI_DUMMY_VALUE             0xA5
#define SPI_CLOCK_SPEED_HZ          17000000

#define SPI_BAD_CONNECTION_FD       ((int) -1)
#define TMP_WRITE_BUF_SIZE          256

static uint8_t tmpWriteBuf[TMP_WRITE_BUF_SIZE]; 

/*******************************************************************************
 * Data Types Declarations
 ******************************************************************************/


/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

// Enable Slave
static void spi_select(void);

// Disable Slave
static void spi_deselect(void);

// Write Buffer to SPI
static SPI_ErrCode_t spi_writeBuf(uint8_t* buf, uint32_t len);

// Read Buffer from SPI
static SPI_ErrCode_t spi_readBuf(uint8_t* buf, uint32_t len);


/*******************************************************************************
 * Public Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief SPI_Init
 *
 * Initialized the SPI Port
 *
 * @param   > None
 *
 * @return None
 *
 ******************************************************************************/
void SPI_Init(void)
{
    int fd = wiringPiSPISetup(SPI_CHANNEL, SPI_CLOCK_SPEED_HZ);
}

/*******************************************************************************
 * @brief SPI_Write
 *
 * Writes len bytes from buf to the SPI slave. This will OVERWRITE the given
 * buffer since RPi uses full-duplex SPI transactions
 *
 * @param   > uint8_t*: buffer of data to write
 *          > uint32_t: number of bytes to write
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_Write(uint8_t* buf, uint32_t len)
{
    SPI_ErrCode_t err = SPI_ERR_INVALID_INPUT;
    if((buf != NULL) && (len > 0))
    {
        err = SPI_ERR_OK;
        spi_select();
        err = spi_writeBuf(buf, len);
        spi_deselect();
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_Write2
 *
 * Writes 2 bufs to the SPI slave.
 *
 * @param   > uint8_t*: buffer1 of data to write
 *          > uint32_t: number1 of bytes to write
 *          > uint8_t*: buffer2 of data to write
 *          > uint32_t: number2 of bytes to write
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_Write2(uint8_t* buf1, uint32_t len1, \
    uint8_t* buf2, uint32_t len2)
{
    SPI_ErrCode_t err = SPI_ERR_INVALID_INPUT;
    if((buf1 != NULL) && (len1 > 0) && (buf2 != NULL) && (len2 > 0))
    {
        err = SPI_ERR_OK;
        spi_select();
        err = spi_writeBuf(buf1, len1);
        if(err == SPI_ERR_OK)
        {
            err = spi_writeBuf(buf2, len2);
        }
        spi_deselect();
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_Write3
 *
 * Writes 3 bufs to the SPI slave.
 *
 * @param   > uint8_t*: buffer1 of data to write
 *          > uint32_t: number1 of bytes to write
 *          > uint8_t*: buffer2 of data to write
 *          > uint32_t: number2 of bytes to write
 *          > uint8_t*: buffer3 of data to write
 *          > uint32_t: number3 of bytes to write
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_Write3(uint8_t* buf1, uint32_t len1, \
    uint8_t* buf2, uint32_t len2, uint8_t* buf3, uint32_t len3)
{
    SPI_ErrCode_t err = SPI_ERR_INVALID_INPUT;
    if((buf1 != NULL) && (len1 > 0) && (buf2 != NULL) && (len2 > 0))
    {
        err = SPI_ERR_OK;
        spi_select();
        err = spi_writeBuf(buf1, len1);
        if(err == SPI_ERR_OK)
        {
            err = spi_writeBuf(buf2, len2);
        }
        if(err == SPI_ERR_OK)
        {
            err = spi_writeBuf(buf3, len3);
        }
        spi_deselect();
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_Read
 *
 * Reads len bytes into buf from the SPI slave.
 *
 * @param   > uint8_t*: buffer to read in to
 *          > uint32_t: number of bytes to read
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_Read(uint8_t* buf, uint32_t len)
{
    SPI_ErrCode_t err = SPI_ERR_INVALID_INPUT;
    if((buf != NULL) && (len > 0))
    {
        err = SPI_ERR_OK;
        spi_select();
        err = spi_readBuf(buf, len);
        spi_deselect();
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_Read2
 *
 * Reads in 2 buffers
 *
 * @param   > uint8_t*: buffer1 to read in to
 *          > uint32_t: number1 of bytes to read
 *          > uint8_t*: buffer2 to read in to
 *          > uint32_t: number2 of bytes to read
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_Read2(uint8_t* buf1, uint32_t len1, \
    uint8_t* buf2, uint32_t len2)
{
    SPI_ErrCode_t err = SPI_ERR_INVALID_INPUT;
    if((buf1 != NULL) && (len1 > 0) && (buf2 != NULL) && (len2 > 0))
    {
        err = SPI_ERR_OK;
        spi_select();
        err = spi_readBuf(buf1, len1);
        if(err == SPI_ERR_OK)
        {
            err = spi_readBuf(buf2, len2);
        }
        spi_deselect();
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_WriteRead
 *
 * Reads in 2 buffers
 *
 * @param   > uint8_t*: buffer1 to read in to
 *          > uint32_t: number1 of bytes to read
 *          > uint8_t*: buffer2 to read in to
 *          > uint32_t: number2 of bytes to read
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_WriteRead(uint8_t* bufWrite, \
    uint32_t lenWrite, uint8_t* bufRead, uint32_t lenRead)
{
    SPI_ErrCode_t err = SPI_ERR_OK;
    if((bufWrite != NULL) && (lenWrite > 0) && (bufRead != NULL) && (lenRead > 0))
    {
        err = SPI_ERR_OK;
        spi_select();
        err = spi_writeBuf(bufWrite, lenWrite);
        if(err == SPI_ERR_OK)
        {
            err = spi_readBuf(bufRead, lenRead);
        }
        spi_deselect();
    }
    else
    {
        err = SPI_ERR_INVALID_INPUT;
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_Write2Read
 *
 * Reads in 2 buffers
 *
 * @param   > uint8_t*: buffer1 to read in to
 *          > uint32_t: number1 of bytes to read
 *          > uint8_t*: buffer2 to read in to
 *          > uint32_t: number2 of bytes to read
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_Write2Read(uint8_t* bufWrite1,\
    uint32_t lenWrite1, uint8_t* bufWrite2, uint32_t lenWrite2, uint8_t* bufRead, uint32_t lenRead)
{
    SPI_ErrCode_t err = SPI_ERR_OK;
    if((bufWrite1 != NULL) && (lenWrite1 > 0) && (bufWrite2 != NULL) && (lenWrite2 > 0) && (bufRead != NULL) && (lenRead > 0))
    {
        err = SPI_ERR_OK;
        spi_select();
        err = spi_writeBuf(bufWrite1, lenWrite1);
        if(err == SPI_ERR_OK)
        {
            err = spi_writeBuf(bufWrite2, lenWrite2);
        }
        if(err == SPI_ERR_OK)
        {
            err = spi_readBuf(bufRead, lenRead);
        }
        spi_deselect();
    }
    else
    {
        err = SPI_ERR_INVALID_INPUT;
    }
    return err;
}


/*******************************************************************************
 * Private Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief SPI_select
 *
 * Enable Slave
 *
 * @param   > None
 *
 * @return None
 *
 ******************************************************************************/
static void spi_select(void)
{
    digitalWrite(SPI_CS_PIN, 0);
}

/*******************************************************************************
 * @brief SPI_select
 *
 * Disable Slave
 *
 * @param   > None
 *
 * @return None
 *
 ******************************************************************************/
static void spi_deselect(void)
{
    digitalWrite(SPI_CS_PIN, 1);
}

/*******************************************************************************
 * @brief SPI_WriteBuf
 *
 * Write Buffer to SPI
 *
 * @param   > uint8_t*: buffer to write from
 *          > uint32_t: number of bytes to write
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
static SPI_ErrCode_t spi_writeBuf(uint8_t* buf, uint32_t len)
{
    SPI_ErrCode_t err = SPI_ERR_OK;
    uint32_t i = 0;
    int fd = 0;
    uint32_t currentLen = 0;
    while(len > 0)
    {
	currentLen = MIN(TMP_WRITE_BUF_SIZE, len);
	memcpy(tmpWriteBuf, buf + i, currentLen);
	len -= currentLen;
	i += currentLen;
	fd = wiringPiSPIDataRW(SPI_CHANNEL, tmpWriteBuf, (int) currentLen);
        if(fd == SPI_BAD_CONNECTION_FD)
        {
            err = SPI_ERR_GENERAL;
        }
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_ReadBuf
 *
 * Read Buffer from SPI
 *
 * @param   > uint8_t*: buffer to read into
 *          > uint32_t: number of bytes to read
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
static SPI_ErrCode_t spi_readBuf(uint8_t* buf, uint32_t len)
{
    SPI_ErrCode_t err = SPI_ERR_OK;
    int fd = wiringPiSPIDataRW(SPI_CHANNEL, buf, (int) len);
    if(fd == SPI_BAD_CONNECTION_FD)
    {
        err = SPI_ERR_GENERAL;
    }
    return err;
}
