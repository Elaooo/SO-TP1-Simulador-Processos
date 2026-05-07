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

void inicializarProcessoInit(processo* proc, int id, instrucao* listaInstrucoes, int nInstrucao){

    //definir depois valor que não represente nada
    proc->pid=id;
    proc->listaInstrucoes=listaInstrucoes;
    
    proc->listaInstrucoes = (instrucao*) malloc(sizeof(instrucao) * nInstrucao);
    if (proc->listaInstrucoes != NULL) {
        for (int i = 0; i < nInstrucao; i++) {
            proc->listaInstrucoes[i] = listaInstrucoes[i];
        }
    }
}


//debug
void imprimirInstrucoes(instrucao *lista, int tamanho) {

    if(tamanho == 0){
        printf("Instrucoes vazias\n");
        return;
    }
    for (int i = 0; i < tamanho; i++) {

        printf("Instrucao %d:\n", i);
        switch (lista[i].tipo) {
            case 'N':
                printf("Comando N | N: %d\n", lista[i].n);
                break;
            case 'D':
                printf("Comando D | X: %d\n", lista[i].x);
                break;
            case 'V':
                printf("Comando V | X: %d | N: %d\n", lista[i].x, lista[i].n);
                break;
            case 'A':
            case 'S':
                printf("Comando %c | X: %d | N: %d\n",
                       lista[i].tipo,
                       lista[i].x,
                       lista[i].n);
                break;
            case 'R':
                printf("Comando R | Arquivo: %s\n",
                       lista[i].caminhoArquivo);
                break;
            case 'F':
                printf("Comando F | X: %d\n", lista[i].x);
                break;
            case 'T':
                printf("Comando T\n");
                break;
            default:
                printf("Comando desconhecido: %c\n", lista[i].tipo);
                break;
        }
        printf("\n");
    }
}