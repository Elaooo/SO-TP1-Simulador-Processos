#ifndef TAD_GERENCIADORPROCESSO_H
#define TAD_GERENCIADORPROCESSO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <../include/fila.h>
#include <../include/tempo.h>
#include <../include/TAD_CPU.h>
#include <../include/TAD_TabelaProcessos.h>
//para o escalonador
#define QUANTUM_PRIORIDADE_0 1
#define QUANTUM_PRIORIDADE_1 2
#define QUANTUM_PRIORIDADE_2 4
#define QUANTUM_PRIORIDADE_3 8
#define NUM_PRIORIDADES 4

typedef struct{

    TFila estadoPronto[4];
    TFila estadoBloquado;
    TFila estadoEmExecucao;
    Tempo tempo;
    cpu_s cpu;
    TabelaDeProcessos tabelaProcessos;

}GerenciadorProcesso;

int escalonadorMLFQ(GerenciadorProcesso* gerenciador);
int leituraProcessoInit(TabelaDeProcessos *tabelaProcessos);
int inicializaGerenciadorProcessos(GerenciadorProcesso *gerenciadorProcessos);
void rodarGerenciador(int fd_leitura);




#endif
