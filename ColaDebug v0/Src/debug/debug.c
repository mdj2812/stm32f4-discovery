
/* Scheduler includes. */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "cola.h"
#include "debug.h"
#include "debug_UART.h"
#include "stm32f4xx_hal.h"

static char *msg_error = "***ERROR*** cola_leer() = -1 ????? \r\n" ;

#define BUFFER_SIZE 1024
static unsigned char buffer[BUFFER_SIZE];
static SemaphoreHandle_t semaforo_debug_isruart;
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
  xSemaphoreGiveFromISR( semaforo_debug_isruart, &xHigherPriorityTaskWoken );
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
  xReturned = xTaskCreate(
                    DebugTask,       /* Function that implements the task. */
                    "debug",          /* Text name for the task. */
                    configMINIMAL_STACK_SIZE + 16, /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,/* Priority at which the task is created. */
                    &xHandle );      /* Used to pass out the created task's handle. */

 //inicializar la UART de depuracion
 debug_uart_init();
}
 
/* DebugTask function */
void DebugTask(void * argument)
{

  /* USER CODE BEGIN DebugTask */
  int i, res_leer;
  
  /* Infinite loop */
  cola_guardar(&colaDebug, "Hello World!\r\n");
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
    debug_uart_send(buffer, res_leer);
     
    //Esperar a que la UART nos de permiso para continuar ..
    //Significa que el mensaje del buffer ya se ha enviado completamente
    if(xSemaphoreTake( semaforo_debug_isruart, portMAX_DELAY /* (TickType_t)10 */ ))
    {
      puts("New cycle.");
    }
  }
}
