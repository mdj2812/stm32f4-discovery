/**
  ******************************************************************************
  * @file       KNX_PH_TPUart.c
  * @author     MA Dingjie
  * @version    V1.0.0
  * @date       6-September-2016
  * @brief      KNX Physical Layer communication via TP-UART.
  *             This file provides functions to manage following functionalities:
  *              + Initialization functions
  *              + Send and Receive messages functions
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "KNX_Aux.h"
#include "KNX_Ph_TPUart.h"
#include "stm32f4xx_hal.h"

/** @addtogroup KNX_Lib
  * @{
  */

/** @addtogroup KNX_PH KNX Physical Layer
  * @{
  */

/** @defgroup KNX_PH_TPUart KNX Physical Layer TPUart
  * @{
  */

/* External functions --------------------------------------------------------*/
/** @defgroup   KNX_PH_TPUart_External_Functions KNX PH TPUART External Functions
  * @brief      External functions to be used in \ref KNX_PH_Sup module
  * @{
  */
extern void knx_uart_isr_begin (void);
extern void knx_uart_isr_end (void);
extern void knx_uart_isr_rx (void);
extern void knx_uart_isr_tx (void);
/**
  * @}
  */

/* Private variables --------------------------------------------------------*/
/** @defgroup KNX_PH_TPUART_Private_Variables KNX TPUart Private Variables
  * @{
  */
/** \brief UART Handler */
UART_HandleTypeDef knx_huart;
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @defgroup KNX_PH_TPUart_Exported_Functions KNX Physical Layer TPUart Public Functions
  * @{
  */

/** @defgroup KNX_PH_TPUart_Exported_Functions_Group1 KNX TPUart Initialization Functions
  * @{
  */

/**
  * @brief      Initialization of the uart handler ::knx_huart. 
  * @retval     ::TPUart_OK for a successful initialization, while ::TPUart_ERROR
  *             for failed.        
  */
uint8_t KNX_PH_TPUart_init(void)
{
  knx_huart.Instance = USART3;
  knx_huart.Init.BaudRate = 19200;
  knx_huart.Init.WordLength = UART_WORDLENGTH_9B;
  knx_huart.Init.StopBits = UART_STOPBITS_1;
  knx_huart.Init.Parity = UART_PARITY_EVEN;
  knx_huart.Init.Mode = UART_MODE_TX_RX;
  knx_huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  knx_huart.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&knx_huart) != HAL_OK)
  {
    puts("***ERROR*** knx_uart failed to initiate \r\n");
    return TPUart_ERROR;
  }
  
  __HAL_UART_ENABLE_IT(&knx_huart, UART_IT_RXNE);  /** Activate Flag Receptie */
  __HAL_UART_ENABLE_IT(&knx_huart, UART_IT_TC);    /** Activate Flag TX       */
  
  HAL_Delay(1000);
  /* Set the RESn Pin to 1 and toggle it to 0 to start the reset behavior of the TP-UART2 */
  HAL_GPIO_WritePin(GPIOD, TPUART_RESn_Pin, GPIO_PIN_SET);
  /* Wait for 1 s */
  HAL_Delay(50);
  HAL_GPIO_TogglePin(GPIOD, TPUART_RESn_Pin);
  HAL_Delay(50);
  HAL_GPIO_DeInit(GPIOD, TPUART_RESn_Pin);
  
  return TPUart_OK;
}
/**
  * @}
  */

/** @defgroup KNX_PH_TPUart_Exported_Functions_Group2 KNX TPUart Send/Receive Functions
  * @{
  */

/**
  * @brief      Send the data through UART.
  * @param      data:  pointer to the data buffer.
  * @param      size:  amount of data to be sent.
  * @retval     ::TPUart_Status_t according to the status
  */
uint8_t KNX_PH_TPUart_Send(uint8_t *data, uint16_t size)
{
  if((&knx_huart)->gState == HAL_UART_STATE_READY)
  {
    if((data == NULL ) || (size == 0U)) 
    {
      return TPUart_ERROR;
    }
  
    /* Process Locked */
    if((&knx_huart)->Lock == HAL_LOCKED)
    {
      return TPUart_BUSY;
    }
    else
    {
      (&knx_huart)->Lock = HAL_LOCKED;
    }
    
    (&knx_huart)->pTxBuffPtr = data;
    (&knx_huart)->TxXferSize = size;
    (&knx_huart)->TxXferCount = size;

    (&knx_huart)->ErrorCode = HAL_UART_ERROR_NONE;
    (&knx_huart)->gState = HAL_UART_STATE_BUSY_TX;
    
    /* Process Unlocked */
    (&knx_huart)->Lock = HAL_UNLOCKED;
    
    /* Enable the UART Transmit data register empty Interrupt */
    SET_BIT((&knx_huart)->Instance->CR1, USART_CR1_TXEIE);
    
    return TPUart_OK;
  }
  else
  {
    return TPUart_BUSY;   
  }

}

/**
  * @brief      Receive the data through UART.
  * @param      data:  pointer to the data buffer.
  * @param      size:  amount of data to be received.
  * @retval     ::TPUart_Status_t according to the status
  */
uint8_t KNX_PH_TPUart_Receive(uint8_t *data, uint16_t size)
{
  /* Check that a Rx process is not already ongoing */ 
  if((&knx_huart)->RxState == HAL_UART_STATE_READY)
  {
    if((data == NULL ) || (size == 0U)) 
    {
      return TPUart_ERROR;
    }
    
    /* Process Locked */
    if((&knx_huart)->Lock == HAL_LOCKED)
    {
      return TPUart_BUSY;
    }
    else
    {
      (&knx_huart)->Lock = HAL_LOCKED;
    }
    
    (&knx_huart)->pRxBuffPtr = data;
    (&knx_huart)->RxXferSize = size;
    (&knx_huart)->RxXferCount = size;
    
    (&knx_huart)->ErrorCode = HAL_UART_ERROR_NONE;
    (&knx_huart)->RxState = HAL_UART_STATE_BUSY_RX;
    
    /* Process Unlocked */
    (&knx_huart)->Lock = HAL_UNLOCKED;
    
    /* Enable the UART Parity Error Interrupt */
    SET_BIT((&knx_huart)->Instance->CR1, USART_CR1_PEIE);
    
    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT((&knx_huart)->Instance->CR3, USART_CR3_EIE);

    /* Enable the UART Data Register not empty Interrupt */
    SET_BIT((&knx_huart)->Instance->CR1, USART_CR1_RXNEIE);
    
    return TPUart_OK;
  }
  else
  {
    return TPUart_BUSY; 
  }
}
/**
  * @}
  */

/** @defgroup KNX_PH_TPUart_Exported_Functions_Group3 TPUART Interrupt function
  * @{
  */

/**
  * @brief      UART interrupt routines.
  */
void TPUart_isr(void)
{  
  knx_uart_isr_begin();
    
  /* UART in mode Receiver ---------------------------------------------------*/
  knx_uart_isr_rx();
  
  /* UART in mode Transmitter ------------------------------------------------*/
  knx_uart_isr_tx();
  
  knx_uart_isr_end();
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
