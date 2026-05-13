#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/TAD_processo_simulado.h"
#include "../include/TAD_LeituraArquivo.h"
#include "../include/TAD_TabelaProcessos.h"
#include "../include/TAD_CPU.h"
#include "../include/TAD_GerenciadorProcesso.h"


int main(){

    processo proc;
    cpu_s cpu;
    inicializarCPU(&cpu);
    leituraProcessoInit(&proc);
    printf("%d", proc.pid);
    //imprimirInstrucoes(proc.listaInstrucoes,proc.nInstrucoes);

    AtualizarRegistradorCPU(&cpu,&proc,20);
    
    imprimirCPU(&cpu);
    
    for(int i = 0; i<20 ; i++){
        executaInstrucoes(&cpu);
        imprimirCPU(&cpu);
    }

    SalvarContextoCPU(&cpu);

    imprimirProcesso(&proc);
   

    return 0;
}