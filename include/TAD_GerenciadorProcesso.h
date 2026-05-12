#ifndef TAD_GERENCIADORPROCESSO_H
#define TAD_GERENCIADORPROCESSO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <../include/fila.h>
#include <../include/tempo.h>
#include <../include/TAD_CPU.h>
#include <../include/TAD_TabelaProcessos.h>

typedef struct{
    
    TFila estadoPronto;
    TFila estadoBloquado;
    TFila estadoEmExecucao;
    Tempo tempo;
    cpu_s cpu;
    TabelaDeProcessos tabelaProcessos;

}GerenciadorProcesso;

int leituraProcessoInit(TabelaDeProcessos *tabelaProcessos);
int inicializaGerenciadorProcessos(GerenciadorProcesso *gerenciadorProcessos);
void rodarGerenciador(int fd_leitura);




#endif
