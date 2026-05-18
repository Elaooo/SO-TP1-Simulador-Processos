#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef TAD_PROCESSO_SIMULADO_H
#define TAD_PROCESSO_SIMULADO_H


enum estado {
  PRONTO = 1,
  BLOQUEADO = 0,
  EXECUCAO = 2,
  TERMINADO = 3
};

typedef struct{

    char tipo;
    int x;
    int n;
    char caminhoArquivo[256];

} instrucao;

typedef struct{
    
    int pid; //process id
    int nVariaveis;
    int *variaveis;
    int pcCounter;
    enum estado estado;
    int quantum;
    int quantum_usado_CPUatual;
    int prioridade;
    int nInstrucoes; 
    instrucao *listaInstrucoes;
    int tempoBloqueado;
 
}processo;

//void inicializarProcesso(processo* proc, int nVar, int nInstrucao, int* variaveisIniciais, enum estado estadoInicial, int prioridade, instrucao* listaInstrucoes);
//void salvaEstado(processo* proc, int pcAtual, int* variaveisAtuais, int quantumAtual, int nVar);
void inicializarProcesso(processo* proc, instrucao* listaInstrucoes, int nInstrucao);
void inicializarProcessoInit(processo* proc, instrucao* listaInstrucoes, int nInstrucao);
void imprimirInstrucoes(instrucao *lista, int tamanho);
void imprimirProcesso(processo *p);
const char* estadoParaString(enum estado e);

#endif