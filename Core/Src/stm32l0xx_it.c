/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */
long timeMs=0;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
  timeMs++;
  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32L0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
  if (LL_USART_IsActiveFlag_RXNE(USART2))
  {
      rxFifoWrite(LL_USART_ReceiveData8(USART2));
  }

  /* Clear overrun error flag */
  if (LL_USART_IsActiveFlag_ORE(USART2))
    {
      LL_USART_ClearFlag_ORE(USART2);
    }
  /* USER CODE END USART2_IRQn 0 */
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE BEGIN 1 */
#define FIFO_LENGTH 30
struct s_fifo
{
   int readPos;
   int writePos;
   int count;
   uint8_t buffer[FIFO_LENGTH];
}g_rxFifo;
void rxFifoInit(void)
{
  g_rxFifo.readPos=0;
  g_rxFifo.writePos=0;
  g_rxFifo.count=0;
}
void rxFifoWrite(uint8_t p_car)
{
 if (g_rxFifo.count==FIFO_LENGTH) return;  // buffer full
 g_rxFifo.buffer[g_rxFifo.writePos]=p_car;
 g_rxFifo.writePos++;
 g_rxFifo.count++;
 if (g_rxFifo.writePos==FIFO_LENGTH) g_rxFifo.writePos=0;
}
int rxFifoRead(void)
{
  int l_car;
  __disable_irq();
  if (g_rxFifo.count==0)
  {
      __enable_irq();
      return -1; //empty
  }
  l_car=g_rxFifo.buffer[g_rxFifo.readPos];
  g_rxFifo.count--;
  __enable_irq();
  g_rxFifo.readPos++;
  if (g_rxFifo.readPos==FIFO_LENGTH) g_rxFifo.readPos=0;
  return l_car;
}

void getchInit(void)
{
}

//Return  a char received on UART2
// Wait if none received yet
uint8_t getch(void)
{
  int l_car=0;
  do
  {
      l_car=rxFifoRead();
      if (l_car==-1)
      {
	  __WFI(); //wait for interrupt (for a key ==> data RX)
      }
  }
  while(l_car==-1); //while no char received
  return (uint8_t)l_car; //return received data
}
//Return 1 if a char received on UART2
// else 0
int kbhit(void)
{
 if(g_rxFifo.count==0)
   return 0;
 else
   return 1;
}
void serialWrite(uint8_t p_car)
{
  while(LL_USART_IsActiveFlag_TXE(USART2)==0);
  LL_USART_TransmitData8(USART2,p_car);
}

/* USER CODE END 1 */
