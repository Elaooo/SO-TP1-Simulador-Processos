#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef TAD_PROCESSO_SIMULADO_H
#define TAD_PROCESSO_SIMULADO_H


#define PRONTO 1
#define BLOQUEADO 0
#define EXECUCAO 2

typedef struct{

    char tipo;
    int x;
    int n;
    char caminhoArquivo[256];

} instrucao;

typedef struct{
    
    int pid; //process id
    int *variaveis; //dar malloc ao iniciar, com o valor de N n
    int pcCounter;
    char estado;
    int quantum;
    int prioridade;
    instrucao *listaInstrucoes;
 
}processo;

void inicializarProcesso(processo* proc, int id, int n, int* variaveisIniciais, char estado, int prioridade, instrucao* listaInstrucoes);

void salvaEstado(processo* proc, int pcAtual, int* variaveisAtuais, int quantum);


#endif