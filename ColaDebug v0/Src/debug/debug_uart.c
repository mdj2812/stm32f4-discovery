/**
  **********************************************************************************
  * File Name          : debug_uart.c
  * Description        : Configuration file to adapt to the board STM32F4-DISCOVERY. 
  **********************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "debug_uart.h"

/* Extern variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huartx;

/* Extern functions ---------------------------------------------------------*/
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

/**
  * @brief  Sends the data to UART.
  * @param  data: Pointer to data buffer
  * @param	size: size of the data buffer
  * @retval None
  */
void debug_uart_send (uint8_t *data, uint16_t size) {
  HAL_UART_Transmit_IT(&huartx, data, size);
}

/**
  * @brief  Initialize the uart pins
  */
void debug_uart_init(void) {

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

/**
  * @brief  UART interrupt routine
  */
void debug_uart_isr(void)
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
