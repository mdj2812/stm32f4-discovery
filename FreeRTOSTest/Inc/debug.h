#ifndef debug_H
#define debug_H

#include "cola.h"

// Declaraciones p�blicas del m�dulo led

extern t_cola colaDebug;
void DebugInit(void);
void DebugTask(void * argument);

#endif 

