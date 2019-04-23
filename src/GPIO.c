/*******************************************************************************
 *  @file GPIO.c
 *
 *  @brief c file from which other c files in this project should be made
 *
 *  @author KSolomon
 *  @date Nov 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/


/******************************************************************************
 * Include Section
 ******************************************************************************/

// System Includes
#include "stm32f2xx_ll_bus.h"
#include "stm32f2xx_ll_exti.h"
#include "stm32f2xx_ll_system.h"
#include "stm32f2xx_it.h"
#include "Config.h"
#include "TypeDefs.h"

// Module Includes
#include "Token.h"

// Utility Includes

// Driver Includes
#include "SPI.h"
#include "UART.h"
#include "I2C.h"
#include "GPIO.h"
#include "Debug.h"
#include "USB.h"


/*******************************************************************************
 * Constants Declarations
 ******************************************************************************/

static const char* m_gpio_names[GPIO_PORT_COUNT][GPIO_PIN_COUNT];
static const char m_gpio_defaultName[] = "unnamed";


/*******************************************************************************
 * Data Types Declarations
 ******************************************************************************/


/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

// Configure pins for a specific SPI Port.
static void gpio_setupSPI(SPI_Port_t port);

// Configure pins for a specific USART (Asynchronous) Port. Either this
// OR setupUSART should be called for an in-use USART port. Not both.
static void gpio_setupUART(UART_SerialPort_t port);

// Configure pins for a specific USART (Synchronous) Port. Either this
// OR setupUART should be called for an in-use USART port. Not both.
static void gpio_setupUSART(UART_SerialPort_t port);

// Convert GPIO Port to EXTI Port
static uint32_t gpio_portToExtiPort(GPIO_TypeDef *GPIOx);

// Convert pin to EXTI Clock Line
static uint32_t gpio_pinToExtiClockLine(uint32_t pin);

// Convert pin to EXTI IQR (interrupt handler)
static IRQn_Type gpio_pinToIQR(uint32_t pin);

// Setup given pin to handle externally triggered interrupt
static void gpio_setupExtiInterrupt(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint8_t trigger, GPIO_EXTI_Piority_t priority);

// Setup given pin to handle internally triggered interrupt
static void gpio_setupSWTriggeredExtiInterrupt(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint8_t trigger, GPIO_EXTI_Piority_t priority);

// Convert from GPIO_TypeDef* to enum
static uint8_t gpio_portMask2Num(GPIO_TypeDef *GPIOx);

// Convert from pin mask to pin number (e.g. LL_GPIO_PIN_0 -> 0)
static uint8_t gpio_pinMask2PinNum(uint32_t PinMask);

// Initialze names for GPIOs
static void gpio_setupNames(void);

// Initialze name for GPIO
static void gpio_setupName(GPIO_TypeDef* GPIOx, uint32_t pinMask, char* name);

// Initialize USB peripheral
static void gpio_setupUSB(void);


/*******************************************************************************
 * Public Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief GPIO_config
 *
 * Configure all GPIO Ports/Pins. Call this soon after power-up, before calling
 * other peripheral's inits
 *
 *  @param  > None
 *
 *  @return None
 *
 ******************************************************************************/
void GPIO_config(void)
{
    // DeInit all the GPIOs
    LL_GPIO_DeInit(GPIOA);
    LL_GPIO_DeInit(GPIOB);
    LL_GPIO_DeInit(GPIOC);
    LL_GPIO_DeInit(GPIOD);
    LL_GPIO_DeInit(GPIOE);
    LL_GPIO_DeInit(GPIOF);
    LL_GPIO_DeInit(GPIOG);

    // Enable GPIO
    LL_AHB1_GRP1_EnableClock(GPIO_toPeriph(GPIOA));
    LL_AHB1_GRP1_EnableClock(GPIO_toPeriph(GPIOB));
    LL_AHB1_GRP1_EnableClock(GPIO_toPeriph(GPIOC));
    LL_AHB1_GRP1_EnableClock(GPIO_toPeriph(GPIOD));
    LL_AHB1_GRP1_EnableClock(GPIO_toPeriph(GPIOE));
    LL_AHB1_GRP1_EnableClock(GPIO_toPeriph(GPIOF));
    LL_AHB1_GRP1_EnableClock(GPIO_toPeriph(GPIOG));
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_OTGHS);

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);

    gpio_setupUART(DEBUG_PORT);

#ifndef BOOTLOADER
    gpio_setupUART(AMIMON_UART_PORT);

    gpio_setupUART(SILEX_UART_PORT);

    gpio_setupUSART(CHIPCORDER_USART_PORT);
#endif

    gpio_setupSPI(TOKEN_SPI_PORT);
    gpio_setupSPI(FLASH_SPI_PORT);

    GPIO_setupI2C(EEPROM_I2C_PORT);
    GPIO_setupI2C(DDC_I2C_PORT);

    gpio_setupUSB();

    // Initialize Input Pins (all High-Z)
    GPIO_SetupInputHiZ(SIDE_DETECT_PIN);
    GPIO_SetupInputHiZ(DIP_1_PIN);
    GPIO_SetupInputHiZ(DIP_2_PIN);
    GPIO_SetupInputHiZ(MCU_BOOT_PB_PIN);
    GPIO_SetupInputHiZ(DRACO_0_PIN);
    GPIO_SetupInputHiZ(BOOT_PIN);
    GPIO_SetupInputHiZ(CYCLONE10_INT_PIN);
    GPIO_SetupInputHiZ(CYCLONE10_0_PIN);
    GPIO_SetupInputHiZ(CYCLONE10_1_PIN);
    GPIO_SetupInputHiZ(MAX10_RDY_PIN);
    GPIO_SetupInputHiZ(DRACO_RTR_PIN);
    GPIO_SetupInputHiZ(DRACO_DEFAULT_BAUD_PIN);
    GPIO_SetupInputHiZ(AUDIO_INTB_PIN);
    GPIO_SetupInputHiZ(AUDIO_RDY_PIN);
    GPIO_SetupInputHiZ(PHAN_DET_PIN);
    GPIO_SetupInputHiZ(PHAN_5V_DET_PIN);

    GPIO_SetOutputPinsToDefault();

    gpio_setupExtiInterrupt(TOKEN_LOFO_PIN, LL_EXTI_TRIGGER_RISING_FALLING, TOKEN_LOFO_ISR_PRIORITY);
    
    // FIXME: In PP1, there is no PUR for LOFO line. Remove for PP2 SW
    #ifndef HW_VERSION_PP1
    if(GPIO_isRX())
    {
        GPIO_setupInputPin(TOKEN_LOFO_PIN, LL_GPIO_PULL_UP);
    }
    #endif
    
    gpio_setupExtiInterrupt(DRACO_INT_IN_PIN, LL_EXTI_TRIGGER_RISING, DRACO_ISR_PRIORITY);

    gpio_setupSWTriggeredExtiInterrupt(I2C_EXTI_PIN, LL_EXTI_TRIGGER_RISING, I2C_EXTI_ISR_PRIORITY); // Must be at least 1 level higher than the Timer priority (Timer.c)

    gpio_setupNames();
}

/*******************************************************************************
 * @brief GPIO_toggle
 *
 * Set given pin to a generic configuration of GPIO output pin
 *
 * @param  > GPIO_TypeDef*: port
 *         > uint32_t: pin
 *
 * @return None
 ******************************************************************************/
void GPIO_toggle(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
    LL_GPIO_TogglePin(GPIOx, PinMask);
}

/*******************************************************************************
 * @brief GPIO_profileStart
 *
 * Start Profile
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
void GPIO_profileStart(void)
{
    GPIO_toggle(DEBUG_TEST_PIN);
    GPIO_toggle(DEBUG_TEST_PIN);
}

/*******************************************************************************
 * @brief GPIO_profileEnd
 *
 * End Profile
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
void GPIO_profileEnd(void)
{
    GPIO_toggle(DEBUG_TEST_PIN);
}

/*******************************************************************************
 * @brief GPIO_isRX
 *
 * Determines what type of board this is from SIDE_DETECT_PIN.
 * Pin is Set if TX, Reset if RX
 *
 * @param  > None
 *
 * @return bool
 ******************************************************************************/
bool GPIO_isRX(void)
{
    return (bool) !LL_GPIO_IsInputPinSet(SIDE_DETECT_PIN);
}

/*******************************************************************************
 * @brief GPIO_toPeriph
 *
 * Convert from GPIO to the peripheral bitmask. This is used for init
 *
 * @param  > GPIO_TypeDef*: port
 *
 * @return None
 ******************************************************************************/
uint32_t GPIO_toPeriph(GPIO_TypeDef *GPIOx)
{
    uint32_t periph = LL_AHB1_GRP1_PERIPH_GPIOA;
    switch((uint32_t) GPIOx)
    {
    case (uint32_t) GPIOA:
        periph = LL_AHB1_GRP1_PERIPH_GPIOA;
        break;
    case (uint32_t) GPIOB:
        periph = LL_AHB1_GRP1_PERIPH_GPIOB;
        break;
    case (uint32_t) GPIOC:
        periph = LL_AHB1_GRP1_PERIPH_GPIOC;
        break;
    case (uint32_t) GPIOD:
        periph = LL_AHB1_GRP1_PERIPH_GPIOD;
        break;
    #if defined(GPIOE)
    case (uint32_t) GPIOE:
        periph = LL_AHB1_GRP1_PERIPH_GPIOE;
        break;
    #endif
    #if defined(GPIOF)
    case (uint32_t) GPIOF:
        periph = LL_AHB1_GRP1_PERIPH_GPIOF;
        break;
    #endif
    #if defined(GPIOG)
    case (uint32_t) GPIOG:
        periph = LL_AHB1_GRP1_PERIPH_GPIOG;
        break;
    #endif
    default:
        TRACE_ERROR("INVALID DEBUG PORT: %d", GPIOx);
        periph = NULL;
        break;
    }
    return periph;
}

/*******************************************************************************
 * @brief GPIO_setupOutputPin
 *
 * Set given pin to a generic configuration of GPIO output pin
 *
 * @param  > GPIO_TypeDef*: port
 *         > uint32_t: pin
 *         > uint32_t: mode (LL_GPIO_OUTPUT_PUSHPULL or LL_GPIO_OUTPUT_OPENDRAIN)
 *         > uint32_t: pull (LL_GPIO_PULL_UP or LL_GPIO_PULL_DOWN)
 *
 * @return None
 ******************************************************************************/
void GPIO_setupOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint32_t mode, uint32_t pullDirection)
{
    LL_GPIO_SetPinMode(GPIOx, PinMask, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(GPIOx, PinMask, mode);
    LL_GPIO_SetPinPull(GPIOx, PinMask, pullDirection);
    LL_GPIO_SetPinSpeed(GPIOx, PinMask, LL_GPIO_SPEED_FREQ_HIGH);
}

/*******************************************************************************
 * @brief GPIO_setupInputPin
 *
 * Set given pin to a generic configuration of GPIO EXTI Input
 *
 * @param  > GPIO_TypeDef*: port
 *         > uint32_t: pin
 *         > uint32_t: pull (LL_GPIO_PULL_UP, LL_GPIO_PULL_DOWN, or LL_GPIO_PULL_NO)
 *
 * @return None
 ******************************************************************************/
void GPIO_setupInputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint32_t pullDirection)
{
    LL_GPIO_SetPinMode(GPIOx, PinMask, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOx, PinMask, pullDirection);
    LL_GPIO_SetPinSpeed(GPIOx, PinMask, LL_GPIO_SPEED_FREQ_VERY_HIGH);
}

/*******************************************************************************
 * @brief GPIO_setPin
 *
 * Set given pin HIGH
 *
 * @param  > GPIO_TypeDef* : port
 *         > uint32_t: pin
 *
 * @return None
 ******************************************************************************/
void GPIO_setPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
    LL_GPIO_SetOutputPin(GPIOx, PinMask);
}

/*******************************************************************************
 * @brief GPIO_resetPin
 *
 * Set given pin LOW
 *
 * @param  > GPIO_TypeDef*: port
 *         > uint32_t: pin
 *
 * @return None
 ******************************************************************************/
void GPIO_resetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
    LL_GPIO_ResetOutputPin(GPIOx, PinMask);
}

/*******************************************************************************
 * @brief GPIO_isPinSet
 *
 * Determine if a pin previously setup as input pin is set or not
 *
 * @param  > GPIO_TypeDef*: port
 *         > uint32_t: pin
 *
 * @return bool
 ******************************************************************************/
bool GPIO_isPinSet(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
    return LL_GPIO_IsInputPinSet(GPIOx, PinMask);
}

/*******************************************************************************
 * @brief GPIO_setupI2C
 *
 * Configure pins for a specific I2C Port.
 *
 * @param  > I2C_Port_t
 *
 * @return None
 ******************************************************************************/
void GPIO_setupI2C(I2C_Port_t port) {
    LL_GPIO_InitTypeDef GPIO_SCL;
    LL_GPIO_InitTypeDef GPIO_SDA;

    GPIO_SCL.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_SCL.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_SCL.Pull = LL_GPIO_PULL_NO;
    GPIO_SCL.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_SCL.Alternate = LL_GPIO_AF_4;

    GPIO_SDA = GPIO_SCL;

    GPIO_TypeDef* gpio_scl_port;
    GPIO_TypeDef* gpio_sda_port;

    bool isValidPort = true;
    switch(port)
    {
        case SPI_PORT_1:
            GPIO_SCL.Pin  = I2C1_SCL_PIN_NUM;
            GPIO_SDA.Pin  = I2C1_SDA_PIN_NUM;

            gpio_scl_port = I2C1_SCL_PIN_PORT;
            gpio_sda_port = I2C1_SDA_PIN_PORT;
            break;
        case SPI_PORT_2:
            GPIO_SCL.Pin  = I2C2_SCL_PIN_NUM;
            GPIO_SDA.Pin  = I2C2_SDA_PIN_NUM;

            gpio_scl_port = I2C2_SCL_PIN_PORT;
            gpio_sda_port = I2C2_SDA_PIN_PORT;
            break;
        case SPI_PORT_3:
            GPIO_SCL.Pin  = I2C3_SCL_PIN_NUM;
            GPIO_SDA.Pin  = I2C3_SDA_PIN_NUM;

            gpio_scl_port = I2C3_SCL_PIN_PORT;
            gpio_sda_port = I2C3_SDA_PIN_PORT;
            break;
        default:
            TRACE_ERROR("Invalid I2C Port: %d", port);
            isValidPort = false;
    }
    if(isValidPort)
    {
        LL_GPIO_Init(gpio_scl_port, &GPIO_SCL);
        LL_GPIO_Init(gpio_sda_port, &GPIO_SDA);
        I2C_init(port);
    }
}

/*******************************************************************************
 * @brief GPIO_PinToExtiLine
 *
 * Convert pin to EXTI Line
 *
 * @param  > uint32_t : pin (e.g. LL_GPIO_PIN_0)
 *
 * @return IRQn_Type (e.g. LL_SYSCFG_EXTI_LINE0)
 ******************************************************************************/
uint32_t GPIO_PinToExtiLine(uint32_t pin)
{
    uint32_t line = LL_SYSCFG_EXTI_LINE0;
    switch(pin)
    {
    case LL_GPIO_PIN_0:
        line = LL_EXTI_LINE_0;
        break;
    case LL_GPIO_PIN_1:
        line = LL_EXTI_LINE_1;
        break;
    case LL_GPIO_PIN_2:
        line = LL_EXTI_LINE_2;
        break;
    case LL_GPIO_PIN_3:
        line = LL_EXTI_LINE_3;
        break;
    case LL_GPIO_PIN_4:
        line = LL_EXTI_LINE_4;
        break;
    case LL_GPIO_PIN_5:
        line = LL_EXTI_LINE_5;
        break;
    case LL_GPIO_PIN_6:
        line = LL_EXTI_LINE_6;
        break;
    case LL_GPIO_PIN_7:
        line = LL_EXTI_LINE_7;
        break;
    case LL_GPIO_PIN_8:
        line = LL_EXTI_LINE_8;
        break;
    case LL_GPIO_PIN_9:
        line = LL_EXTI_LINE_9;
        break;
    case LL_GPIO_PIN_10:
        line = LL_EXTI_LINE_10;
        break;
    case LL_GPIO_PIN_11:
        line = LL_EXTI_LINE_11;
        break;
    case LL_GPIO_PIN_12:
        line = LL_EXTI_LINE_12;
        break;
    case LL_GPIO_PIN_13:
        line = LL_EXTI_LINE_13;
        break;
    case LL_GPIO_PIN_14:
        line = LL_EXTI_LINE_14;
        break;
    case LL_GPIO_PIN_15:
        line = LL_EXTI_LINE_15;
        break;
    default:
        TRACE_ERROR("Invalid Pin: %d", pin);
        break;
    }
    return line;
}

/*******************************************************************************
 * @brief GPIO_Pin2Str
 *
 * Convert gpio pin to string representation
 *
 * @param  < char*               : name (must be at least 4 byte buffer)
 *         > GPIO_TypeDef        : port (e.g. GPIOA)
 *         > uint32_t            : pin  (e.g. LL_GPIO_PIN_0)
 *
 * @return None
 ******************************************************************************/
void GPIO_Pin2Str(char* name, GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
    uint8_t num = gpio_pinMask2PinNum(PinMask);

    switch((uint32_t) GPIOx)
    {
    case (uint32_t) GPIOA:
        name[0] = 'A';
        break;
    case (uint32_t) GPIOB:
        name[0] = 'B';
        break;
    case (uint32_t) GPIOC:
        name[0] = 'C';
        break;
    case (uint32_t) GPIOD:
        name[0] = 'D';
        break;
    #if defined(GPIOE)
    case (uint32_t) GPIOE:
        name[0] = 'E';
        break;
    #endif
    #if defined(GPIOF)
    case (uint32_t) GPIOF:
        name[0] = 'F';
        break;
    #endif
    #if defined(GPIOG)
    case (uint32_t) GPIOG:
        name[0] = 'G';
        break;
    #endif
    default:
        name[0] = 'X';
        break;
    }

    if(num > 9)
    {
        name[1] = '1';
        name[2] = NUM_TO_CHAR(num % 10);
    }
    else
    {
        name[1] = NUM_TO_CHAR(num % 10);
        name[2] = ' ';
    }
    name[3] = '\0';
}

/*******************************************************************************
 * @brief GPIO_PinGetName
 *
 * Get name of a GPIO Pin (if any)
 *
 * @param  > GPIO_TypeDef        : port (e.g. GPIOA)
 *         > uint32_t            : pin  (e.g. LL_GPIO_PIN_0)
 *
 * @return char*: name
 ******************************************************************************/
char* GPIO_PinGetName(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
    return (char*) m_gpio_names[gpio_portMask2Num(GPIOx)][gpio_pinMask2PinNum(PinMask)];
}

/*******************************************************************************
 * @brief GPIO_SetOutputPinsToDefault
 *
 * Set all output GPIO-only pins to default values
 *
 * @param  > None
 *
 * @return int
 ******************************************************************************/
int GPIO_SetOutputPinsToDefault(void)
{
    // Initialize Output Pins that have external control or aren't yet defined
    // as High-Z
    GPIO_SetupOutputHiZ(MAX10_RELOAD_PIN);
    GPIO_SetupOutputHiZ(DRACO_BOOT2ND_PIN);
    GPIO_SetupOutputHiZ(DRACO_UARTMUX_PIN);
    GPIO_SetupOutputHiZ(DRACO_RESET_PIN);
    GPIO_SetupOutputHiZ(CYCLONE10_RESET_PIN);

    GPIO_SetupOutputPushPull(DRACO_INT_OUT_PIN); // I want to define as pull down
    GPIO_SetupOutputHiZ(DRACO_CTS_PIN);
    GPIO_SetupOutputHiZ(PHAN_12V_ENUV_PIN);
    GPIO_SetupOutputHiZ(PHAN_5V_HPD_EN_PIN);

    // Outputs default high
    GPIO_SetupOutputPullUp(SILEX_RESET_PIN);
    GPIO_SetupOutputPullUp(TOKEN_HOLD_PIN);
    GPIO_SetupOutputPullUp(FLASH_HOLD_PIN);
    GPIO_SetupOutputPullUp(FLASH_WP_PIN);
    GPIO_SetupOutputPullUp(AUDIO_AMP_SHUTDOWN_PIN);

    // Outputs default low
    GPIO_SetupOutputPullDown(DEBUG_TEST_PIN);
    GPIO_SetupOutputPullDown(LED_1_PIN);
    GPIO_SetupOutputPullDown(LED_2_PIN);
    GPIO_SetupOutputPullDown(AUDIO_RESET_PIN);
    GPIO_SetupOutputPullDown(USB_POWER_EN_PIN);
    GPIO_SetupOutputPullDown(PHAN_HPD_MASK_PIN); // Defining as anything other than LOW will result in No Video

    return 0;
}


/*******************************************************************************
 * Private Function Implementation
 ******************************************************************************/

/*******************************************************************************
 * @brief gpio_setupSPI
 *
 * Configure pins for a specific SPI Port.
 *
 * @param  > SPI_Port_t
 *
 * @return None
 ******************************************************************************/
static void gpio_setupSPI(SPI_Port_t port)
{
    LL_GPIO_InitTypeDef GPIO_SCK;
    LL_GPIO_InitTypeDef GPIO_MISO;
    LL_GPIO_InitTypeDef GPIO_MOSI;
    LL_GPIO_InitTypeDef GPIO_CS;

    GPIO_SCK.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_SCK.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_SCK.Pull = LL_GPIO_PULL_NO;
    GPIO_SCK.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_SCK.Alternate = LL_GPIO_AF_5;

    GPIO_MOSI = GPIO_SCK;
    GPIO_MISO = GPIO_SCK;

    GPIO_TypeDef* gpio_sck_port;
    GPIO_TypeDef* gpio_mosi_port;
    GPIO_TypeDef* gpio_miso_port;
    GPIO_TypeDef* gpio_cs_port;

    bool isValidPort = true;
    switch(port)
    {
        case SPI_PORT_1:
            GPIO_SCK.Pin  = SPI1_SCK_PIN;
            GPIO_CS.Pin   = SPI1_CS_PIN;
            GPIO_MOSI.Pin = SPI1_MOSI_PIN;
            GPIO_MISO.Pin = SPI1_MISO_PIN;

            gpio_sck_port  = SPI1_SCK_PORT;
            gpio_mosi_port = SPI1_MOSI_PORT;
            gpio_miso_port = SPI1_MISO_PORT;
            gpio_cs_port   = SPI1_CS_PORT;
            break;
        case SPI_PORT_2:
            GPIO_SCK.Pin  = SPI2_SCK_PIN;
            GPIO_CS.Pin   = SPI2_CS_PIN;
            GPIO_MOSI.Pin = SPI2_MOSI_PIN;
            GPIO_MISO.Pin = SPI2_MISO_PIN;

            gpio_sck_port  = SPI2_SCK_PORT;
            gpio_mosi_port = SPI2_MOSI_PORT;
            gpio_miso_port = SPI2_MISO_PORT;
            gpio_cs_port   = SPI2_CS_PORT;
            break;
        case SPI_PORT_3:
            GPIO_SCK.Pin  = SPI3_SCK_PIN;
            GPIO_CS.Pin   = SPI3_CS_PIN;
            GPIO_MOSI.Pin = SPI3_MOSI_PIN;
            GPIO_MISO.Pin = SPI3_MISO_PIN;

            gpio_sck_port  = SPI3_SCK_PORT;
            gpio_mosi_port = SPI3_MOSI_PORT;
            gpio_miso_port = SPI3_MISO_PORT;
            gpio_cs_port   = SPI3_CS_PORT;

            GPIO_MOSI.Alternate = LL_GPIO_AF_6;
            GPIO_MISO.Alternate = LL_GPIO_AF_6;
            GPIO_SCK.Alternate = LL_GPIO_AF_6;
            break;
        default:
            TRACE_ERROR("Invalid SPI Port: %d", port);
            isValidPort = false;
            break;
    }
    if(isValidPort)
    {
        LL_GPIO_Init(gpio_sck_port, &GPIO_SCK);
        LL_GPIO_Init(gpio_mosi_port, &GPIO_MOSI);
        LL_GPIO_Init(gpio_miso_port, &GPIO_MISO);
        GPIO_setupOutputPin(gpio_cs_port, GPIO_CS.Pin, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_DOWN);
        SPI_initialize(port);
    }
}

/*******************************************************************************
 * @brief gpio_setupUART
 *
 * Configure pins for a specific USART (Asynchronous) Port. Either this
 * OR setupUSART should be called for an in-use USART port. Not both.
 *
 * @param  > UART_SerialPort_t
 *
 * @return None
 ******************************************************************************/
static void gpio_setupUART(UART_SerialPort_t port)
{
    LL_GPIO_InitTypeDef GPIO_RX;
    GPIO_RX.Speed       = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_RX.Mode        = LL_GPIO_MODE_ALTERNATE;
    GPIO_RX.OutputType  = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_RX.Pull        = LL_GPIO_PULL_UP;
    GPIO_RX.Alternate   = LL_GPIO_AF_7;

    LL_GPIO_InitTypeDef GPIO_TX = GPIO_RX;

    GPIO_TypeDef* gpio_tx_port;
    GPIO_TypeDef* gpio_rx_port;

    bool isValidPort = true;
    switch(port)
    {
        case SERIAL_PORT_0:
            GPIO_TX.Pin  = USART1_TX_PIN;
            GPIO_RX.Pin  = USART1_RX_PIN;
            gpio_tx_port = USART1_TX_PORT;
            gpio_rx_port = USART1_RX_PORT;
            break;
        case SERIAL_PORT_1:
            GPIO_TX.Pin  = USART2_TX_PIN;
            GPIO_RX.Pin  = USART2_RX_PIN;
            gpio_tx_port = USART2_TX_PORT;
            gpio_rx_port = USART2_RX_PORT;
            break;
        case SERIAL_PORT_2:
            GPIO_TX.Pin  = USART3_TX_PIN;
            GPIO_RX.Pin  = USART3_RX_PIN;
            gpio_tx_port = USART3_TX_PORT;
            gpio_rx_port = USART3_RX_PORT;
            break;
        case SERIAL_PORT_3:
            GPIO_TX.Pin  = UART4_TX_PIN;
            GPIO_RX.Pin  = UART4_RX_PIN;
            gpio_tx_port = UART4_TX_PORT;
            gpio_rx_port = UART4_RX_PORT;
            GPIO_TX.Alternate = LL_GPIO_AF_8;
            GPIO_RX.Alternate = LL_GPIO_AF_8;
            break;
        case SERIAL_PORT_4:
            GPIO_TX.Pin  = UART5_TX_PIN;
            GPIO_RX.Pin  = UART5_RX_PIN;
            gpio_tx_port = UART5_TX_PORT;
            gpio_rx_port = UART5_RX_PORT;
            GPIO_TX.Alternate = LL_GPIO_AF_8;
            GPIO_RX.Alternate = LL_GPIO_AF_8;
            break;
        case SERIAL_PORT_5:
            GPIO_TX.Pin  = USART6_TX_PIN;
            GPIO_RX.Pin  = USART6_RX_PIN;
            gpio_tx_port = USART6_TX_PORT;
            gpio_rx_port = USART6_RX_PORT;
            GPIO_TX.Alternate = LL_GPIO_AF_8;
            GPIO_RX.Alternate = LL_GPIO_AF_8;
            break;
        default:
            TRACE_ERROR("Invalid UART Port: %d", port);
            isValidPort = false;
            break;
    }
    if(isValidPort)
    {
        LL_GPIO_Init(gpio_tx_port, &GPIO_TX);
        LL_GPIO_Init(gpio_rx_port, &GPIO_RX);
        UART_Init(port);
    }
}

/*******************************************************************************
 * @brief gpio_setupUSART
 *
 * Configure pins for a specific USART (Synchronous) Port. Either this
 * OR setupUART should be called for an in-use USART port. Not both.
 *
 * @param  > UART_SerialPort_t
 *
 * @return None
 ******************************************************************************/
static void gpio_setupUSART(UART_SerialPort_t port)
{
    LL_GPIO_InitTypeDef GPIO_RX;
    GPIO_RX.Speed       = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_RX.Mode        = LL_GPIO_MODE_ALTERNATE;
    GPIO_RX.OutputType  = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_RX.Pull        = LL_GPIO_PULL_NO;
    GPIO_RX.Alternate   = LL_GPIO_AF_7;

    LL_GPIO_InitTypeDef GPIO_TX = GPIO_RX;
    LL_GPIO_InitTypeDef GPIO_SCK = GPIO_RX;
    LL_GPIO_InitTypeDef GPIO_CS = GPIO_RX;

    GPIO_TypeDef* gpio_tx_port;
    GPIO_TypeDef* gpio_rx_port;
    GPIO_TypeDef* gpio_sck_port;
    GPIO_TypeDef* gpio_cs_port;
    bool isValidPort = true;
    switch(port)
    {
        case SERIAL_PORT_0:
            gpio_tx_port = USART1_TX_PORT;
            gpio_rx_port = USART1_RX_PORT;
            gpio_sck_port = USART1_SCK_PORT;
            gpio_cs_port = USART1_CS_PORT;

            GPIO_TX.Pin  = USART1_TX_PIN;
            GPIO_RX.Pin  = USART1_RX_PIN;
            GPIO_CS.Pin  = USART1_CS_PIN;
            GPIO_SCK.Pin = USART1_SCK_PIN;
            break;
        case SERIAL_PORT_1:
            gpio_tx_port = USART2_TX_PORT;
            gpio_rx_port = USART2_RX_PORT;
            gpio_sck_port = USART2_SCK_PORT;
            gpio_cs_port = USART2_CS_PORT;

            GPIO_TX.Pin  = USART2_TX_PIN;
            GPIO_RX.Pin  = USART2_RX_PIN;
            GPIO_CS.Pin  = USART2_CS_PIN;
            GPIO_SCK.Pin = USART2_SCK_PIN;
            break;
        case SERIAL_PORT_2:
            gpio_tx_port = USART3_TX_PORT;
            gpio_rx_port = USART3_RX_PORT;
            gpio_sck_port = USART3_SCK_PORT;
            gpio_cs_port = USART3_CS_PORT;

            GPIO_TX.Pin  = USART3_TX_PIN;
            GPIO_RX.Pin  = USART3_RX_PIN;
            GPIO_CS.Pin  = USART3_CS_PIN;
            GPIO_SCK.Pin = USART3_SCK_PIN;
            break;
        case SERIAL_PORT_5:
            gpio_tx_port = USART6_TX_PORT;
            gpio_rx_port = USART6_RX_PORT;
            gpio_sck_port = USART6_SCK_PORT;
            gpio_cs_port = USART6_CS_PORT;

            GPIO_TX.Pin  = USART6_TX_PIN;
            GPIO_RX.Pin  = USART6_RX_PIN;
            GPIO_CS.Pin  = USART6_CS_PIN;
            GPIO_SCK.Pin = USART6_SCK_PIN;

            GPIO_TX.Alternate  = LL_GPIO_AF_8;
            GPIO_RX.Alternate  = LL_GPIO_AF_8;
            GPIO_SCK.Alternate = LL_GPIO_AF_8;
            break;
        default:
            TRACE_ERROR("Invalid USART Port: %d", port);
            isValidPort = false;
            break;
    }
    if(isValidPort)
    {
        LL_GPIO_Init(gpio_tx_port, &GPIO_TX);
        LL_GPIO_Init(gpio_rx_port, &GPIO_RX);
        LL_GPIO_Init(gpio_sck_port, &GPIO_SCK);
        GPIO_setupOutputPin(gpio_cs_port, GPIO_CS.Pin, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_DOWN);
        USART_Init(port);
    }
}

/*******************************************************************************
 * @brief gpio_setupUSB
 *
 * Configure pins for a USB HS device port
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
static void gpio_setupUSB(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    /**USB_OTG_HS GPIO Configuration
    PB14     ------> USB_OTG_HS_DM
    PB15     ------> USB_OTG_HS_DP
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_12;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral interrupt init */
    NVIC_SetPriority(OTG_HS_IRQn, 0);
    NVIC_EnableIRQ(OTG_HS_IRQn);
}


/*******************
 * EXTI FUNCTIONS
 ******************/

 /*******************************************************************************
 * @brief gpio_portToExtiPort
 *
 * Convert GPIO Port to EXTI Port
 *
 * @param  > GPIO_TypeDef* : port (e.g. GPIOA)
 *
 * @return uint32_t: EXTI Port
 ******************************************************************************/
static uint32_t gpio_portToExtiPort(GPIO_TypeDef *GPIOx)
{
    uint32_t extiPort = (uint32_t) GPIOA;
    switch((uint32_t) GPIOx)
    {
    case (uint32_t) GPIOA:
        extiPort = LL_SYSCFG_EXTI_PORTA;
        break;
    case (uint32_t) GPIOB:
        extiPort = LL_SYSCFG_EXTI_PORTB;
        break;
    case (uint32_t) GPIOC:
        extiPort = LL_SYSCFG_EXTI_PORTC;
        break;
    case (uint32_t) GPIOD:
        extiPort = LL_SYSCFG_EXTI_PORTD;
        break;
    #ifdef GPIOE
    case (uint32_t) GPIOE:
        extiPort = LL_SYSCFG_EXTI_PORTE;
        break;
    #endif
    #ifdef GPIOF
    case (uint32_t) GPIOF:
        extiPort = LL_SYSCFG_EXTI_PORTF;
        break;
    #endif
    #ifdef GPIOG
    case (uint32_t) GPIOG:
        extiPort = LL_SYSCFG_EXTI_PORTG;
        break;
    #endif
    default:
        TRACE_ERROR("INVALID EXTI PORT: %d", GPIOx);
        extiPort = NULL;
        break;
    }
    return extiPort;
}

/*******************************************************************************
 * @brief GPIO_PinToExtiLine
 *
 * Convert pin to EXTI Line
 *
 * @param  > uint32_t : pin (e.g. LL_GPIO_PIN_0)
 *
 * @return IRQn_Type (e.g. LL_SYSCFG_EXTI_LINE0)
 ******************************************************************************/
static uint32_t gpio_pinToExtiClockLine(uint32_t pin)
{
    uint32_t line = LL_SYSCFG_EXTI_LINE0;
    switch(pin)
    {
    case LL_GPIO_PIN_0:
        line = LL_SYSCFG_EXTI_LINE0;
        break;
    case LL_GPIO_PIN_1:
        line = LL_SYSCFG_EXTI_LINE1;
        break;
    case LL_GPIO_PIN_2:
        line = LL_SYSCFG_EXTI_LINE2;
        break;
    case LL_GPIO_PIN_3:
        line = LL_SYSCFG_EXTI_LINE3;
        break;
    case LL_GPIO_PIN_4:
        line = LL_SYSCFG_EXTI_LINE4;
        break;
    case LL_GPIO_PIN_5:
        line = LL_SYSCFG_EXTI_LINE5;
        break;
    case LL_GPIO_PIN_6:
        line = LL_SYSCFG_EXTI_LINE6;
        break;
    case LL_GPIO_PIN_7:
        line = LL_SYSCFG_EXTI_LINE7;
        break;
    case LL_GPIO_PIN_8:
        line = LL_SYSCFG_EXTI_LINE8;
        break;
    case LL_GPIO_PIN_9:
        line = LL_SYSCFG_EXTI_LINE9;
        break;
    case LL_GPIO_PIN_10:
        line = LL_SYSCFG_EXTI_LINE10;
        break;
    case LL_GPIO_PIN_11:
        line = LL_SYSCFG_EXTI_LINE11;
        break;
    case LL_GPIO_PIN_12:
        line = LL_SYSCFG_EXTI_LINE12;
        break;
    case LL_GPIO_PIN_13:
        line = LL_SYSCFG_EXTI_LINE13;
        break;
    case LL_GPIO_PIN_14:
        line = LL_SYSCFG_EXTI_LINE14;
        break;
    case LL_GPIO_PIN_15:
        line = LL_SYSCFG_EXTI_LINE15;
        break;
    default:
        TRACE_ERROR("Invalid Pin: %d", pin);
        break;
    }
    return line;
}

/*******************************************************************************
 * @brief gpio_pinToIQR
 *
 * Convert pin to EXTI IQR (interrupt handler)
 *
 * @param  > uint32_t : pin (e.g. LL_GPIO_PIN_0)
 *
 * @return IRQn_Type
 ******************************************************************************/
static IRQn_Type gpio_pinToIQR(uint32_t pin)
{
    IRQn_Type irq = EXTI0_IRQn;
    switch(pin)
    {
    case LL_GPIO_PIN_0:
        irq = EXTI0_IRQn;
        break;
    case LL_GPIO_PIN_1:
        irq = EXTI1_IRQn;
        break;
    case LL_GPIO_PIN_2:
        irq = EXTI2_IRQn;
        break;
    case LL_GPIO_PIN_3:
        irq = EXTI3_IRQn;
        break;
    case LL_GPIO_PIN_4:
        irq = EXTI4_IRQn;
        break;
    case LL_GPIO_PIN_5:
    case LL_GPIO_PIN_6:
    case LL_GPIO_PIN_7:
    case LL_GPIO_PIN_8:
    case LL_GPIO_PIN_9:
        irq = EXTI9_5_IRQn;
        break;
    case LL_GPIO_PIN_10:
    case LL_GPIO_PIN_11:
    case LL_GPIO_PIN_12:
    case LL_GPIO_PIN_13:
    case LL_GPIO_PIN_14:
    case LL_GPIO_PIN_15:
        irq = EXTI15_10_IRQn;
        break;
    default:
        TRACE_ERROR("Invalid Pin: %d", pin);
        break;
    }
    return irq;
}

/*******************************************************************************
 * @brief gpio_setupExtiInterrupt
 *
 * Setup given pin to handle externally triggered interrupt
 *
 * @param  > GPIO_TypeDef        : port      (e.g. GPIOA)
 *         > uint32_t            : pin       (e.g. LL_GPIO_PIN_0)
 *         > uint8_t             : trigger   (e.g. LL_EXTI_TRIGGER_FALLING)
 *         > GPIO_EXTI_Piority_t : priority  (e.g. GPIO_PRI_0)
 *
 * @return None
 ******************************************************************************/
static void gpio_setupExtiInterrupt(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint8_t trigger, GPIO_EXTI_Piority_t priority)
{
    if(trigger == LL_EXTI_TRIGGER_RISING)
    {
        GPIO_setupInputPin(GPIOx, PinMask, LL_GPIO_PULL_DOWN);
    }
    else if(trigger == LL_EXTI_TRIGGER_FALLING)
    {
        GPIO_setupInputPin(GPIOx, PinMask, LL_GPIO_PULL_UP);
    } else
    {
        GPIO_setupInputPin(GPIOx, PinMask, LL_GPIO_PULL_NO);
    }

    LL_SYSCFG_SetEXTISource(gpio_portToExtiPort(GPIOx), gpio_pinToExtiClockLine(PinMask));

    LL_EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.Line_0_31 = GPIO_PinToExtiLine(PinMask);
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = trigger;
    LL_EXTI_Init(&EXTI_InitStruct);

    NVIC_SetPriority(gpio_pinToIQR(PinMask), priority);
    NVIC_EnableIRQ(gpio_pinToIQR(PinMask));
}

/*******************************************************************************
 * @brief gpio_setupSWTriggeredExtiInterrupt
 *
 * Setup given pin to handle internally triggered interrupt
 *
 * @param  > GPIO_TypeDef        : port      (e.g. GPIOA)
 *         > uint32_t            : pin       (e.g. LL_GPIO_PIN_0)
 *         > uint8_t             : trigger   (e.g. LL_EXTI_TRIGGER_FALLING)
 *         > GPIO_EXTI_Piority_t : priority  (e.g. GPIO_PRI_0)
 *
 * @return None
 ******************************************************************************/
static void gpio_setupSWTriggeredExtiInterrupt(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint8_t trigger, GPIO_EXTI_Piority_t priority)
{
    GPIO_setupOutputPin(GPIOx, PinMask, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_DOWN);

    LL_SYSCFG_SetEXTISource(gpio_portToExtiPort(GPIOx), gpio_pinToExtiClockLine(PinMask));

    LL_EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.Line_0_31 = GPIO_PinToExtiLine(PinMask);
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = trigger;
    LL_EXTI_Init(&EXTI_InitStruct);

    NVIC_SetPriority(gpio_pinToIQR(PinMask), priority);
    NVIC_EnableIRQ(gpio_pinToIQR(PinMask));
}

/*******************************************************************************
 * @brief gpio_portMask2Num
 *
 * Convert from GPIO_TypeDef* to enum
 *
 * @param  > GPIO_TypeDef*: port
 *
 * @return uint8_t
 ******************************************************************************/
static uint8_t gpio_portMask2Num(GPIO_TypeDef *GPIOx)
{
    GPIO_Port_t port = GPIO_PORT_A;
    switch((uint32_t) GPIOx)
    {
    case (uint32_t) GPIOA:
        port = GPIO_PORT_A;
        break;
    case (uint32_t) GPIOB:
        port = GPIO_PORT_B;
        break;
    case (uint32_t) GPIOC:
        port = GPIO_PORT_C;
        break;
    case (uint32_t) GPIOD:
        port = GPIO_PORT_D;
        break;
    case (uint32_t) GPIOE:
        port = GPIO_PORT_E;
        break;
    default:
        TRACE_ERROR("INVALID EXTI PORT: %d", GPIOx);
        port = (GPIO_Port_t) NULL;
        break;
    }
    return (uint8_t) port;
}

/*******************************************************************************
 * @brief gpio_pinMask2PinNum
 *
 * Convert from pin mask to pin number (e.g. LL_GPIO_PIN_0 -> 0)
 *
 * @param  > uint32_t: pin
 *
 * @return uint8_t
 ******************************************************************************/
static uint8_t gpio_pinMask2PinNum(uint32_t PinMask)
{
    uint8_t pinNum = 0;
    while(PinMask != 1)
    {
        PinMask >>= 1;
        pinNum++;
    }
    return pinNum;
}

/*******************************************************************************
 * @brief gpio_setupNames
 *
 * Initialze names for GPIOs
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
static void gpio_setupNames(void)
{
    for(uint8_t i = 0; i < GPIO_PORT_COUNT; i++)
    {
        for(uint8_t j = 0; j < GPIO_PIN_COUNT; j++)
        {
            m_gpio_names[i][j] = m_gpio_defaultName;
        }
    }

    // Input Pins
    gpio_setupName(SIDE_DETECT_PIN, SIDE_DETECT_NAME);
    gpio_setupName(TOKEN_LOFO_PIN, TOKEN_LOFO_NAME);
    gpio_setupName(DIP_1_PIN, DIP_1_NAME);
    gpio_setupName(DIP_2_PIN, DIP_2_NAME);
    gpio_setupName(MCU_BOOT_PB_PIN, MCU_BOOT_PB_NAME);
    gpio_setupName(DRACO_INT_IN_PIN, DRACO_INT_IN_NAME);
    gpio_setupName(DRACO_0_PIN, DRACO_0_NAME);
    gpio_setupName(BOOT_PIN, BOOT_NAME);
    gpio_setupName(CYCLONE10_INT_PIN,  CYCLONE10_INT_NAME);
    gpio_setupName(CYCLONE10_0_PIN, CYCLONE10_0_NAME);
    gpio_setupName(CYCLONE10_1_PIN, CYCLONE10_1_NAME);
    gpio_setupName(MAX10_RDY_PIN, MAX10_RDY_NAME);
    gpio_setupName(DRACO_RTR_PIN, DRACO_RTR_NAME);
    gpio_setupName(DRACO_DEFAULT_BAUD_PIN, DRACO_DEFAULT_BAUD_NAME);
    gpio_setupName(AUDIO_INTB_PIN, AUDIO_INTB_NAME);
    gpio_setupName(AUDIO_RDY_PIN, AUDIO_RDY_NAME);
    gpio_setupName(PHAN_DET_PIN, PHAN_DET_NAME);
    gpio_setupName(PHAN_5V_DET_PIN, PHAN_5V_DET_NAME);

    // Outputs default High
    gpio_setupName(SILEX_RESET_PIN, SILEX_RESET_NAME);
    gpio_setupName(TOKEN_HOLD_PIN, TOKEN_HOLD_NAME);
    gpio_setupName(FLASH_HOLD_PIN, FLASH_HOLD_NAME);
    gpio_setupName(FLASH_WP_PIN, FLASH_WP_NAME);
    gpio_setupName(AUDIO_AMP_SHUTDOWN_PIN, AUDIO_AMP_SHUTDOWN_NAME);
    gpio_setupName(DRACO_RESET_PIN, DRACO_RESET_NAME);
    gpio_setupName(CYCLONE10_RESET_PIN, CYCLONE10_RESET_NAME);

    // Outputs default Low
    gpio_setupName(DEBUG_TEST_PIN, DEBUG_TEST_NAME);
    gpio_setupName(LED_1_PIN, LED_1_NAME);
    gpio_setupName(LED_2_PIN, LED_2_NAME);
    gpio_setupName(AUDIO_RESET_PIN, AUDIO_RESET_NAME);
    gpio_setupName(USB_POWER_EN_PIN, USB_POWER_EN_NAME);
    gpio_setupName(I2C_EXTI_PIN, I2C_EXTI_NAME);

    // Outputs default HiZ
    gpio_setupName(MAX10_RELOAD_PIN, MAX10_RELOAD_NAME);
    gpio_setupName(DRACO_INT_OUT_PIN, DRACO_INT_OUT_NAME);
    gpio_setupName(DRACO_BOOT2ND_PIN, DRACO_BOOT2ND_NAME);
    gpio_setupName(DRACO_UARTMUX_PIN, DRACO_UARTMUX_NAME);
    gpio_setupName(DRACO_CTS_PIN, DRACO_CTS_NAME);
    gpio_setupName(PHAN_12V_ENUV_PIN, PHAN_12V_ENUV_NAME);
    gpio_setupName(PHAN_5V_HPD_EN_PIN, PHAN_5V_HPD_EN_NAME);
    gpio_setupName(PHAN_HPD_MASK_PIN, PHAN_HPD_MASK_NAME);

}

/*******************************************************************************
 * @brief gpio_setupName
 *
 * Initialze name for GPIO
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
static void gpio_setupName(GPIO_TypeDef* GPIOx, uint32_t pinMask, char* name)
{
    m_gpio_names[gpio_portMask2Num(GPIOx)][gpio_pinMask2PinNum(pinMask)] = name;
}

// EOF
