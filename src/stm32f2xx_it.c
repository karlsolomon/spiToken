/**
  ******************************************************************************
  * @file    Demonstrations/Src/stm32f2xx_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_it.h"
#include "stm32f2xx.h"
#include "stm32f2xx_ll_usart.h"
#include "stm32f2xx_ll_exti.h"
#include "stm32f2xx_ll_tim.h"
#include "stm32f2xx_hal_pcd.h"
#include "stm32f2xx_hal.h"
#include "UART.h"
#include "Timer.h"
#include "LED.h"
#include "Token.h"
#include "GPIO.h"
#include "Amimon.h"
#include "Watchdog.h"
#include "I2C.h"
#include "Debug.h"

#ifndef BOOTLOADER
    #include "Silex.h"
    #include "USB.h"
#endif

/** @addtogroup STM32F2xx_HAL_Examples
  * @{
  */

/** @addtogroup GPIO_EXTI
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define ERROR_BLINK_SPEED Timer_250MS
#define ERROR_RESET_SPEED Timer_1SEC

#define ERROR_BLINK_NUM_HARDFAULT 1
#define ERROR_BLINK_NUM_USAGEFAULT 2
#define ERROR_BLINK_NUM_BUSFAULT 3
#define ERROR_BLINK_NUM_MEMMEXCEPTION 4
#define ERROR_BLINK_NUM_PENDSVEXCEPTION 5
#define ERROR_BLINK_NUM_DEBUGMONEXCEPTION 6
#define ERROR_BLINK_NUM_SVCEXCEPTION 7
#define ERROR_BLINK_NUM_NMIEXCEPTION 8


// Blink Error LED a set number of times
static void blinkNTimes(uint8_t numTimesToBlink);

extern PCD_HandleTypeDef hpcd_USB_OTG_HS;

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/***********************************
 * Red Token Ring Exceptions
 **********************************/
/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    LED1_ON();
    LED_SetColorUnsafe(LED_COLOR_ERROR);
    TRACE_ERROR("Hard Fault");
    while (1)
    {
        blinkNTimes(ERROR_BLINK_NUM_HARDFAULT);
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    LED1_ON();
    LED_SetColorUnsafe(LED_COLOR_ERROR);
    TRACE_ERROR("Usage Fault");
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
        blinkNTimes(ERROR_BLINK_NUM_USAGEFAULT);
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    LED1_ON();
    LED_SetColorUnsafe(LED_COLOR_ERROR);
    TRACE_ERROR("Bus Fault");
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
        blinkNTimes(ERROR_BLINK_NUM_BUSFAULT);
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    LED1_ON();
    LED_SetColorUnsafe(LED_COLOR_ERROR);
    TRACE_ERROR("Memory Manage Exception");
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
        blinkNTimes(ERROR_BLINK_NUM_MEMMEXCEPTION);
    }
}


/***********************************
 * Magenta Token Ring Exceptions
 **********************************/

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
    LED1_ON();
    LED_SetColorUnsafe(LED_COLOR_ERROR);
    TRACE_ERROR("PendSVC Exception");
    while (1)
    {
        blinkNTimes(ERROR_BLINK_NUM_PENDSVEXCEPTION);
    }
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
    LED1_ON();
    LED_SetColorUnsafe(LED_COLOR_ERROR);
    TRACE_ERROR("Debug Monitor Exception");
    while (1)
    {
        blinkNTimes(ERROR_BLINK_NUM_DEBUGMONEXCEPTION);
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
    LED1_ON();
    LED_SetColorUnsafe(LED_COLOR_ERROR);
    TRACE_ERROR("SVCall Exception");
    while (1)
    {
        blinkNTimes(ERROR_BLINK_NUM_SVCEXCEPTION);
    }
}

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
    LED1_ON();
    LED_SetColorUnsafe(LED_COLOR_ERROR);
    TRACE_ERROR("NMI Exception");
    while (1)
    {
        blinkNTimes(ERROR_BLINK_NUM_NMIEXCEPTION);
    }
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f2xx.s).                                               */
/******************************************************************************/

/*******************************************************************************
 * @brief TIM2_IRQHandler
 *
 * ISR for 1ms timer -- for general purpose timer
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
void TIM2_IRQHandler(void)
{
    /* Check whether update interrupt is pending */
    if(LL_TIM_IsActiveFlag_UPDATE(TIM2) == 1)
    {
    	/* Clear the update interrupt flag*/
        Timer_UpdateCallback();
        LL_TIM_ClearFlag_UPDATE(TIM2);
    }
}

#ifndef BOOTLOADER
/*******************************************************************************
 * @brief TIM3_IRQHandler
 *
 * ISR for Silex timer
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
void TIM3_IRQHandler(void)
{
    /* Check whether update interrupt is pending */
    if(LL_TIM_IsActiveFlag_UPDATE(TIM3) == 1)
    {
        /* Clear the update interrupt flag*/
        Silex_ScanCallback();
        LL_TIM_ClearFlag_UPDATE(TIM3);
    }
}

/*******************************************************************************
 * @brief OTG_HS_IRQHandler
 *
 * This function handles USB On The Go HS global interrupt.
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
void OTG_HS_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_HS);
}

/*******************************************************************************
 * @brief EXTI0_IRQHandler
 *
 * ISR for Draco Event Register Change
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
void EXTI0_IRQHandler(void)
{
    if (LL_EXTI_IsActiveFlag_0_31(GPIO_PinToExtiLine(DRACO_INT_IN_PIN_NUM)) != RESET)
    {
        if(GPIO_isPinSet(DRACO_INT_IN_PIN))
        {
            AMIMON_EventRegisterISR();
        }
        LL_EXTI_ClearFlag_0_31(GPIO_PinToExtiLine(DRACO_INT_IN_PIN_NUM));
    }
}

/*******************************************************************************
 * @brief TIM6_DAC_IRQHandler
 *
 * ISR for USB Timer Callback
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
void TIM6_DAC_IRQHandler(void)
{
    /* Check whether update interrupt is pending */
    if(LL_TIM_IsActiveFlag_UPDATE(TIM6) == 1)
    {
        /* Clear the update interrupt flag*/
        USB_WriteCallback();
        LL_TIM_ClearFlag_UPDATE(TIM6);
    }    
}

#endif

/*******************************************************************************
 * @brief TIM5_IRQHandler
 *
 * ISR for LED Driver Dim/Blink
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
void TIM5_IRQHandler(void)
{
  /* Check whether update interrupt is pending */
  if(LL_TIM_IsActiveFlag_UPDATE(TIM5) == 1)
  {
    // disable timer s/t ISR doesn't get triggered while in ISR.
    // This is bad practice, ISRs shouldn't take this long
    // However, this is necessary for given LED Driver since we must send 3 cmds
    LL_TIM_DisableCounter(TIM5);
    LED_TimerCallback();
    LL_TIM_EnableCounter(TIM5);

    /* Clear the update interrupt flag*/
    LL_TIM_ClearFlag_UPDATE(TIM5);
  }
}

/*******************************************************************************
 * @brief TIM7_IRQHandler
 *
 * ISR for 200ms timer -- for token debounce
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
void TIM7_IRQHandler(void)
{
    /* Check whether update interrupt is pending */
    if(LL_TIM_IsActiveFlag_UPDATE(TIM7) == 1)
    {
        Token_DebounceCallback();
        /* Clear the update interrupt flag*/
        LL_TIM_ClearFlag_UPDATE(TIM7);
    }
}

/*******************************************************************************
 * @brief TIM4_IRQHandler
 *
 * ISR for 1 second timer -- for watchdog reset when in menu mode.
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
void TIM4_IRQHandler(void)
{
    /* Check whether update interrupt is pending */
    if(LL_TIM_IsActiveFlag_UPDATE(TIM4) == 1)
    {
        // Reset IWDG.
        IWDG_Refresh();

        /* Clear the update interrupt flag*/
        LL_TIM_ClearFlag_UPDATE(TIM4);
    }
}

/*******************************************************************************
 * @brief EXTI9_5_IRQHandler
 *
 * ISR for I2C Communication
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
    if (LL_EXTI_IsActiveFlag_0_31(GPIO_PinToExtiLine(I2C_EXTI_PIN_NUM)) != RESET)
    {
        if(GPIO_isPinSet(I2C_EXTI_PIN))
        {
            I2C_Callback();
            GPIO_resetPin(I2C_EXTI_PIN);
        }
        LL_EXTI_ClearFlag_0_31(GPIO_PinToExtiLine(I2C_EXTI_PIN_NUM));
    }
}

/*******************************************************************************
 * @brief EXTI15_10_IRQHandler
 *
 * EXTI ISR for token connect/disconnect detect (LOFO)
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
    if (LL_EXTI_IsActiveFlag_0_31(GPIO_PinToExtiLine(TOKEN_LOFO_PIN_NUM)) != RESET)
    {
        if(GPIO_isPinSet(TOKEN_LOFO_PIN))
        {
            Token_LofoISR(TOKEN_REMOVED);
        }
        else
        {
            Token_LofoISR(TOKEN_INSERTED);
        }
        LL_EXTI_ClearFlag_0_31(GPIO_PinToExtiLine(TOKEN_LOFO_PIN_NUM));
    }
}

/*******************************************************************************
 * @brief blinkNTimes
 *
 * Blink Error LED a set number of times
 *
 * @param  > None
 *
 * @return None
 ******************************************************************************/
static void blinkNTimes(uint8_t numTimesToBlink)
{
    for(uint8_t i = 0; i < numTimesToBlink; i++)
    {
        LED2_ON();
        Timer_Sleep(ERROR_BLINK_SPEED);
        LED2_OFF();
        Timer_Sleep(ERROR_BLINK_SPEED);
    }
    Timer_Sleep(ERROR_RESET_SPEED);
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
