
#ifndef debug_UART_H
#define debug_UART_H


// Declaraciones públicas del módulo

enum which_uart {UART_1 = 0, UART_2 = 1, UART_3 = 2};
typedef enum which_uart which_uart_t;
void debug_uart_init(which_uart_t which_uart);
void debug_uart_send (uint8_t *data);


#pragma weak debug_uart_isr_rx
void debug_uart_isr_rx (void);

#pragma weak debug_uart_isr_tx
void debug_uart_isr_tx (void);

#pragma weak debug_uart_isr_begin
void debug_uart_isr_begin (void);

#pragma weak debug_uart_isr_end
void debug_uart_isr_end (void);

void debug_uart_isr (void);


#endif 

