
/* Scheduler includes. */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "cola.h"
#include "debug.h"
#include "debug_UART.h"

static char *msg_error = "***ERROR*** cola_leer() = -1 ????? \r\n" ;

#define BUFFER_SIZE 1024
static unsigned char buffer[BUFFER_SIZE];
static SemaphoreHandle_t semaforo_debug_isruart;
static int buffer_indice;
static int buffer_indice_max;
static BaseType_t xHigherPriorityTaskWoken;

void debug_uart_isr_begin (void);
void debug_uart_isr_end (void);
void debug_uart_isr_tx(void);
void debug_uart_isr_rx(void);


void debug_uart_isr_begin (void){
   xHigherPriorityTaskWoken = pdFALSE;
}

void debug_uart_isr_end (void){
   portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/*para rellenar*/
void debug_uart_isr_tx(){
    buffer_indice ++;
    if (buffer_indice > buffer_indice_max){
      xSemaphoreGiveFromISR( semaforo_debug_isruart, &xHigherPriorityTaskWoken );
    }else{
      debug_uart_send (buffer);
    }
}

void debug_uart_isr_rx(){
	// No hacer nada
}
    
    
void DebugInit(void){
  BaseType_t xReturned;
  TaskHandle_t xHandle = NULL;
  //inicializar cola+mutex para almacenar mensajes
 cola_init(&colaDebug);
 //inicializar semaforo compartido entre tarea debuj y la isr de la UART
 semaforo_debug_isruart = xSemaphoreCreateBinary();
 //crear tarea de depuracion

 //inicializar la UART de depuracion
 //debug_uart_init(UART_2);
}
 
/* DebugTask function */
void DebugTask(void * argument)
{

  /* USER CODE BEGIN DebugTask */
  int i, res_leer;
  
  /* Infinite loop */
  cola_guardar(&colaDebug, "\r\n");
  //cola_guardar(&colaDebug, "Mensaje desde DebugTask ... inicializando\r\n");
  for(;;)
  {
    res_leer = cola_leer(&colaDebug, buffer, BUFFER_SIZE);
    while (res_leer == 0) {
    vTaskDelay( 5 );
    res_leer = cola_leer(&colaDebug, buffer, BUFFER_SIZE);
    }
    if (res_leer == -1){
      //copiar en buffer mensaje de error
      for(i=0; msg_error[i] != '\n'; i++){
          buffer[i] = msg_error[i];
      }
      res_leer = i;
    }
        
    //Activar transmisión de la UART para transmitir el mensaje 
    //almacenado en buffer que es de res_leer caracteres
    buffer_indice = 0; 
    buffer_indice_max = res_leer - 1;
    debug_uart_send (buffer);
    
     
     //Esperar a que la UART nos de permiso para continuar ..
     //Significa que el mensaje del buffer ya se ha enviado completamente
     xSemaphoreTake( semaforo_debug_isruart, portMAX_DELAY /* (TickType_t)10 */ );
  }
}
