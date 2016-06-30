/**
  **********************************************************************************
  * File Name          : debug_uart.c
  * Description        : Configuration file to adapt to the board STM32F4-DISCOVERY. 
  **********************************************************************************
  */

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"

#include "debug_uart.h"

extern UART_HandleTypeDef huartx;

extern void debug_uart_isr_rx (void);
extern void debug_uart_isr_tx (void);
extern void debug_uart_isr_begin (void);
extern void debug_uart_isr_end (void);

 /* Funcion vacía declarada "weak" para ser reemplazada por otra con el mismo nombre
    pero con funcionalidad*/
__weak void debug_uart_isr_rx (void){}

__weak void debug_uart_isr_tx (void){}

__weak void debug_uart_isr_begin (void){}

__weak void debug_uart_isr_end (void){}

void debug_uart_send (uint8_t *data, uint16_t size) {
  /* Process Locked */
  (&huartx)->pTxBuffPtr = data;
  (&huartx)->TxXferSize = size;
  (&huartx)->TxXferCount = size;

  (&huartx)->ErrorCode = HAL_UART_ERROR_NONE;
  (&huartx)->gState = HAL_UART_STATE_BUSY_TX;
  
  /* Enable the UART Transmit data register empty Interrupt */
  SET_BIT((&huartx)->Instance->CR1, USART_CR1_TXEIE);
}

void debug_uart_init() {

  huartx.Instance = USART2;
  huartx.Init.BaudRate = 9600;
  huartx.Init.WordLength = UART_WORDLENGTH_8B;
  huartx.Init.StopBits = UART_STOPBITS_1;
  huartx.Init.Parity = UART_PARITY_NONE;
  huartx.Init.Mode = UART_MODE_TX_RX;
  huartx.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huartx.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huartx) != HAL_OK)
  {
    puts("***ERROR*** uart failed to initiate \r\n");
  }
}

void debug_uart_isr()
{  
  debug_uart_isr_begin ();
    
  /* UART in mode Receiver -------------------------------------------------*/
  if(HAL_UART_GetState(&huartx) == HAL_UART_STATE_BUSY_RX)
  {
    debug_uart_isr_rx();
  }
  
  /* UART in mode Transmitter ------------------------------------------------*/
  if(HAL_UART_GetState(&huartx) == HAL_UART_STATE_BUSY_TX)
  {
    debug_uart_isr_tx();
  }
  
  debug_uart_isr_end();
}

HAL_UART_StateTypeDef UART_GetState(void)
{
  return HAL_UART_GetState(&huartx);
}
