#include <stdio.h>
#include <string.h>
#include "../include/TAD_processo_simulado.h"

void salvaEstado(processo* proc, int pcAtual, int* variaveisAtuais, int quantumAtual, int nVar) {

    proc->pcCounter = pcAtual;
    proc->quantum=quantumAtual;

    for (int i = 0; i < nVar; i++) {
        proc->variaveis[i] = variaveisAtuais[i];
    }
}

void inicializarProcesso(processo* proc, int id, int nVar, int nInstrucao, int* variaveisIniciais, enum estado estadoInicial, int prioridade, instrucao* listaInstrucoes){

    proc->pid=id;
    proc->pcCounter=0;
    proc->estado=estadoInicial;
    proc->quantum=0;
    proc->quantum_usado_CPUatual=0;
    proc->prioridade=prioridade;
    proc->listaInstrucoes=listaInstrucoes;

    proc->variaveis = (int*) malloc(sizeof(int) * nVar);
    if (proc->variaveis != NULL) {
        for (int i = 0; i < nVar; i++) {
            proc->variaveis[i] = variaveisIniciais[i];
        }
    }

    proc->listaInstrucoes = (instrucao*) malloc(sizeof(instrucao) * nInstrucao);
    if (proc->listaInstrucoes != NULL) {
        for (int i = 0; i < nInstrucao; i++) {
            proc->listaInstrucoes[i] = listaInstrucoes[i];
        }
    }
}