#include "../include/TAD_GerenciadorProcesso.h"

#include <stdio.h>

void Imprime(GerenciadorProcesso * gerenciadorProcesso){
    
    int qtdProcessosProntos = 0, qtdProcessosBloqueados = 0, 
    qtdProcessosEmExecucao = 0, qtdProcessos = 0;

    processo * processoAtual = gerenciadorProcesso->cpu.processo_atual;

    /////////////////////////////////////////////////////////////////

    printf("----------PROCESSOS BLOQUEADOS----------");
    printf("quantidade de processos bloqueados: %d \n", qtdProcessosBloqueados);
    printf("Fila de processos bloqueados: \n\n");

    Celula * celulaBloquados = gerenciadorProcesso->estadoBloquado.pFrente->pProx;
    processo * processoBloqueado;
    printf("  ID  |  pcCounter  |  prioridade  |  quantum  |\n");
    for(int i = 0; i<qtdProcessosBloqueados; i++){
        processoBloqueado = buscarProcessoTabela(&gerenciadorProcesso->tabelaProcessos, celulaBloquados->Item.Chave);
        
        printf("  %d    %d    %d    %d\n", 
                processoBloqueado->pid, processoBloqueado->pcCounter, 
                processoBloqueado->prioridade, processoBloqueado->quantum);

        celulaBloquados = celulaBloquados->pProx;
    }
    free(celulaBloquados);
    printf("\n");

    /////////////////////////////////////////////////////////////////

    printf("-----------PROCESSOS PRONTOS-----------");
    printf("quantidade de processos prontos para executar: %d\n", qtdProcessosProntos);
    printf("Fila de processos prontos: \n\n");

    Celula * celulaProntos = gerenciadorProcesso->estadoPronto.pFrente->pProx;
    processo * processoPronto;
    printf("  ID  |  pcCounter  |  prioridade  |  quantum  |\n");
    for(int i = 0; i<qtdProcessosProntos; i++){
        processoPronto = buscarProcessoTabela(&gerenciadorProcesso->tabelaProcessos, celulaProntos->Item.Chave);
        
        printf("  %d    %d    %d    %d\n", 
                processoPronto->pid, processoPronto->pcCounter, 
                processoPronto->prioridade, processoPronto->quantum);

        celulaProntos = celulaProntos->pProx;
    }
    free(celulaProntos);
    printf("\n");

    /////////////////////////////////////////////////////////////////

    printf("----------PROCESSOS EM EXECUCAO----------\n");

    printf("quantidade de processos em execucao: %d\n", qtdProcessosEmExecucao);
    printf("Fila de processos em execucao: \n\n");

    Celula * celulaEmExecucao = gerenciadorProcesso->estadoEmExecucao.pFrente->pProx;
    processo * processoEmExecucao;
    printf("  ID  |  pcCounter  |  prioridade  |  quantum  |\n");
    for(int i = 0; i<qtdProcessosEmExecucao; i++){
        processoEmExecucao = buscarProcessoTabela(&gerenciadorProcesso->tabelaProcessos, celulaEmExecucao->Item.Chave);
        
        printf("  %d    %d    %d    %d\n", 
                processoEmExecucao->pid, processoEmExecucao->pcCounter, 
                processoEmExecucao->prioridade, processoEmExecucao->quantum);

        celulaEmExecucao = celulaEmExecucao->pProx;
    }
    free(celulaEmExecucao);
    printf("\n");


    printf("----------------------------------------\n");

}
