#ifndef GERENCIADORPROCESSO_H
#define GERENCIADORPROCESSO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/Fila.h"
#include "../include/Tempo.h"
#include "../include/Cpu.h"
#include "../include/TabelaProcessos.h"
#include "../include/ProcessoControle.h"
//para o escalonador

//Flag pioraidade
#define FIFO 1
#define MLFQ 0

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

int inicializaGerenciadorProcessos(GerenciadorProcesso *gerenciadorProcessos);
void rodarGerenciador(int fd_leitura, int escFlag);
void atualizarProcessosBloqueados(GerenciadorProcesso *gerenciador, int escFlag);

processo* clonaProcesso(cpu_s *cpu);




#endif
