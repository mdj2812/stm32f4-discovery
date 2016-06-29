
#ifndef debug_UART_H
#define debug_UART_H


// Declaraciones públicas del módulo

void debug_uart_init();
void debug_uart_send (uint8_t data);
void debug_uart_isr(void);

__weak void debug_uart_isr_rx (void);

__weak void debug_uart_isr_tx (void);

__weak void debug_uart_isr_begin (void);

__weak void debug_uart_isr_end (void);

#endif 

