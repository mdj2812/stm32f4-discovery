#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"

#include "debug_uart.h"

#define NVIC_INT2IRQ(x)                         (x-16)

static USART_TypeDef * uart_base = 0;

#define UART_LCRH_WLEN_VAL		UART_LCRH_WLEN_8
#define INT_BAUD_RATE			9600

UART_HandleTypeDef huartx;

extern void debug_uart_isr_rx (void);
extern void debug_uart_isr_tx (void);
extern void debug_uart_isr_begin (void);
extern void debug_uart_isr_end (void);

/*
#pragma weak debug_uart_isr_rx
 * Funcion vacía declarada "weak" para ser reemplazada por otra con el mismo nombre
    pero con funcionalidad
void debug_uart_isr_rx (void){}

#pragma weak debug_uart_isr_tx
void debug_uart_isr_tx (void){}

#pragma weak debug_uart_isr_begin
void debug_uart_isr_begin (void){}

#pragma weak debug_uart_isr_end
void debug_uart_isr_end (void){}
*/

//uint32_t uart_base = 0;

void debug_uart_send (uint8_t *data) {
   if (uart_base == 0) {
        return;
   }
   HAL_UART_Transmit_IT(&huartx, data, sizeof(data));
}

static void _uart_enable_interrupts_nvic (which_uart_t which_uart) {
    HAL_NVIC_EnableIRQ(USART2_IRQn);
}

static void _uart_disable_interrupts_nvic (which_uart_t which_uart) {
    HAL_NVIC_DisableIRQ(USART2_IRQn);
}

static USART_TypeDef * _select_uart(which_uart_t which_uart) {
    switch (which_uart) {
        case UART_1:
            return USART1;
        case UART_2:
            return USART2;
        case UART_3:
            return USART3;
        default: return 0; break;
    }
}

static void _uart_enable_hw(which_uart_t which_uart){
  GPIO_InitTypeDef GPIO_InitStruct;
  
  switch (which_uart)
  {
     case UART_1:
        /* Peripheral clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();
      
        /**USART2 GPIO Configuration    
        PA9     ------> USART1_TX
        PA10    ------> USART1_RX 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        break;
      case UART_2:
        /* Peripheral clock enable */
        __HAL_RCC_USART2_CLK_ENABLE();
      
        /**USART2 GPIO Configuration    
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        break;
      case UART_3:
        /* Peripheral clock enable */
        __HAL_RCC_USART3_CLK_ENABLE();
      
        /**USART2 GPIO Configuration    
        PB10    ------> USART3_TX
        PB11    ------> USART3_RX 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        break;
      default:
        break;
  }
}

void debug_uart_init(which_uart_t which_uart) {
    uart_base = _select_uart(which_uart);
    if (uart_base == 0) {
        return;
    }

    // Habilitamos HW (controlador y pines)
    _uart_enable_hw(which_uart);

/*
    UART deshabilitada. Configurar todo aquí */

    huartx.Instance = uart_base;
    huartx.Init.BaudRate = INT_BAUD_RATE;
    huartx.Init.WordLength = UART_WORDLENGTH_8B;
    huartx.Init.StopBits = UART_STOPBITS_1;
    huartx.Init.Parity = UART_PARITY_NONE;
    huartx.Init.Mode = UART_MODE_TX_RX;
    huartx.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huartx.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huartx);
    
    // Rehabilitar interrupciones UART en el controlador de interrupciones???
    // ahora que la UART está lista y hemos borrado el flag TXIFG*/
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    _uart_enable_interrupts_nvic (which_uart);
}

void debug_uart_isr (void){

  HAL_UART_StateTypeDef state;
  if (uart_base == 0){
    return;
  }
  debug_uart_isr_begin ();

  state = HAL_UART_GetState(&huartx);
    if((state&HAL_UART_STATE_BUSY_TX)==HAL_UART_STATE_BUSY_TX){
    	debug_uart_isr_tx();
    }
    if((state&HAL_UART_STATE_BUSY_RX)==HAL_UART_STATE_BUSY_RX){
    	debug_uart_isr_rx();
    }
  debug_uart_isr_end ();
}
