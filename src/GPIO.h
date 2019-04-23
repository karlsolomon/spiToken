/*******************************************************************************
 *  @file GPIO.h
 *
 *  @brief  header file from which other headers in this project should be made
 *
 *  @author KSolomon
 *  @date Nov 2018
 *  @copyright 2018 Stryker Corporation. All rights reserved.
 ******************************************************************************/

#ifndef _GPIO_H_
#define _GPIO_H_


/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "TypeDefs.h"
#include "stm32f2xx_ll_gpio.h"

#include "I2C.h"


/*******************************************************************************
 * Macros
 ******************************************************************************/

typedef enum
{
    GPIO_PORT_A = 0,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_F,
    GPIO_PORT_G,
    GPIO_PORT_COUNT
} GPIO_Port_t;

#define GPIO_PIN_COUNT 16

/***********************************
 * General Defines
 **********************************/

// Defs for GPIO Input/Output Pin Configuration
#define GPIO_PULL_UP                LL_GPIO_PULL_UP
#define GPIO_PULL_DOWN              LL_GPIO_PULL_DOWN
#define GPIO_PULL_NO                LL_GPIO_PULL_NO
#define GPIO_OUTPUT_PUSHPULL        LL_GPIO_OUTPUT_PUSHPULL
#define GPIO_OUTPUT_OPENDRAIN       LL_GPIO_OUTPUT_OPENDRAIN

#define GPIO_NAME_SIZE              4

#define GPIO_SetupInputHiZ(pin)          GPIO_setupInputPin(pin, LL_GPIO_PULL_NO)
#define GPIO_SetupOutputHiZ(pin)         do { \
            GPIO_setPin(pin); \
            GPIO_setupOutputPin(pin, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_UP); \
        } while(0)
#define GPIO_SetupOutputPushPull(pin)    GPIO_setupOutputPin(pin, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_NO)
#define GPIO_SetupOutputPullDown(pin)    GPIO_setupOutputPin(pin, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_DOWN)
#define GPIO_SetupOutputPullUp(pin)      do { \
            GPIO_setupOutputPin(pin, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_UP); \
            GPIO_setPin(pin); \
        } while(0) 

/***********************************
 * Major Peripherals
 **********************************/

// PINS for SPI_PORT_1 -- Flash
#define SPI1_SCK_PORT               GPIOA
#define SPI1_SCK_PIN                LL_GPIO_PIN_5   // Pin 6 on chip
#define SPI1_MISO_PORT              GPIOA
#define SPI1_MISO_PIN               LL_GPIO_PIN_6   // Pin 2 on chip
#define SPI1_MOSI_PORT              GPIOA
#define SPI1_MOSI_PIN               LL_GPIO_PIN_7   // Pin 5 on chip
#define SPI1_CS_PORT                GPIOA
#define SPI1_CS_PIN                 LL_GPIO_PIN_4   // Pin 1 on chip

// PINS for SPI_PORT_2 -- UNUSED
#define SPI2_SCK_PORT               GPIOB
#define SPI2_SCK_PIN                LL_GPIO_PIN_13
#define SPI2_MISO_PORT              GPIOC
#define SPI2_MISO_PIN               LL_GPIO_PIN_2
#define SPI2_MOSI_PORT              GPIOC
#define SPI2_MOSI_PIN               LL_GPIO_PIN_3
#define SPI2_CS_PORT                GPIOB
#define SPI2_CS_PIN                 LL_GPIO_PIN_1

// PINS for SPI_PORT_3 -- Token
#define SPI3_SCK_PORT               GPIOC
#define SPI3_SCK_PIN                LL_GPIO_PIN_10  // Pin 5 on hdr
#define SPI3_MISO_PORT              GPIOC
#define SPI3_MISO_PIN               LL_GPIO_PIN_11  // Pin 7 on hdr
#define SPI3_MOSI_PORT              GPIOB
#define SPI3_MOSI_PIN               LL_GPIO_PIN_5   // Pin 6 on hdr
#define SPI3_CS_PORT                GPIOD
#define SPI3_CS_PIN                 LL_GPIO_PIN_10  // Pin 4 on hdr

// PINS for SERIAL_PORT_0 -- UNUSED
#define USART1_TX_PORT              GPIOA
#define USART1_TX_PIN               LL_GPIO_PIN_9
#define USART1_RX_PORT              GPIOA
#define USART1_RX_PIN               LL_GPIO_PIN_10
#define USART1_SCK_PORT             GPIOA
#define USART1_SCK_PIN              LL_GPIO_PIN_8
#define USART1_CS_PORT              GPIOA
#define USART1_CS_PIN               LL_GPIO_PIN_1

// PINS for SERIAL_PORT_1 -- SYNCHRONOUS -- Chipcorder (TX Only)
#define USART2_TX_PORT              GPIOA
#define USART2_TX_PIN               LL_GPIO_PIN_2  // Pin 16 on chip
#define USART2_RX_PORT              GPIOA
#define USART2_RX_PIN               LL_GPIO_PIN_3  // Pin 13 on chip
#define USART2_SCK_PORT             GPIOD
#define USART2_SCK_PIN              LL_GPIO_PIN_7  // Pin 14 on chip
#define USART2_CS_PORT              GPIOB
#define USART2_CS_PIN               LL_GPIO_PIN_9  // Pin 15 on chip

// PINS for SERIAL_PORT_2 -- ASYNCHRONOUS -- Debug
#define USART3_TX_PORT              GPIOD
#define USART3_TX_PIN               LL_GPIO_PIN_8
#define USART3_RX_PORT              GPIOD
#define USART3_RX_PIN               LL_GPIO_PIN_9
// UNUSED
#define USART3_SCK_PORT             GPIOD
#define USART3_SCK_PIN              LL_GPIO_PIN_10
#define USART3_CS_PORT              GPIOD
#define USART3_CS_PIN               LL_GPIO_PIN_1

// PINS for SERIAL_PORT_3 -- Silex (RX Only)
#define UART4_TX_PORT               GPIOA
#define UART4_TX_PIN                LL_GPIO_PIN_0  // Pin 38 on hdr
#define UART4_RX_PORT               GPIOA
#define UART4_RX_PIN                LL_GPIO_PIN_1  // Pin 36 on hdr

// PINS for SERIAL_PORT_4 -- Draco
#define UART5_TX_PORT               GPIOC
#define UART5_TX_PIN                LL_GPIO_PIN_12
#define UART5_RX_PORT               GPIOD
#define UART5_RX_PIN                LL_GPIO_PIN_2

// PINS for SERIAL_PORT_5 -- UNUSED
#define USART6_TX_PORT              GPIOC
#define USART6_TX_PIN               LL_GPIO_PIN_6
#define USART6_RX_PORT              GPIOC
#define USART6_RX_PIN               LL_GPIO_PIN_7
#define USART6_SCK_PORT             GPIOC
#define USART6_SCK_PIN              LL_GPIO_PIN_8
#define USART6_CS_PORT              GPIOE
#define USART6_CS_PIN               LL_GPIO_PIN_9   // TODO: update if used

// PINS for I2S_PORT_2 -- Chipcorder
#define I2S2_SD_PORT                GPIOC
#define I2S2_SD_PIN                 LL_GPIO_PIN_3   // Pin 4 on hdr
#define I2S2_SCK_PORT               GPIOB
#define I2S2_SCK_PIN                LL_GPIO_PIN_10  // Pin 5 on hdr
#define I2S2_WS_PORT                GPIO_B
#define I2S2_WS_PIN                 LL_GPIO_PIN_12  // Pin 6 on hdr
#define I2S2_MCK_PORT               GPIOC
#define I2S2_MCK_PIN                LL_GPIO_PIN_6   // NC

// PINS for I2C1
#define I2C1_SCL_PIN_PORT           GPIOB
#define I2C1_SCL_PIN_NUM            LL_GPIO_PIN_6
#define I2C1_SCL_PIN                I2C1_SCL_PIN_PORT, I2C1_SCL_PIN_NUM // 11 - LED, 6 - EEPROM
#define I2C1_SDA_PIN_PORT           GPIOB
#define I2C1_SDA_PIN_NUM            LL_GPIO_PIN_7
#define I2C1_SDA_PIN                I2C1_SDA_PIN_PORT, I2C1_SDA_PIN_NUM // 9 - LED, 5 - EEPROM

// PINS for I2C2 -- UNUSED
#define I2C2_SCL_PIN_PORT           GPIOB
#define I2C2_SCL_PIN_NUM            LL_GPIO_PIN_10
#define I2C2_SCL_PIN                I2C2_SCL_PIN_PORT, I2C2_SCL_PIN_NUM
#define I2C2_SDA_PIN_PORT           GPIOB
#define I2C2_SDA_PIN_NUM            LL_GPIO_PIN_11
#define I2C2_SDA_PIN                I2C2_SDA_PIN_PORT, I2C2_SDA_PIN_NUM

// PINS for I2C3
#define I2C3_SCL_PIN_PORT           GPIOA
#define I2C3_SCL_PIN_NUM            LL_GPIO_PIN_8
#define I2C3_SCL_PIN                I2C3_SCL_PIN_PORT, I2C3_SCL_PIN_NUM
#define I2C3_SDA_PIN_PORT           GPIOC
#define I2C3_SDA_PIN_NUM            LL_GPIO_PIN_9
#define I2C3_SDA_PIN                I2C3_SDA_PIN_PORT, I2C3_SDA_PIN_NUM


/***********************************
 * Minor Peripherals
 **********************************/


/*************
 * Inputs
 ************/

// PIN for RX/TX Definition
#define SIDE_DETECT_PIN_PORT        GPIOE
#define SIDE_DETECT_PIN_NUM         LL_GPIO_PIN_13
#define SIDE_DETECT_PIN             SIDE_DETECT_PIN_PORT, SIDE_DETECT_PIN_NUM
#define SIDE_DETECT_NAME            "SIDE_DETECT"

// PIN for TOKEN insertion EXTI
// This Pin # MUST be maintained w/ the EXTI_IRQHandler. Current handler only
// supports this function if it is a pin # 10-15 (inclusive).
#define TOKEN_LOFO_PIN_PORT         GPIOD
#define TOKEN_LOFO_PIN_NUM          LL_GPIO_PIN_12
#define TOKEN_LOFO_PIN              TOKEN_LOFO_PIN_PORT, TOKEN_LOFO_PIN_NUM
#define TOKEN_LOFO_NAME             "TOKEN_LOFO"

// PINS to read DIP switch
#define DIP_1_PIN_PORT              GPIOC
#define DIP_1_PIN_NUM               LL_GPIO_PIN_7
#define DIP_1_PIN                   DIP_1_PIN_PORT, DIP_1_PIN_NUM
#define DIP_1_NAME                  "DIP_1"

#define DIP_2_PIN_PORT              GPIOC
#define DIP_2_PIN_NUM               LL_GPIO_PIN_8
#define DIP_2_PIN                   DIP_2_PIN_PORT, DIP_2_PIN_NUM
#define DIP_2_NAME                  "DIP_2"

// PIN to detect power button state
#define MCU_BOOT_PB_PIN_PORT        GPIOE
#define MCU_BOOT_PB_PIN_NUM         LL_GPIO_PIN_8
#define MCU_BOOT_PB_PIN             MCU_BOOT_PB_PIN_PORT, MCU_BOOT_PB_PIN_NUM
#define MCU_BOOT_PB_NAME            "MCU_BOOT_PB"

// PIN for Draco Event Reg ISR. Interrupt triggered = high (rising)
#define DRACO_INT_IN_PIN_PORT       GPIOD
#define DRACO_INT_IN_PIN_NUM        LL_GPIO_PIN_0
#define DRACO_INT_IN_PIN            DRACO_INT_IN_PIN_PORT, DRACO_INT_IN_PIN_NUM
#define DRACO_INT_IN_NAME           "DRACO_INT_IN"

#define DRACO_0_PIN_PORT            GPIOD
#define DRACO_0_PIN_NUM             LL_GPIO_PIN_14
#define DRACO_0_PIN                 DRACO_0_PIN_PORT, DRACO_0_PIN_NUM
#define DRACO_0_NAME                "DRACO_0"

// active low
#define BOOT_PIN_PORT               GPIOE
#define BOOT_PIN_NUM                LL_GPIO_PIN_8
#define BOOT_PIN                    BOOT_PIN_PORT, BOOT_PIN_NUM
#define BOOT_NAME                   "BOOT"

// active high
#define CYCLONE10_INT_PIN_PORT      GPIOE
#define CYCLONE10_INT_PIN_NUM       LL_GPIO_PIN_3
#define CYCLONE10_INT_PIN           CYCLONE10_INT_PIN_PORT, CYCLONE10_INT_PIN_NUM
#define CYCLONE10_INT_NAME          "CYCLONE10_INT"

#define CYCLONE10_0_PIN_PORT        GPIOE
#define CYCLONE10_0_PIN_NUM         LL_GPIO_PIN_5
#define CYCLONE10_0_PIN             CYCLONE10_0_PIN_PORT, CYCLONE10_0_PIN_NUM
#define CYCLONE10_0_NAME            "CYCLONE10_0"

#define CYCLONE10_1_PIN_PORT        GPIOE
#define CYCLONE10_1_PIN_NUM         LL_GPIO_PIN_6
#define CYCLONE10_1_PIN             CYCLONE10_1_PIN_PORT, CYCLONE10_1_PIN_NUM
#define CYCLONE10_1_NAME            "CYCLONE10_1"

#define MAX10_RDY_PIN_PORT          GPIOE
#define MAX10_RDY_PIN_NUM           LL_GPIO_PIN_4
#define MAX10_RDY_PIN               MAX10_RDY_PIN_PORT, MAX10_RDY_PIN_NUM
#define MAX10_RDY_NAME              "MAX10_RDY"

#define DRACO_RTR_PIN_PORT          GPIOC
#define DRACO_RTR_PIN_NUM           LL_GPIO_PIN_13
#define DRACO_RTR_PIN               DRACO_RTR_PIN_PORT, DRACO_RTR_PIN_NUM
#define DRACO_RTR_NAME              "DRACO_RTR"

// active high (high = non-default, low = 115200)
#define DRACO_DEFAULT_BAUD_PIN_PORT GPIOC
#define DRACO_DEFAULT_BAUD_PIN_NUM  LL_GPIO_PIN_14
#define DRACO_DEFAULT_BAUD_PIN      DRACO_DEFAULT_BAUD_PIN_PORT, DRACO_DEFAULT_BAUD_PIN_NUM
#define DRACO_DEFAULT_BAUD_NAME     "DRACO_DEFAULT_BAUD"

#define AUDIO_INTB_PIN_PORT         GPIOC
#define AUDIO_INTB_PIN_NUM          LL_GPIO_PIN_0
#define AUDIO_INTB_PIN              AUDIO_INTB_PIN_PORT, AUDIO_INTB_PIN_NUM
#define AUDIO_INTB_NAME             "AUDIO_INTB"

#define AUDIO_RDY_PIN_PORT          GPIOC
#define AUDIO_RDY_PIN_NUM           LL_GPIO_PIN_1
#define AUDIO_RDY_PIN               AUDIO_RDY_PIN_PORT, AUDIO_RDY_PIN_NUM
#define AUDIO_RDY_NAME              "AUDIO_RDY"

#define PHAN_DET_PIN_PORT           GPIOD
#define PHAN_DET_PIN_NUM            LL_GPIO_PIN_3
#define PHAN_DET_PIN                PHAN_DET_PIN_PORT, PHAN_DET_PIN_NUM
#define PHAN_DET_NAME               "PHAN_DET"

#define PHAN_5V_DET_PIN_PORT        GPIOD
#define PHAN_5V_DET_PIN_NUM         LL_GPIO_PIN_4
#define PHAN_5V_DET_PIN             PHAN_5V_DET_PIN_PORT, PHAN_5V_DET_PIN_NUM
#define PHAN_5V_DET_NAME            "PHAN_5V_DET"


/*************
 * Outputs
 ************/

// Default State HIGH

// PIN to reset Silex module. Lo = Reset
#define SILEX_RESET_PIN_PORT        GPIOE
#define SILEX_RESET_PIN_NUM         LL_GPIO_PIN_11
#define SILEX_RESET_PIN             SILEX_RESET_PIN_PORT, SILEX_RESET_PIN_NUM
#define SILEX_RESET_NAME            "SILEX_RESET"

// PIN for TOKEN HOLD. Should be Driven HIGH and left alone
#define TOKEN_HOLD_PIN_PORT         GPIOD
#define TOKEN_HOLD_PIN_NUM          LL_GPIO_PIN_11
#define TOKEN_HOLD_PIN              TOKEN_HOLD_PIN_PORT, TOKEN_HOLD_PIN_NUM
#define TOKEN_HOLD_NAME             "TOKEN_HOLD"

// Leave defined as high
#define FLASH_HOLD_PIN              GPIOE, LL_GPIO_PIN_14
#define FLASH_HOLD_NAME             "FLASH_HOLD"
#define FLASH_WP_PIN                GPIOE, LL_GPIO_PIN_15
#define FLASH_WP_NAME               "FLASH_WP"

// Shutdown = Low, default = high
#define AUDIO_AMP_SHUTDOWN_PIN_PORT GPIOB
#define AUDIO_AMP_SHUTDOWN_PIN_NUM  LL_GPIO_PIN_1
#define AUDIO_AMP_SHUTDOWN_PIN      AUDIO_AMP_SHUTDOWN_PIN_PORT, AUDIO_AMP_SHUTDOWN_PIN_NUM
#define AUDIO_AMP_SHUTDOWN_NAME     "AUDIO_AMP_SHUTDOWN"

// PIN to send mainboard reset cmd
#define DRACO_RESET_PIN_PORT        GPIOE
#define DRACO_RESET_PIN_NUM         LL_GPIO_PIN_10
#define DRACO_RESET_PIN             DRACO_RESET_PIN_PORT, DRACO_RESET_PIN_NUM
#define DRACO_RESET_NAME            "DRACO_RESET"

// LO = Reset
#define CYCLONE10_RESET_PIN_PORT    GPIOE
#define CYCLONE10_RESET_PIN_NUM     LL_GPIO_PIN_9
#define CYCLONE10_RESET_PIN         CYCLONE10_RESET_PIN_PORT, CYCLONE10_RESET_PIN_NUM
#define CYCLONE10_RESET_NAME        "CYCLONE10_RESET"


// Default State LOW

// PINS for GPIO Profiling Pin - use to determine function/operation duration
#define DEBUG_TEST_PIN_PORT         GPIOC
#define DEBUG_TEST_PIN_NUM          LL_GPIO_PIN_4
#define DEBUG_TEST_PIN              DEBUG_TEST_PIN_PORT, DEBUG_TEST_PIN_NUM
#define DEBUG_TEST_NAME             "DEBUG_TEST"

// PINS to turn on debug LEDs
#define LED_1_PIN_PORT              GPIOE
#define LED_1_PIN_NUM               LL_GPIO_PIN_0
#define LED_1_PIN                   LED_1_PIN_PORT, LED_1_PIN_NUM
#define LED_1_NAME                  "LED_1"

#define LED_2_PIN_PORT              GPIOE
#define LED_2_PIN_NUM               LL_GPIO_PIN_1
#define LED_2_PIN                   LED_2_PIN_PORT, LED_2_PIN_NUM
#define LED_2_NAME                  "LED_2"

// Reset = High
#define AUDIO_RESET_PIN_PORT        GPIOB
#define AUDIO_RESET_PIN_NUM         LL_GPIO_PIN_0
#define AUDIO_RESET_PIN             AUDIO_RESET_PIN_PORT, AUDIO_RESET_PIN_NUM
#define AUDIO_RESET_NAME            "AUDIO_RESET"

#define USB_POWER_EN_PIN_PORT       GPIOE
#define USB_POWER_EN_PIN_NUM        LL_GPIO_PIN_12
#define USB_POWER_EN_PIN            USB_POWER_EN_PIN_PORT, USB_POWER_EN_PIN_NUM
#define USB_POWER_EN_NAME           "USB_POWER_EN"

// PIN to trigger I2C EXTI
#define I2C_EXTI_PIN_PORT           GPIOC
#define I2C_EXTI_PIN_NUM            LL_GPIO_PIN_5
#define I2C_EXTI_PIN                I2C_EXTI_PIN_PORT, I2C_EXTI_PIN_NUM
#define I2C_EXTI_NAME               "I2C_EXTI"


// Default State HiZ

// Define as Open Drain
#define MAX10_RELOAD_PIN_PORT       GPIOD
#define MAX10_RELOAD_PIN_NUM        LL_GPIO_PIN_13
#define MAX10_RELOAD_PIN            MAX10_RELOAD_PIN_PORT, MAX10_RELOAD_PIN_NUM
#define MAX10_RELOAD_NAME           "MAX10_RELOAD"

#define DRACO_INT_OUT_PIN_PORT      GPIOD
#define DRACO_INT_OUT_PIN_NUM       LL_GPIO_PIN_15
#define DRACO_INT_OUT_PIN           DRACO_INT_OUT_PIN_PORT, DRACO_INT_OUT_PIN_NUM
#define DRACO_INT_OUT_NAME          "DRACO_INT_OUT"

// active low
#define DRACO_BOOT2ND_PIN_PORT      GPIOD
#define DRACO_BOOT2ND_PIN_NUM       LL_GPIO_PIN_1
#define DRACO_BOOT2ND_PIN           DRACO_BOOT2ND_PIN_PORT, DRACO_BOOT2ND_PIN_NUM
#define DRACO_BOOT2ND_NAME          "DRACO_BOOT2ND"

// Controlled by a DIP Switch, should be defined as HiZ
#define DRACO_UARTMUX_PIN_PORT      GPIOE
#define DRACO_UARTMUX_PIN_NUM       LL_GPIO_PIN_7
#define DRACO_UARTMUX_PIN           DRACO_UARTMUX_PIN_PORT, DRACO_UARTMUX_PIN_NUM
#define DRACO_UARTMUX_NAME          "DRACO_UARTMUX"

#define DRACO_CTS_PIN_PORT          GPIOC
#define DRACO_CTS_PIN_NUM           LL_GPIO_PIN_15
#define DRACO_CTS_PIN               DRACO_CTS_PIN_PORT, DRACO_CTS_PIN_NUM
#define DRACO_CTS_NAME              "DRACO_CTS"

// Phantom Power
#define PHAN_12V_ENUV_PIN_PORT      GPIOD
#define PHAN_12V_ENUV_PIN_NUM       LL_GPIO_PIN_5
#define PHAN_12V_ENUV_PIN           PHAN_12V_ENUV_PIN_PORT, PHAN_12V_ENUV_PIN_NUM
#define PHAN_12V_ENUV_NAME          "PHAN_12V_ENUV"

#define PHAN_5V_HPD_EN_PIN_PORT     GPIOD
#define PHAN_5V_HPD_EN_PIN_NUM      LL_GPIO_PIN_6
#define PHAN_5V_HPD_EN_PIN          PHAN_5V_HPD_EN_PIN_PORT, PHAN_5V_HPD_EN_PIN_NUM
#define PHAN_5V_HPD_EN_NAME         "PHAN_5V_HPD_EN"

#define PHAN_HPD_MASK_PIN_PORT      GPIOB
#define PHAN_HPD_MASK_PIN_NUM       LL_GPIO_PIN_8
#define PHAN_HPD_MASK_PIN           PHAN_HPD_MASK_PIN_PORT, PHAN_HPD_MASK_PIN_NUM
#define PHAN_HPD_MASK_NAME          "PHAN_HPD_MASK"


/***********************************
 * Component to Peripheral Mapping
 **********************************/

// Peripheral Mapping for LED Driver
#define LED_NCP_SCL_PIN             I2C1_SCL_PIN
#define LED_NCP_SDA_PIN             I2C1_SDA_PIN


/***********************************
 * ISR Priorities
 **********************************/

typedef enum
{
    GPIO_PRI_0,
    GPIO_PRI_1,
    GPIO_PRI_2,
    GPIO_PRI_3,
    GPIO_PRI_COUNT
} GPIO_EXTI_Piority_t;

#define TIMER_SYSTEM_CLOCK_PRIORITY     GPIO_PRI_0
#define TIMER_USB_CLOCK_PRIORITY        GPIO_PRI_1 // Should be higher priority than UART NVIC
#define I2C_EXTI_ISR_PRIORITY           GPIO_PRI_2
#define TIMER_PERIPHERAL_CLOCK_PRIORITY GPIO_PRI_3
#define UART_ISR_PRIORITY               GPIO_PRI_3
#define DRACO_ISR_PRIORITY              GPIO_PRI_3
#define TOKEN_LOFO_ISR_PRIORITY         GPIO_PRI_3


/*******************************************************************************
 * Public Declarations
 ******************************************************************************/

// Configure all GPIO Ports/Pins. Call this soon after power-up, before
// calling other peripheral's inits
void GPIO_config(void);

// Set given pin to a generic configuration of GPIO output pin
void GPIO_setupOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint32_t mode, uint32_t pullDirection);

// Set given pin to a generic configuration of GPIO EXTI Input
void GPIO_setupInputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint32_t pullDirection);

// Set given pin HIGH
void GPIO_setPin(GPIO_TypeDef *GPIOx, uint32_t PinMask);

// Set given pin LOW
void GPIO_resetPin(GPIO_TypeDef *GPIOx, uint32_t PinMask);

// Toggle Pin
void GPIO_toggle(GPIO_TypeDef *GPIOx, uint32_t PinMask);

// Start Profile
void GPIO_profileStart(void);

// End Profile
void GPIO_profileEnd(void);

// Convert from GPIO to the peripheral bitmask. This is used for init
uint32_t GPIO_toPeriph(GPIO_TypeDef *GPIOx);

// Determines what type of board this is from SIDE_DETECT_PIN.
// Pin is Set if RX, Reset if TX
bool GPIO_isRX(void);

// Determine if a pin previously setup as input pin is set or not
bool GPIO_isPinSet(GPIO_TypeDef *GPIOx, uint32_t PinMask);

// Configure pins for a specific I2C Port
void GPIO_setupI2C(I2C_Port_t port);

// Convert pin to EXTI Line
uint32_t GPIO_PinToExtiLine(uint32_t pin);

// Convert gpio pin to string representation
void GPIO_Pin2Str(char* name, GPIO_TypeDef *GPIOx, uint32_t PinMask);

// Get name of a GPIO Pin (if any)
char* GPIO_PinGetName(GPIO_TypeDef *GPIOx, uint32_t PinMask);

// Set all output GPIO-only pins to default values
int GPIO_SetOutputPinsToDefault(void);


// Internal LED toggle functionality.
#define LED1_ON()       GPIO_setPin(LED_1_PIN) // Used to show if a watchdog timeout occurred.
#define LED1_OFF()      GPIO_resetPin(LED_1_PIN)
#define LED2_ON()       GPIO_setPin(LED_2_PIN)
#define LED2_OFF()      GPIO_resetPin(LED_2_PIN)

#define DRACO_IS_UARTMUX_ENABLED() GPIO_isPinSet(DRACO_UARTMUX_PIN)

#define DRACO_UARTMUX_ENABLE() do { \
    GPIO_SetupOutputPullUp(DRACO_UARTMUX_PIN); \
    GPIO_setPin(DRACO_UARTMUX_PIN); \
} while(0)

#define DRACO_UARTMUX_DISABLE() do { \
    GPIO_SetupOutputPullUp(DRACO_UARTMUX_PIN); \
    GPIO_resetPin(DRACO_UARTMUX_PIN); \
} while(0)

#define DRACO_UARTMUX_DEFAULT() GPIO_SetupOutputHiZ(DRACO_UARTMUX_PIN)

// Internal switch functionality.
// TODO: Add switch functionality.

#endif /* _GPIO_H_ */

