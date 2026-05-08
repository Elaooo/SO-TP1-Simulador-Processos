#include <stdio.h>
#include <stdlib.h>
#include "../include/TAD_TabelaProcessos.h"

void inicializarTabelaProcessos(TabelaDeProcessos* tabela) {
    tabela->quantidade_atual = 0;
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        tabela->processos[i] = NULL;
    }
}

int inserirProcessoTabela(TabelaDeProcessos* tabela, processo* novo_processo) {
    if (tabela->quantidade_atual >= MAX_PROCESSOS) {
        printf("Erro: Tabela de processos cheia.\n");
        return 0; 
    }
    
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (tabela->processos[i] == NULL) {
            tabela->processos[i] = novo_processo;
            tabela->quantidade_atual++;
            return 1; 
        }
    }
    return 0;
}

int removerProcessoTabela(TabelaDeProcessos* tabela, int pid_alvo) {
    if (tabela->quantidade_atual == 0) return 0;

    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (tabela->processos[i] != NULL && tabela->processos[i]->pid == pid_alvo) {
            tabela->processos[i] = NULL; 
            tabela->quantidade_atual--;
            return 1;
        }
    }
    return 0; 
}

processo* buscarProcessoTabela(TabelaDeProcessos* tabela, int pid_alvo) {
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (tabela->processos[i] != NULL && tabela->processos[i]->pid == pid_alvo) {
            return tabela->processos[i];
        }
    }
    return NULL; 
}