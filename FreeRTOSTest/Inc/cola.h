#ifndef cola_H
#define cola_H

#include <stdint.h> 


// Declaraciones p�blicas del m�dulo

struct s_cola;
typedef struct s_cola t_cola;
void cola_init(t_cola *p);
int cola_guardar (t_cola *p, unsigned char *msg);
int cola_leer (t_cola *p, unsigned char *msg, uint32_t l);

#endif 



