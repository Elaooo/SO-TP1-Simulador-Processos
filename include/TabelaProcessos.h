#ifndef TAD_TABELAPROCESSOS_H
#define TAD_TABELAPROCESSOS_H

#include "ProcessoSimulado.h"

#define MAX_PROCESSOS 100 

typedef struct {
    processo* processos[MAX_PROCESSOS]; 
    int quantidade_atual;              
} TabelaDeProcessos;

void inicializarTabelaProcessos(TabelaDeProcessos* tabela);
int inserirProcessoTabela(TabelaDeProcessos* tabela, processo* novo_processo);
int removerProcessoTabela(TabelaDeProcessos* tabela, int pid_alvo);
processo* buscarProcessoTabela(TabelaDeProcessos* tabela, int pid_alvo);

#endif