#ifndef debug_H
#define debug_H

#include "cola.h"

// Declaraciones p�blicas del m�dulo led

extern t_cola colaDebug;
void debug_systick_isr_1s(void);
void debug_systick_isr_2s(void);
void DebugInit(void);
void DebugTask(void * argument);
void DebugGuardar1sTask(void * argument);
void DebugGuardar2sTask(void * argument);


#endif 

