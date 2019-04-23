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
#define SPI_CLOCK_SPEED_HZ          500000
#define SPI_CS_PIN                  20

#define SPI_BAD_CONNECTION_FD       ((int) -1)


/*******************************************************************************
 * Data Types Declarations
 ******************************************************************************/

static SPI_TypeDef* spi_ports[SPI_PORT_COUNT];


/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

// Convert SPI port to SPI typedef
static SPI_TypeDef* spi_portToSPI(SPI_Port_t port);

// Enable Slave
static void spi_select(SPI_Port_t port);

// Disable Slave
static void spi_deselect(SPI_Port_t port);

// Write Buffer to SPI
static SPI_ErrCode_t spi_writeBuf(SPI_Port_t port, uint8_t* buf, uint32_t len);

// Read Buffer from SPI
static SPI_ErrCode_t spi_readBuf(SPI_Port_t port, uint8_t* buf, uint32_t len);


/*******************************************************************************
 * Public Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief SPI_Init
 *
 * Initialized the SPI Port given. Internally specifies if this is a slave or
 * a master port
 *
 * @param   > SPI_PORT: port to initialize
 *
 * @return None
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_Init(SPI_Port_t port)
{
    SPI_ErrCode_t err = SPI_ERR_OK;
    int fd = wiringPiSPISetup(SPI_PORT_1, SPI_CLOCK_SPEED_HZ);
    if(fd == SPI_BAD_CONNECTION_FD)
    {
        err = SPI_ERR_OK;
    }
    else
    {
        pinMode(SPI_CS_PIN, OUTPUT);
        pullUpDnControl(SPI_CS_PIN, PUD_UP);
    }
}

/*******************************************************************************
 * @brief SPI_Write
 *
 * Writes len bytes from buf to the SPI slave. This will OVERWRITE the given
 * buffer since RPi uses full-duplex SPI transactions
 *
 * @param   > SPI_PORT: port to write to
 *          > uint8_t*: buffer of data to write
 *          > uint32_t: number of bytes to write
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_Write(SPI_Port_t port, uint8_t* buf, uint32_t len)
{
    SPI_ErrCode_t err = SPI_ERR_INVALID_INPUT;
    if((buf != NULL) && (len > 0))
    {
        err = SPI_ERR_OK;
        spi_select(port);
        err = spi_writeBuf(port, buf, len);
        spi_deselect(port);
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_Write2
 *
 * Writes 2 bufs to the SPI slave.
 *
 * @param   > SPI_PORT: port to write to
 *          > uint8_t*: buffer1 of data to write
 *          > uint32_t: number1 of bytes to write
 *          > uint8_t*: buffer2 of data to write
 *          > uint32_t: number2 of bytes to write
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_Write2(SPI_Port_t port, uint8_t* buf1, uint32_t len1, \
    uint8_t* buf2, uint32_t len2)
{
    SPI_ErrCode_t err = SPI_ERR_INVALID_INPUT;
    if((buf1 != NULL) && (len1 > 0) && (buf2 != NULL) && (len2 > 0))
    {
        err = SPI_ERR_OK;
        spi_select(port);
        err = spi_writeBuf(port, buf1, len1);
        if(err == SPI_ERR_OK)
        {
            err = spi_writeBuf(port, buf2, len2);
        }
        spi_deselect(port);
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_write3
 *
 * Writes 3 bufs to the SPI slave.
 *
 * @param   > SPI_PORT: port to write to
 *          > uint8_t*: buffer1 of data to write
 *          > uint32_t: number1 of bytes to write
 *          > uint8_t*: buffer2 of data to write
 *          > uint32_t: number2 of bytes to write
 *          > uint8_t*: buffer3 of data to write
 *          > uint32_t: number3 of bytes to write
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_write3(SPI_Port_t port, uint8_t* buf1, uint32_t len1, \
    uint8_t* buf2, uint32_t len2, uint8_t* buf3, uint32_t len3)
{
    SPI_ErrCode_t err = SPI_ERR_INVALID_INPUT;
    if((buf1 != NULL) && (len1 > 0) && (buf2 != NULL) && (len2 > 0))
    {
        err = SPI_ERR_OK;
        spi_select(port);
        err = spi_writeBuf(port, buf1, len1);
        if(err == SPI_ERR_OK)
        {
            err = spi_writeBuf(port, buf2, len2);
        }
        if(err == SPI_ERR_OK)
        {
            err = spi_writeBuf(port, buf3, len3);
        }
        spi_deselect(port);
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_read
 *
 * Reads len bytes into buf from the SPI slave.
 *
 * @param   > SPI_PORT: port to read
 *          > uint8_t*: buffer to read in to
 *          > uint32_t: number of bytes to read
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_read(SPI_Port_t port, uint8_t* buf, uint32_t len)
{
    SPI_ErrCode_t err = SPI_ERR_INVALID_INPUT;
    if((buf != NULL) && (len > 0))
    {
        err = SPI_ERR_OK;
        spi_select(port);
        err = spi_readBuf(port, buf, len);
        spi_deselect(port);
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_read2
 *
 * Reads in 2 buffers
 *
 * @param   > SPI_PORT: port to read
 *          > uint8_t*: buffer1 to read in to
 *          > uint32_t: number1 of bytes to read
 *          > uint8_t*: buffer2 to read in to
 *          > uint32_t: number2 of bytes to read
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_read2(SPI_Port_t port, uint8_t* buf1, uint32_t len1, \
    uint8_t* buf2, uint32_t len2)
{
    SPI_ErrCode_t err = SPI_ERR_INVALID_INPUT;
    if((buf1 != NULL) && (len1 > 0) && (buf2 != NULL) && (len2 > 0))
    {
        err = SPI_ERR_OK;
        spi_select(port);
        err = spi_readBuf(port, buf1, len1);
        if(err == SPI_ERR_OK)
        {
            err = spi_readBuf(port, buf2, len2);
        }
        spi_deselect(port);
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_writeRead
 *
 * Reads in 2 buffers
 *
 * @param   > SPI_PORT: port to read
 *          > uint8_t*: buffer1 to read in to
 *          > uint32_t: number1 of bytes to read
 *          > uint8_t*: buffer2 to read in to
 *          > uint32_t: number2 of bytes to read
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_writeRead(SPI_Port_t port, uint8_t* bufWrite, \
    uint32_t lenWrite, uint8_t* bufRead, uint32_t lenRead)
{
    SPI_ErrCode_t err = SPI_ERR_OK;
    if((bufWrite != NULL) && (lenWrite > 0) && (bufRead != NULL) && (lenRead > 0))
    {
        err = SPI_ERR_OK;
        spi_select(port);
        err = spi_writeBuf(port, bufWrite, lenWrite);
        if(err == SPI_ERR_OK)
        {
            err = spi_readBuf(port, bufRead, lenRead);
        }
        spi_deselect(port);
    }
    else
    {
        err = SPI_ERR_INVALID_INPUT;
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_write2Read
 *
 * Reads in 2 buffers
 *
 * @param   > SPI_PORT: port to read
 *          > uint8_t*: buffer1 to read in to
 *          > uint32_t: number1 of bytes to read
 *          > uint8_t*: buffer2 to read in to
 *          > uint32_t: number2 of bytes to read
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
SPI_ErrCode_t SPI_write2Read(SPI_Port_t port, uint8_t* bufWrite1,\
    uint32_t lenWrite1, uint8_t* bufWrite2, uint32_t lenWrite2, uint8_t* bufRead, uint32_t lenRead)
{
    SPI_ErrCode_t err = SPI_ERR_OK;
    if((bufWrite1 != NULL) && (lenWrite1 > 0) && (bufWrite2 != NULL) && (lenWrite2 > 0) && (bufRead != NULL) && (lenRead > 0))
    {
        err = SPI_ERR_OK;
        spi_select(port);
        err = spi_writeBuf(port, bufWrite1, lenWrite1);
        if(err == SPI_ERR_OK)
        {
            err = spi_writeBuf(port, bufWrite2, lenWrite2);
        }
        if(err == SPI_ERR_OK)
        {
            err = spi_readBuf(port, bufRead, lenRead);
        }
        spi_deselect(port);
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
 * @param   > SPI_PORT: port
 *
 * @return None
 *
 ******************************************************************************/
static void spi_select(SPI_Port_t port)
{
    switch(port)
    {
        case SPI_PORT_1:
            digitalWrite(SPI_CS_PIN, 0);
            break;
        default:
            TRACE_ERROR("Invalid SPI Port %d", port);
            break;
    }
}

/*******************************************************************************
 * @brief SPI_select
 *
 * Disable Slave
 *
 * @param   > SPI_PORT: port
 *
 * @return None
 *
 ******************************************************************************/
static void spi_deselect(SPI_Port_t port)
{
    switch(port)
    {
        case SPI_PORT_1:
            digitalWrite(SPI_CS_PIN, 1);
            break;
        default:
            TRACE_ERROR("Invalid SPI Port %d", port);
            break;
    }
}

/*******************************************************************************
 * @brief SPI_writeBuf
 *
 * Write Buffer to SPI
 *
 * @param   > SPI_PORT: port to write
 *          > uint8_t*: buffer to write from
 *          > uint32_t: number of bytes to write
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
static SPI_ErrCode_t spi_writeBuf(SPI_Port_t port, uint8_t* buf, uint32_t len)
{
    SPI_ErrCode_t err = SPI_ERR_OK;
    int fd = wiringPiSPIDataRW(SPI_PORT_1, buf, (int) len);
    if(fd == SPI_BAD_CONNECTION_FD)
    {
        err = SPI_ERR_GENERAL;
    }
    return err;
}

/*******************************************************************************
 * @brief SPI_readBuf
 *
 * Read Buffer from SPI
 *
 * @param   > SPI_PORT: port to read
 *          > uint8_t*: buffer to read into
 *          > uint32_t: number of bytes to read
 *
 * @return SPI_ErrCode_t
 *
 ******************************************************************************/
static SPI_ErrCode_t spi_readBuf(SPI_Port_t port, uint8_t* buf, uint32_t len)
{
    SPI_ErrCode_t err = SPI_ERR_OK;
    int fd = wiringPiSPIDataRW(SPI_PORT_1, buf, (int) len);
    if(fd == SPI_BAD_CONNECTION_FD)
    {
        err = SPI_ERR_GENERAL;
    }
    return err;
}
