
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "semphr.h"

/* Demo application includes. */
#include "cola.h"
#include "stdio.h"

#define COLA_SIZE 4*1024

struct s_cola{
  
  /*huecos vacios de tamaño COLA_SIZE, indice de lectura, indice de escritura*/
  uint32_t huecos, cabeza, cola, items;
  
  SemaphoreHandle_t mutex;
  
  uint8_t datos[COLA_SIZE];
};

t_cola colaDebug;

/*p = tomara el valor del dato usado por la funcion que llama*/  
void cola_init(t_cola *p){
  
  p->cabeza = 0;
  p->cola = 0; 
  p->items = 0;
  p->huecos = COLA_SIZE;
  p->mutex = xSemaphoreCreateMutex();
  
}

int cola_guardar (t_cola *p, unsigned char *msg){
  
 int i,l,res = 1;
 
  /*Calcular la longitud del mensaje en l*/
 for (l = 0; msg[l] != '\n'; l++) {
 }
 l++;
 xSemaphoreTake( p->mutex, portMAX_DELAY /* (TickType_t)10 */ );
 
 if(l < p->huecos){
   
     for (i = 0; i<l; i++) {
        /*guardar msg[i] en la cola p*/
 
       p->datos [ p->cola ] = msg[i];
       p->cola++; 
       if  (p->cola >= COLA_SIZE){
         p->cola = 0;
       }
       p->huecos--;
       p->items++;
     }
 } else {
      res = 0;
 }
 
 xSemaphoreGive( p->mutex );
 
 
 return res;
}

/*l es el tamaño del array msg, 
para saber si el mensaje que se lee cabe o no cabe en msg.
El valor de retorno sera: 
-1 si el mensaje de la cola p no cabe en msg.
0 si no hay mensaje.
1,2 o mas si cabe; l_de_p será el valor de la longitud del mensaje*/
int cola_leer (t_cola *p, unsigned char *msg, uint32_t l){
  
   int i, res, l_de_p, Ndatos;
 
 xSemaphoreTake( p->mutex, portMAX_DELAY /* (TickType_t)10 */ );
 
 Ndatos = COLA_SIZE - p->huecos;
   
  /*Calcular la longitud del mensaje que se va a sacar de la cola p en l*/
 for (l_de_p = 0, i = p->cabeza; l_de_p <= Ndatos; l_de_p++) {
       
    if ( p->datos[i] == '\n'){
        break;
    }   
    i++; 
    if ( i == COLA_SIZE){ 
       i = 0;
    }
 }
 if ((Ndatos == 0 )||( p->datos[i] != '\n')){
    res = 0;
 } else {
   l_de_p++;   // Contabilizar \n
   if (l_de_p > l){
     res = -1;
   } else { res = l_de_p;
           for (i = 0; i<l_de_p; i++) {
        /*copiar msg[i] de la cola p al mensaje*/
             msg[i] = p->datos [ p->cabeza ];
             p->cabeza++; 
             if (p->cabeza >= COLA_SIZE){
                p->cabeza = 0;
             }
             p->huecos++;
           }
     }
 }
  
 xSemaphoreGive( p->mutex );
 
 return res;
}
 
       
  
