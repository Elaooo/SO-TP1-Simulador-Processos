#ifndef TAD_FILACOMANDOS_H
#define TAD_FILACOMANDOS_H

#include <pthread.h>

#define MAX_COMANDOS 100

typedef struct {
    char tipo;
    int opcaoImpressao;
}Comando;

typedef struct {
    Comando fila[MAX_COMANDOS];

    int inicio;
    int fim;
    int tamanho;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
}FilaComandos;

void inicializaFilaComandos(FilaComandos *fila);

void enfileiraComando(FilaComandos *fila, Comando comando);

Comando desenfileiraComando(FilaComandos *fila);

#endif