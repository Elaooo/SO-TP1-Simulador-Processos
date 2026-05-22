#include "../include/FilaComandos.h"

void inicializaFilaComandos(FilaComandos *fila)
{
    fila->inicio = 0;
    fila->fim = 0;
    fila->tamanho = 0;

    pthread_mutex_init(&fila->mutex, NULL);
    pthread_cond_init(&fila->cond, NULL);
}

void enfileiraComando(FilaComandos *fila, Comando comando)
{
    pthread_mutex_lock(&fila->mutex);

    fila->fila[fila->fim] = comando;

    fila->fim = (fila->fim + 1) % MAX_COMANDOS;

    fila->tamanho++;

    pthread_cond_signal(&fila->cond);

    pthread_mutex_unlock(&fila->mutex);
}

Comando desenfileiraComando(FilaComandos *fila)
{
    pthread_mutex_lock(&fila->mutex);

    while (fila->tamanho == 0)
    {
        pthread_cond_wait(&fila->cond, &fila->mutex);
    }

    Comando comando = fila->fila[fila->inicio];

    fila->inicio = (fila->inicio + 1) % MAX_COMANDOS;

    fila->tamanho--;

    pthread_mutex_unlock(&fila->mutex);

    return comando;
}