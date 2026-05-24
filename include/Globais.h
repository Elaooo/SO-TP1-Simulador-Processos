#ifndef GLOBAIS_H
#define GLOBAIS_H

#include <pthread.h>
#include "ProcessoControle.h"
#include "../include/FilaComandos.h"


// Flags do escalonador
#define FIFO 1
#define MLFQ 0

extern int proximoPidDisponivel;
extern FilaComandos filaComandos;
extern Comando msg_compartilhada;
extern int tem_novo_comando; // flag para indicar um comando n lido (0 1)

extern pthread_mutex_t mutex_comando;
extern pthread_cond_t cond_comando;

typedef struct {
    int argc;
    char *argv[];
}ArgsControle;

typedef struct {
    int qtdCPU;
    int tipoEscalonador;
}ArgsGerenciador;

#endif