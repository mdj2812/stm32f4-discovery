
#ifndef debug_UART_H
#define debug_UART_H

#include "stm32f4xx_hal.h"

// Declaraciones p�blicas del m�dulo

void debug_uart_init();
void debug_uart_send (uint8_t *data, uint16_t size);
void debug_uart_isr(void);
HAL_UART_StateTypeDef UART_GetState(void);

#endif 

