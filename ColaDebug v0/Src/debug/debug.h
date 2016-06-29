#ifndef debug_H
#define debug_H

#include "cola.h"

// Declaraciones públicas del módulo led

extern t_cola colaDebug;
void DebugInit(void);
void DebugTask(void * argument);

#endif 

