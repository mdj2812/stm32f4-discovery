#include <stdint.h>
#include "hw_types.h"
#include "hw_nvic.h"
#include "hw_sysctl.h"
#include "hw_ints.h"
#include "hw_uart.h"
#include "hw_gpio.h"
#include "hw_memmap.h"
#include "FreeRTOS.h"

#include "debug_uart.h"

#define NVIC_INT2IRQ(x)                         (x-16)

static uint32_t uart_base = 0;

#define GPIO_PIN_0		 		(0x1<<0)
#define GPIO_PIN_1				(0x1<<1)
#define GPIO_PIN_2				(0x1<<2)
#define GPIO_PIN_3				(0x1<<3)

#define UART_LCRH_WLEN_VAL		UART_LCRH_WLEN_8
#define INT_BAUD_RATE			325
#define FRAC_BAUD_RATE			33

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

void debug_uart_send (uint8_t data) {
   if (uart_base == 0) {
        return;
   }
   HWREG(uart_base + UART_O_DR) = data;
}

static void _uart_enable_interrupts_nvic (which_uart_t which_uart) {
    HWREG(NVIC_EN0) = 1 << (NVIC_INT2IRQ((int)INT_UART0) + (int)which_uart);
}

static void _uart_disable_interrupts_nvic (which_uart_t which_uart) {
    HWREG(NVIC_DIS0) = 1 << ((int)INT_UART0-16 + (int)which_uart);
}

static uint32_t _select_uart(which_uart_t which_uart) {
    switch (which_uart) {
        case UART_A0:
            return ((uint32_t) UART0_BASE);
        case UART_A1:
            return ((uint32_t) UART1_BASE);
        default: return 0; break;
    }
}

static void _uart_enable_hw(which_uart_t which_uart){
	volatile unsigned long ulLoop;

	switch (which_uart){
	case UART_A0:
		//
        // Enable the UART0 controller
        //
        HWREG(SYSCTL_RCGC1) = SYSCTL_RCGC1_UART0;
        //
        // Enable the GPIO port that is used for the on-board LED.
        //
        HWREG(SYSCTL_RCGC2) = SYSCTL_RCGC2_GPIOA;
        //
        // Do a dummy read to insert a few cycles after enabling the peripheral.
        //
        ulLoop = HWREG(SYSCTL_RCGC2);
        ulLoop = HWREG(SYSCTL_RCGC2);
        ulLoop = HWREG(SYSCTL_RCGC2);
	
//DEN->1, ODR->0, PUL->0, DIR->0, DR2R->1 (2mA Fuerza), DR4R->0, DR8R->0, SLR->0

        HWREG(GPIO_PORTA_BASE+GPIO_O_AFSEL)  |= GPIO_PIN_0; // UART0 Rx
        HWREG(GPIO_PORTA_BASE+GPIO_O_AFSEL)  |= GPIO_PIN_1; // UART0 Tx
        HWREG(GPIO_PORTA_BASE+GPIO_O_ODR)    &= ~(GPIO_PIN_0 | GPIO_PIN_1); // ODR bits = 0
        HWREG(GPIO_PORTA_BASE+GPIO_O_PUR)    &= ~(GPIO_PIN_0 | GPIO_PIN_1); // ODR bits = 0
        HWREG(GPIO_PORTA_BASE+GPIO_O_PDR)    &= ~(GPIO_PIN_0 | GPIO_PIN_1); // ODR bits = 0
        HWREG(GPIO_PORTA_BASE+GPIO_O_DEN)    |= GPIO_PIN_0 | GPIO_PIN_1;    // DEN bits = 1
        HWREG(GPIO_PORTA_BASE+GPIO_O_DR2R)   |= GPIO_PIN_0 | GPIO_PIN_1;    // DR2R bits = 1 => 2mA drive strengh
        HWREG(GPIO_PORTA_BASE+GPIO_O_DR4R)   &= ~(GPIO_PIN_0 | GPIO_PIN_1); // DR4R bits = 0
        HWREG(GPIO_PORTA_BASE+GPIO_O_DR8R)   &= ~(GPIO_PIN_0 | GPIO_PIN_1); // DR8R bits = 0
        HWREG(GPIO_PORTA_BASE+GPIO_O_SLR)    &= ~(GPIO_PIN_0 | GPIO_PIN_1); // SLR bits = 0
        break;
	case UART_A1:
		//
	    // Enable the UART1 controller
	    //
	    HWREG(SYSCTL_RCGC1) = SYSCTL_RCGC1_UART1;
	    //
	    // Enable the GPIO port that is used for the on-board LED.
	    //
	    HWREG(SYSCTL_RCGC2) = SYSCTL_RCGC2_GPIOD;
	    //
	    // Do a dummy read to insert a few cycles after enabling the peripheral.
	    //
	    ulLoop = HWREG(SYSCTL_RCGC2);
	    ulLoop = HWREG(SYSCTL_RCGC2);
	    ulLoop = HWREG(SYSCTL_RCGC2);
	    HWREG(GPIO_PORTD_BASE+GPIO_O_AFSEL)  |= GPIO_PIN_2; // UART1 Rx
	    HWREG(GPIO_PORTD_BASE+GPIO_O_AFSEL)  |= GPIO_PIN_3; // UART1 Tx
            HWREG(GPIO_PORTD_BASE+GPIO_O_ODR)    &= ~(GPIO_PIN_2 | GPIO_PIN_3); // ODR bits = 0
            HWREG(GPIO_PORTD_BASE+GPIO_O_PUR)    &= ~(GPIO_PIN_2 | GPIO_PIN_3); // ODR bits = 0
            HWREG(GPIO_PORTD_BASE+GPIO_O_PDR)    &= ~(GPIO_PIN_2 | GPIO_PIN_3); // ODR bits = 0
            HWREG(GPIO_PORTD_BASE+GPIO_O_DEN)    |= GPIO_PIN_2 | GPIO_PIN_3;    // DEN bits = 1
            HWREG(GPIO_PORTD_BASE+GPIO_O_DR2R)   |= GPIO_PIN_2 | GPIO_PIN_3;    // DR2R bits = 1 => 2mA drive strengh
            HWREG(GPIO_PORTD_BASE+GPIO_O_DR4R)   &= ~(GPIO_PIN_2 | GPIO_PIN_3); // DR4R bits = 0
            HWREG(GPIO_PORTD_BASE+GPIO_O_DR8R)   &= ~(GPIO_PIN_2 | GPIO_PIN_3); // DR8R bits = 0
            HWREG(GPIO_PORTD_BASE+GPIO_O_SLR)    &= ~(GPIO_PIN_2 | GPIO_PIN_3); // SLR bits = 0
	    break;
	default:
		break;
	}
}

void debug_uart_init(which_uart_t which_uart) {

	uint32_t ulTemp;
	uint32_t nvic_prio_byte;
	uint32_t nvic_prio_shift;


    uart_base = _select_uart(which_uart);
    if (uart_base == 0) {
        return;
    }

    // Deshabilitar interrupciones UART en el controlador de interrupciones
    // por si estuviera en uso
    _uart_disable_interrupts_nvic(which_uart);

	// Habilitamos HW (controlador y pines)
    _uart_enable_hw(which_uart);

    HWREG(uart_base+UART_O_CTL) &= ~UART_CTL_UARTEN;		// DESACTIVA UART para cambiar configuración
    // Esperar el fin de la transmisión en curso (si la hubiera)
    while ((HWREG(uart_base+UART_O_FR) & UART_FR_BUSY) == UART_FR_BUSY) {
    }
    // OJO: Falta esperar el fin de la recepción en curso (si la hubiera)
    // PERO el micro no ofrece ninguna información de estado para indicar
    // que hay una recepción en curso y no queremos esperar indefinidamente ...

	// Flush de la transmit FIFO
    HWREG(uart_base+UART_O_LCRH) &= ~UART_LCRH_FEN;  		// DESACTIVA FIFO


/*
    UART deshabilitada. Configurar todo aquí

    * 1- Modo de trabajo: eUSCI modo UART asincrono, 8 bits, sin paridad,
         1 bit de stop, no IRDA, ...
         LSB-first (*no* configurable)
           Registros a usar:  CTL, LCRH */

    HWREG(uart_base+UART_O_CTL) |= UART_CTL_RXE;			// Activar recepción y transmisión
    HWREG(uart_base+UART_O_CTL) |= UART_CTL_TXE;			//
    HWREG(uart_base+UART_O_CTL) &= ~UART_CTL_LBE; 			// Loopback mode disable
    HWREG(uart_base+UART_O_CTL) &= ~UART_CTL_SIREN;			// no IRDA

    HWREG(uart_base+UART_O_LCRH) &= ~UART_LCRH_BRK;  		// No enviar break
    HWREG(uart_base+UART_O_LCRH) &= ~UART_LCRH_PEN;  		// DESACTIVA PARIDAD
    HWREG(uart_base+UART_O_LCRH) &= ~UART_LCRH_STP2; 		// 1 bit de stop
    HWREG(uart_base+UART_O_LCRH) &= ~UART_LCRH_FEN;		// DESACTIVA FIFO (¿NECESARIO?)
    HWREG(uart_base+UART_O_LCRH) &= ~UART_LCRH_WLEN_M;		// LONGITUD WORD (MÁSCARA)
    HWREG(uart_base+UART_O_LCRH) |=  UART_LCRH_WLEN_VAL; 	// LONGITUD WORD (VALOR)
    HWREG(uart_base+UART_O_LCRH) &= ~UART_LCRH_SPS;  		// DESACTIVA PARIDAD sticky

    /* 2- Velocidad de comunicaciones fija: 9600 bps (bits por segundo)
         usando SYSCLK a 20 MHz
         Registros a usar:  IBRD, FBRD
	*/
    HWREG(uart_base+UART_O_IBRD) &= ~UART_IBRD_DIVINT_M;	// PARTE ENTERA MASK
    HWREG(uart_base+UART_O_IBRD)  =  INT_BAUD_RATE;			// PARTE ENTERA WRITE

    HWREG(uart_base+UART_O_FBRD) &= ~UART_FBRD_DIVFRAC_M;	// PARTE FRACCIONARIA MASK
    HWREG(uart_base+UART_O_FBRD)  =  FRAC_BAUD_RATE;		// PARTE FRACCIONARIA MASK

    HWREG(uart_base+UART_O_LCRH) &= ~UART_LCRH_BRK;  		// No enviar break.
    														// Esta escritura sólo tiene la intención de
    														// cargar los valores del IBRD y FBRD
    														// porque el "no enviar break" ya está configurado
    														// (repetición)
    														// OJO: No quitar o los valores del IBRD y FBRD
    														// no se actualizarán

    HWREG(uart_base+UART_O_CTL) |= UART_CTL_UARTEN;			// ACTIVA UART

    /* 3- Habilitación de interrupciones: transmisión, recepción (no error, no)
         Registros a usar:  IM */

    HWREG(uart_base+UART_O_IM) |= UART_IM_RXIM;		   	// Receive interrupt enable
    HWREG(uart_base+UART_O_IM) |= UART_IM_TXIM;			// Transmit interrupt enable

    // Establecer prioridad de la interrupcion a configKERNEL_INTERRUPT_PRIORITY
    // Usando siempre NVIC_PRI1 porque INT_UARTx = 21 o 22 => IRQx = 5 o 6
	//
	nvic_prio_byte = 1 + (int)which_uart;		// 1 (UART0) o 2 (UART1)
	nvic_prio_shift = 8*nvic_prio_byte;		// 8 bits (UART0) o 16 bits (UART1)
	//
    ulTemp = HWREG(NVIC_PRI1);					// Leer PRI1 completo
    ulTemp &= ~(0xE0 << nvic_prio_shift);	// borrar campo prioridad de la UART correspondiente
    ulTemp |= configKERNEL_INTERRUPT_PRIORITY << nvic_prio_shift;	// Establecer prioridad de la UART correspondiente
    HWREG(NVIC_PRI1) = ulTemp;					// Escribir PRI1 completo

    // Rehabilitar interrupciones UART en el controlador de interrupciones???
    // ahora que la UART está lista y hemos borrado el flag TXIFG*/
    _uart_enable_interrupts_nvic (which_uart);
}

void debug_uart_isr (void){

  uint32_t which_flag;
  if (uart_base == 0){
    return;
  }
  debug_uart_isr_begin ();

  which_flag = HWREG(uart_base + UART_O_MIS);
  HWREG(uart_base + UART_O_ICR) = which_flag;
    if((which_flag&UART_MIS_TXMIS)==UART_MIS_TXMIS){
    	debug_uart_isr_tx();
    }
    if((which_flag&UART_MIS_RXMIS)==UART_MIS_RXMIS){
    	debug_uart_isr_rx();
    }
  debug_uart_isr_end ();
}
