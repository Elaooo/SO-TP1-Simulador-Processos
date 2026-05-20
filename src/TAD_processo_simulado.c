#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../include/TAD_processo_simulado.h"
#include "../include/globais.h"

// void salvaEstado(processo* proc, int pcAtual, int* variaveisAtuais, int quantumAtual, int nVar) {

//     proc->pcCounter = pcAtual;
//     proc->quantum=quantumAtual;

//     for (int i = 0; i < nVar; i++) {
//         proc->variaveis[i] = variaveisAtuais[i];
//     }
// }

// void inicializarProcesso(processo* proc, int id, int nVar, int nInstrucao, int* variaveisIniciais, enum estado estadoInicial, int prioridade, instrucao* listaInstrucoes){

//     proc->pid=proximoPidDisponivel;
//     proc->nVariaveis=nVar;
//     proc->pcCounter=0;
//     proc->estado=estadoInicial;
//     proc->quantum=0;
//     proc->quantum_usado_CPUatual=0;
//     proc->prioridade=prioridade;
//     proc->tempoBloqueado=0;
//     proc->nInstrucoes=nInstrucao;

//     proc->variaveis = (int*) malloc(sizeof(int) * nVar);
//     if (proc->variaveis != NULL) {
//         for (int i = 0; i < nVar; i++) {
//             proc->variaveis[i] = variaveisIniciais[i];
//         }
//     }

//     proc->listaInstrucoes = (instrucao*) malloc(sizeof(instrucao) * nInstrucao);
//     if (proc->listaInstrucoes != NULL) {
//         for (int i = 0; i < nInstrucao; i++) {
//             proc->listaInstrucoes[i] = listaInstrucoes[i];
//         }
//     }
// }

void inicializarProcessoInit(processo* proc, instrucao* listaInstrucoes, int nInstrucao){

    //definir depois valor que não represente nada
    proc->pid=proximoPidDisponivel;
    proximoPidDisponivel++;
    proc->nInstrucoes=nInstrucao;

    proc->listaInstrucoes=NULL;
    proc->variaveis=NULL;

    proc->pcCounter=0;
    proc->estado=PRONTO;
    proc->prioridade=0;

    proc->quantum=0;
    proc->quantum_usado_CPUatual=0;

    proc->tempoBloqueado=0;

    proc->listaInstrucoes = (instrucao*) malloc(sizeof(instrucao) * nInstrucao);
    
    if (proc->listaInstrucoes != NULL) {
        for (int i = 0; i < nInstrucao; i++) {
            proc->listaInstrucoes[i] = listaInstrucoes[i];
        }
    }

    if((proc->listaInstrucoes[0].tipo == 'N')){

        proc->nVariaveis= proc->listaInstrucoes[0].n;

        proc->variaveis = (int*) malloc(sizeof(int) * proc->nVariaveis);

        if(proc->variaveis!=NULL){
            for(int j=0;j < proc->nVariaveis; j++){
                proc->variaveis[j]=999;
            }
        }else{
            printf("erro");
        }

    } else{
        printf("Alocacao de memoria do Falhou\n ");
    }
}

void inicializarProcesso(processo* proc, instrucao* listaInstrucoes, int nInstrucao){

    proc->nInstrucoes=nInstrucao;
    proc->pcCounter=0;
    proc->estado=PRONTO;

    proc->quantum=15;
    proc->quantum_usado_CPUatual=0;

    proc->tempoBloqueado=0;


    if (proc->listaInstrucoes != NULL){
        free(proc->listaInstrucoes);
    }

    proc->listaInstrucoes = (instrucao*) malloc(sizeof(instrucao) * nInstrucao);
    
    if (proc->listaInstrucoes != NULL) {
        for (int i = 0; i < nInstrucao; i++) {
            proc->listaInstrucoes[i] = listaInstrucoes[i];
        }
    }

    if((proc->listaInstrucoes[0].tipo == 'N')){

        proc->variaveis = (int*) malloc(sizeof(int) * proc->listaInstrucoes[0].n);
    } else{
        printf("Alocacao de memoria do Falhou\n ");
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
                printf("Comando F | X: %d\n", lista[i].n);
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


const char* estadoParaString(enum estado e){

    switch(e){

        case PRONTO:
            return "PRONTO";

        case BLOQUEADO:
            return "BLOQUEADO";

        case EXECUCAO:
            return "EXECUCAO";

        case TERMINADO:
            return "TERMINADO";

        default:
            return "DESCONHECIDO";
    }
}

void imprimirProcesso(processo *p){

    if(p == NULL){
        printf("Processo inexistente.\n");
        return;
    }

    printf("\n========= PROCESSO =========\n");

    printf("PID: %d\n", p->pid);

    printf("Estado: %s\n",
           estadoParaString(p->estado));

    printf("PC Counter: %d\n",
           p->pcCounter);

    printf("Quantum: %d\n",
           p->quantum);

    printf("Quantum usado CPU atual: %d\n",
           p->quantum_usado_CPUatual);

    printf("Prioridade: %d\n",
           p->prioridade);

    printf("Tempo bloqueado: %d\n",
           p->tempoBloqueado);

    printf("Numero de variaveis: %d\n",
           p->nVariaveis);

    printf("\n----- Variaveis -----\n");

    if(p->variaveis != NULL){

        for(int i = 0; i < p->nVariaveis; i++){

            printf("Variavel[%d] = %d\n",
                   i,
                   p->variaveis[i]);
        }
    }
    else{
        printf("Nenhuma variavel alocada.\n");
    }

    printf("\n----- Instrucoes -----\n");

    printf("Numero de instrucoes: %d\n",
           p->nInstrucoes);

    if(p->listaInstrucoes != NULL){

        for(int i = 0; i < p->nInstrucoes; i++){

            instrucao inst = p->listaInstrucoes[i];

            printf("[%d] Tipo: %c",
                   i,
                   inst.tipo);

            if(inst.tipo == 'N'){
                printf(" n=%d", inst.n);
            }

            else if(inst.tipo == 'D'){
                printf(" x=%d", inst.x);
            }

            else if(inst.tipo == 'V' ||
                    inst.tipo == 'A' ||
                    inst.tipo == 'S'){

                printf(" x=%d n=%d",
                       inst.x,
                       inst.n);
            }

            else if(inst.tipo == 'R'){

                printf(" arquivo=%s",
                       inst.caminhoArquivo);
            }
            else if(inst.tipo == 'F'){
                printf(" %d", inst.n);
            }

            printf("\n");
        }
    }
    else{
        printf("Nenhuma instrucao carregada.\n");
    }

    printf("================================\n");
}

