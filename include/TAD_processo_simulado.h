#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef TAD_PROCESSO_SIMULADO_H
#define TAD_PROCESSO_SIMULADO_H

enum estado {
  PRONTO = 1,
  BLOQUEADO = 0,
  EXECUCAO = 2
};

typedef struct{

    char tipo;
    int x;
    int n;
    char caminhoArquivo[256];

} instrucao;

typedef struct{
    
    int pid; //process id
    int *variaveis;
    int pcCounter;
    enum estado estado;
    int quantum;
    int prioridade;
    instrucao *listaInstrucoes;
 
}processo;

void inicializarProcesso(processo* proc, int id, int nVar, int nInstrucao, int* variaveisIniciais, enum estado estadoInicial, int prioridade, instrucao* listaInstrucoes);
void salvaEstado(processo* proc, int pcAtual, int* variaveisAtuais, int quantumAtual, int nVar);

#endif