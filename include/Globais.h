#ifndef GLOBAIS_H
#define GLOBAIS_H

#include <pthread.h>
#include "ProcessoControle.h"

extern int proximoPidDisponivel;

extern ComandoPipe msg_compartilhada;
extern int tem_novo_comando; // flag para indicar um comando n lido (0 1)

extern pthread_mutex_t mutex_comando;
extern pthread_cond_t cond_comando;

#endif