
/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "cola.h"
#include "debug.h"
#include "debug_UART.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "string.h"

/* Defines -------------------------------------------------------------------*/
#define BUFFER_SIZE 1024

/* Private variables ---------------------------------------------------------*/
static char *msg_error = "***ERROR*** cola_leer() = -1 ????? \r\n" ;

static unsigned char buffer[BUFFER_SIZE];
unsigned char temp;
__IO uint16_t buffer_indice;
__IO uint16_t buffer_indice_max;

static TaskHandle_t xDebugTaskHandle;
static TaskHandle_t xGuardar1sTaskHandle;
static TaskHandle_t xGuardar2sTaskHandle;
static SemaphoreHandle_t semaforo_debug_isruart;
static SemaphoreHandle_t semaforo_debug_isrsystick_1s;
static SemaphoreHandle_t semaforo_debug_isrsystick_2s;
static BaseType_t xHigherPriorityTaskWoken;
static BaseType_t xGuardarSomeTaskWoken;

/* Private function prototypes -----------------------------------------------*/
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
  buffer_indice++;
  if(buffer_indice > buffer_indice_max)
  {
    xSemaphoreGiveFromISR( semaforo_debug_isruart, &xHigherPriorityTaskWoken );
  }
  else
  {
    temp = buffer[buffer_indice];
    debug_uart_send(&temp, 1);
  }
}

void debug_uart_isr_rx(){
	// No hacer nada
}

void debug_systick_isr_begin(void)
{
  xGuardarSomeTaskWoken = pdFALSE;
}

void debug_systick_isr_1s(void)
{
  xSemaphoreGiveFromISR( semaforo_debug_isrsystick_1s, &xGuardarSomeTaskWoken );
}

void debug_systick_isr_2s(void)
{
  xSemaphoreGiveFromISR( semaforo_debug_isrsystick_2s, &xGuardarSomeTaskWoken );
}

void debug_systick_isr_end(void)
{
  portYIELD_FROM_ISR( xGuardarSomeTaskWoken );
}

void DebugInit(void){  
  //inicializar cola+mutex para almacenar mensajes
 cola_init(&colaDebug);
 //inicializar semaforo compartido entre tarea debuj y la isr de la UART
 semaforo_debug_isruart = xSemaphoreCreateBinary();
 //crear tarea de depuracion
 xTaskCreate(
                DebugTask,       /* Function that implements the task. */
                "debug",          /* Text name for the task. */
                configMINIMAL_STACK_SIZE + 16, /* Stack size in words, not bytes. */
                ( void * ) 0,    /* Parameter passed into the task. */
                tskIDLE_PRIORITY,/* Priority at which the task is created. */
                &xDebugTaskHandle );      /* Used to pass out the created task's handle. */

  /* definition and creation of semaforo_debug_isrsystick */
  semaforo_debug_isrsystick_1s = xSemaphoreCreateBinary();
  semaforo_debug_isrsystick_2s = xSemaphoreCreateBinary();
  
  /* definition and creation of DebugGuardarTask */
  xTaskCreate(
                DebugGuardar1sTask,       /* Function that implements the task. */
                "Guardar_1s",          /* Text name for the task. */
                configMINIMAL_STACK_SIZE + 16, /* Stack size in words, not bytes. */
                ( void * ) 0,    /* Parameter passed into the task. */
                tskIDLE_PRIORITY+1,/* Priority at which the task is created. */
                &xGuardar1sTaskHandle );      /* Used to pass out the created task's handle. */
  
  xTaskCreate(
                DebugGuardar2sTask,       /* Function that implements the task. */
                "Guardar_2s",          /* Text name for the task. */
                configMINIMAL_STACK_SIZE + 16, /* Stack size in words, not bytes. */
                ( void * ) 0,    /* Parameter passed into the task. */
                tskIDLE_PRIORITY+2,/* Priority at which the task is created. */
                &xGuardar2sTaskHandle );      /* Used to pass out the created task's handle. */
  
 //inicializar la UART de depuracion
 debug_uart_init();
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
    buffer_indice=0;
    buffer_indice_max = res_leer-1;
    temp = buffer[buffer_indice];
    debug_uart_send(&temp, 1);
      
    //Esperar a que la UART nos de permiso para continuar ..
    //Significa que el mensaje del buffer ya se ha enviado completamente
    xSemaphoreTake( semaforo_debug_isruart, portMAX_DELAY /* (TickType_t)10 */ );
  }
}

/* DebugGuardar1sTask function */
__IO uint16_t counter_1s = 1;
void DebugGuardar1sTask(void * argument)
{
  unsigned char buffer_1s[20], Task_name[20];
  
  strcpy((char*) Task_name, pcTaskGetName(xGuardar1sTaskHandle));
  for(;;)
  {
    if(xSemaphoreTake(semaforo_debug_isrsystick_1s, portMAX_DELAY))
    {
      sprintf((char*) buffer_1s, "[%d\t]%s\t: %d\r\n", xTaskGetTickCount(), Task_name, counter_1s++);
      cola_guardar(&colaDebug, buffer_1s);
    }
  }
}

/* DebugGuardar2sTask function */
__IO uint16_t counter_2s = 1001;
void DebugGuardar2sTask(void * argument)
{
  unsigned char buffer_2s[20], Task_name[20];
  
  strcpy((char*) Task_name, pcTaskGetName(xGuardar2sTaskHandle));
  for(;;)
  {
    if(xSemaphoreTake(semaforo_debug_isrsystick_2s, portMAX_DELAY))
    {
      sprintf((char*) buffer_2s, "[%d\t]%s\t: %d\r\n", xTaskGetTickCount(), Task_name, counter_2s++);
      cola_guardar(&colaDebug, buffer_2s);
    }
  }
}
