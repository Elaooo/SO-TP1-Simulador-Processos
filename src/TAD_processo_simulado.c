#include <stdio.h>
#include <string.h>
#include "../include/TAD_processo_simulado.h"

void salvaEstado(processo* proc, int pcAtual, int* variaveisAtuais, int quantum) {

    *(proc->pcCounter) = pcAtual;
    proc->quantum=quantum;
    
}

void inicializarProcesso(processo* proc, int id, int n, int* variaveisIniciais, char estado, int prioridade, instrucao* listaInstrucoes){

    proc->pid=id;
    for (int i=0;i<n;i++){
        proc->variaveis[i]=variaveisIniciais[i];
    }
    proc->pcCounter=0;

    proc->estado=estado;
    proc->quantum=0;
    proc->prioridade=prioridade;
    

}