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

    TFila estadoPronto[4];//No FIFO usamos somente o estadoPronto[0]
    TFila estadoBloquado;
    TFila estadoEmExecucao;
    TFila finalizados;
    Tempo tempo;
    cpu_s cpu;
    TabelaDeProcessos tabelaProcessos;
    int totalProcessosFinalizados;
    int somaTemposResposta;

}GerenciadorProcesso;
int escalonadorFIFO(GerenciadorProcesso* gerenciador);
int escalonadorMLFQ(GerenciadorProcesso* gerenciador);
int leituraProcessoInit(processo *processo);
int inicializaGerenciadorProcessos(GerenciadorProcesso *gerenciadorProcessos);
void rodarGerenciador(int fd_leitura, int escFlag);

processo* clonaProcesso(cpu_s *cpu);




#endif
