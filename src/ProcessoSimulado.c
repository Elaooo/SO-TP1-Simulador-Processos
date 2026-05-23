#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../include/ProcessoSimulado.h"
#include "../include/Globais.h"


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

    if (proc->variaveis != NULL) {
        free(proc->variaveis);
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

}
