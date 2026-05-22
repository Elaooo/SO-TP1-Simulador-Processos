#include "../include/Globais.h"

int proximoPidDisponivel = 0;

Comando msg_compartilhada;
int tem_novo_comando = 0;

pthread_mutex_t mutex_comando = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_comando = PTHREAD_COND_INITIALIZER;
