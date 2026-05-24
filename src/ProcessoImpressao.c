#include "../include/ProcessoImpressao.h"

#include <stdio.h>
#include <unistd.h>

#define AZUL "\033[34m"
#define AMARELO "\033[33m"
#define VERMELHO "\033[31m"
#define BRANCO "\033[37m"
#define RESET "\033[0m"

void Imprime(GerenciadorProcesso *gerenciadorProcesso, int opcao, int escFlag)
{

    Informacoes info;
    ColetaInformacoes(&info, gerenciadorProcesso, escFlag);

    switch (opcao)
    {
    case 1:
        ImprimeTodosOsProcessos(&info);
        break;
    case 2:
        ImprimeProcessosEmExecucao(&info);
        break;
    case 3:
        ImprimeProcessosProntos(&info);
        break;
    case 4:
        ImprimeProcessosBloqueados(&info);
        break;
    case 5:
        ImprimeTodasAsCPUs(&info);
        break;
    case 6:
        ImprimeCPU(&info, 1);
        break;
    case 7:
        ImprimeCPU(&info, 2);
        break;
    case 8:
        ImprimeCPU(&info, 3);
        break;
    case 9:
        ImprimeCPU(&info, 4);
        break;
    default:
        break;
    }

    printf(AZUL "----------------------------------------\n" RESET);
    printf("Fim da Impressao\n");
    fflush(stdout);
    return;
}

void ColetaInformacoes(Informacoes *info, GerenciadorProcesso *gerenciadorProcesso, int escFlag)
{

    info->gerenciadorProcesso = gerenciadorProcesso;
    info->qtdProcessosBloqueados = gerenciadorProcesso->estadoBloquado.tam;
    info->qtdProcessosEmExecucao = gerenciadorProcesso->estadoEmExecucao.tam;

    info->qtdProcessosProntos = info->gerenciadorProcesso->estadoPronto[0].tam + info->gerenciadorProcesso->estadoPronto[1].tam +
                                info->gerenciadorProcesso->estadoPronto[2].tam + info->gerenciadorProcesso->estadoPronto[3].tam;

    for(int i = 0; i<4; i++){
        info->qtdProcessosProntosPrioridade[i] = info->gerenciadorProcesso->estadoPronto[i].tam;
    }

    info->qtdProcessos = info->qtdProcessosBloqueados + info->qtdProcessosEmExecucao + info->qtdProcessosProntos;

    info->escFlag = escFlag;
    info->nCPUs = info->gerenciadorProcesso->nCPUs;
}

void ImprimeTodosOsProcessos(Informacoes *info)
{

    printf(AZUL "----------PROCESSOS BLOQUEADOS----------\n" RESET);
    printf("%squantidade de processos bloqueados:  %s%d %s\n", AZUL, BRANCO, info->qtdProcessosBloqueados, RESET);
    printf(AZUL "Fila de processos bloqueados: \n\n" RESET);

    Celula *celulaBloquados = info->gerenciadorProcesso->estadoBloquado.pFrente->pProx;
    processo *processoBloqueado;
    printf(AZUL "  ID  |" BRANCO "  pcCounter  " AZUL "|  prioridade  |" BRANCO "  quantum  " AZUL "|\n" RESET);
    for (int i = 0; i < info->qtdProcessosBloqueados; i++)
    {
        processoBloqueado = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaBloquados->Item.Chave);

        printf("%s  %d     %s     %d       %s      %d      %s       %d\n%s", AZUL,
               processoBloqueado->pid, BRANCO, processoBloqueado->pcCounter, AZUL,
               processoBloqueado->prioridade, BRANCO, processoBloqueado->quantum, RESET);

        celulaBloquados = celulaBloquados->pProx;
    }

    printf("\n");

    /////////////////////////////////////////////////////////////////

    printf(AZUL "-----------PROCESSOS PRONTOS-----------\n" RESET);
    printf("%squantidade de processos prontos para executar: %s%d\n%s", AZUL, BRANCO, info->qtdProcessosProntos, RESET);
    printf(AZUL "Filas de processos prontos: \n\n" RESET);

    if (info->escFlag == MLFQ)
    {
        for (int k = 0; k < 4; k++)
        {

            printf("%sFila Prontos [%s%d%s]\n%s", AZUL, BRANCO, k, AZUL, RESET);

            TFila *filaProntos = &info->gerenciadorProcesso->estadoPronto[k];
            Celula *celulaProntos = filaProntos->pFrente->pProx;

            processo *processoPronto;
            printf(AZUL "  ID  |" BRANCO "  pcCounter  " AZUL "|  prioridade  |" BRANCO "  quantum  " AZUL "|\n" RESET);
            for (int i = 0; i < info->qtdProcessosProntosPrioridade[k]; i++)
            {
                processoPronto = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaProntos->Item.Chave);

                printf("%s  %d     %s     %d       %s      %d      %s       %d\n%s", AZUL,
                       processoPronto->pid, BRANCO, processoPronto->pcCounter, AZUL,
                       processoPronto->prioridade, BRANCO, processoPronto->quantum, RESET);

                celulaProntos = celulaProntos->pProx;
            }

            printf("\n");
        }
    }
    else
    {

        TFila *filaProntos = &info->gerenciadorProcesso->estadoPronto[0];
        Celula *celulaProntos = filaProntos->pFrente->pProx;

        processo *processoPronto;
        printf(AZUL "  ID  |" BRANCO "  pcCounter  " AZUL "|  prioridade  |" BRANCO "  quantum  " AZUL "|\n" RESET);
        for (int i = 0; i < info->qtdProcessosProntos; i++)
        {
            processoPronto = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaProntos->Item.Chave);

            printf("%s  %d     %s     %d       %s      %d      %s       %d\n%s", AZUL,
                   processoPronto->pid, BRANCO, processoPronto->pcCounter, AZUL,
                   processoPronto->prioridade, BRANCO, processoPronto->quantum, RESET);

            celulaProntos = celulaProntos->pProx;
        }

        printf("\n");
    }
    /////////////////////////////////////////////////////////////////

    printf(AZUL "----------PROCESSOS EM EXECUCAO----------\n" RESET);

    printf("%squantidade de processos em execucao: %s%d\n%s", AZUL, BRANCO, info->qtdProcessosEmExecucao, RESET);
    printf(AZUL "Fila de processos em execucao: \n\n" RESET);


    processo *processoEmExecucao;
    printf(BRANCO "  CPU  " AZUL "|  ID  |" BRANCO "  pcCounter  " AZUL "|  prioridade  |" BRANCO "  quantum  " AZUL "|\n" RESET);
    for (int i = 0; i < info->nCPUs; i++)
    {
        if(info->gerenciadorProcesso->cpu[i].emUso){
        processoEmExecucao = info->gerenciadorProcesso->cpu[i].processo_atual;
            printf("%s   %d   %s  %d     %s     %d       %s      %d      %s       %d\n%s", BRANCO, i+1 ,AZUL,
                   processoEmExecucao->pid, BRANCO, processoEmExecucao->pcCounter, AZUL,
                   processoEmExecucao->prioridade, BRANCO, processoEmExecucao->quantum, RESET);
        }

    }

    printf("\n");
}

void ImprimeProcessosEmExecucao(Informacoes *info)
{

    printf(AZUL "----------PROCESSOS EM EXECUCAO----------\n" RESET);

    printf("%squantidade de processos em execucao: %s%d\n%s", AZUL, BRANCO, info->qtdProcessosEmExecucao, RESET);
    printf(AZUL "Fila de processos em execucao: \n\n" RESET);


    processo *processoEmExecucao;
    printf(BRANCO "  CPU  " AZUL "|  ID  |" BRANCO "  pcCounter  " AZUL "|  prioridade  |" BRANCO "  quantum  " AZUL "|\n" RESET);
    for (int i = 0; i < info->nCPUs; i++)
    {
        if(info->gerenciadorProcesso->cpu[i].emUso){
        processoEmExecucao = info->gerenciadorProcesso->cpu[i].processo_atual;
            printf("%s   %d   %s  %d     %s     %d       %s      %d      %s       %d\n%s", BRANCO, i+1 ,AZUL,
                   processoEmExecucao->pid, BRANCO, processoEmExecucao->pcCounter, AZUL,
                   processoEmExecucao->prioridade, BRANCO, processoEmExecucao->quantum, RESET);
        }

    }

    printf("\n");
}

void ImprimeProcessosProntos(Informacoes *info)
{

    printf(AZUL "-----------PROCESSOS PRONTOS-----------\n" RESET);
    printf("%squantidade de processos prontos para executar: %s%d\n%s", AZUL, BRANCO, info->qtdProcessosProntos, RESET);
    printf(AZUL "Filas de processos prontos: \n\n" RESET);

    if (info->escFlag == MLFQ)
    {
        for (int k = 0; k < 4; k++)
        {

            printf("%sFila Prontos [%s%d%s]\n%s", AZUL, BRANCO, k, AZUL, RESET);

            TFila *filaProntos = &info->gerenciadorProcesso->estadoPronto[k];
            Celula *celulaProntos = filaProntos->pFrente->pProx;

            processo *processoPronto;
            printf(AZUL "  ID  |" BRANCO "  pcCounter  " AZUL "|  prioridade  |" BRANCO "  quantum  " AZUL "|\n" RESET);

            for (int i = 0; i < info->qtdProcessosProntosPrioridade[k]; i++)
            {
                processoPronto = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaProntos->Item.Chave);
                printf("%s  %d     %s     %d       %s      %d      %s       %d\n%s", AZUL,
                       processoPronto->pid, BRANCO, processoPronto->pcCounter, AZUL,
                       processoPronto->prioridade, BRANCO, processoPronto->quantum, RESET);

                celulaProntos = celulaProntos->pProx;
            }

            printf("\n");
        }
    }
    else
    {

        TFila *filaProntos = &info->gerenciadorProcesso->estadoPronto[0];
        Celula *celulaProntos = filaProntos->pFrente->pProx;

        processo *processoPronto;
        printf(AZUL "  ID  |" BRANCO "  pcCounter  " AZUL "|  prioridade  |" BRANCO "  quantum  " AZUL "|\n" RESET);
        for (int i = 0; i < info->qtdProcessosProntos; i++)
        {
            processoPronto = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaProntos->Item.Chave);

            printf("%s  %d     %s     %d       %s      %d      %s       %d\n%s", AZUL,
                   processoPronto->pid, BRANCO, processoPronto->pcCounter, AZUL,
                   processoPronto->prioridade, BRANCO, processoPronto->quantum, RESET);

            celulaProntos = celulaProntos->pProx;
        }

        printf("\n");
    }
    printf("\n");
}

void ImprimeProcessosBloqueados(Informacoes *info)
{

    printf(AZUL "----------PROCESSOS BLOQUEADOS----------\n" RESET);
    printf("%squantidade de processos bloqueados:  %s%d %s\n", AZUL, BRANCO, info->qtdProcessosBloqueados, RESET);
    printf(AZUL "Fila de processos bloqueados: \n\n" RESET);

    Celula *celulaBloquados = info->gerenciadorProcesso->estadoBloquado.pFrente->pProx;
    processo *processoBloqueado;
    printf(AZUL "  ID  |" BRANCO "  pcCounter  " AZUL "|  prioridade  |" BRANCO "  quantum  " AZUL "|\n" RESET);
    for (int i = 0; i < info->qtdProcessosBloqueados; i++)
    {
        processoBloqueado = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaBloquados->Item.Chave);

        printf("%s  %d     %s     %d       %s      %d      %s       %d\n%s", AZUL,
               processoBloqueado->pid, BRANCO, processoBloqueado->pcCounter, AZUL,
               processoBloqueado->prioridade, BRANCO, processoBloqueado->quantum, RESET);

        celulaBloquados = celulaBloquados->pProx;
    }

    printf("\n");
}

void ImprimeTodasAsCPUs(Informacoes *info)
{

    printf(AZUL "Informacoes do Gerenciador de Processos:\n" RESET);
    printf("%sTotal de processo finalizados:  %s%d\n%s", AZUL, BRANCO, info->gerenciadorProcesso->totalProcessosFinalizados, RESET);
    printf("%sTempo:  %s%d\n%s", AZUL, BRANCO, info->gerenciadorProcesso->tempo.valor, RESET);

    printf("\n");
    printf(AZUL "Informacoes das CPUs:\n" RESET);

    for(int i = 0; i< info->nCPUs; i++){

        processo * processoAtual = info->gerenciadorProcesso->cpu[i].processo_atual;
        if (processoAtual != NULL){
            printf("%sCPU numero: %s%d%s\n", AZUL,BRANCO,i+1,RESET);
        printf(AZUL "----------------------------------------\n" RESET);
        printf("%sIndice da proxima instrucao (PC):  %s%d\n%s", AZUL, BRANCO, info->gerenciadorProcesso->cpu[i].registradorPC, RESET);
        printf("%sProcesso atual:  %s%d\n%s", AZUL, BRANCO, processoAtual->pid, RESET);
        printf("%sQuantum alocado para o processo atual:  %s%d\n%s", AZUL, BRANCO, info->gerenciadorProcesso->cpu[i].quantum_total, RESET);
        printf(AZUL "----------------------------------------\n" RESET);
        }else{
            printf("%sCPU %s%d%s ociosa!%s\n", AZUL,BRANCO,i+1,AZUL,RESET);
        }



    }

    printf("\n");
    printf("\n");

}

void ImprimeCPU(Informacoes * info, int cpu){

    processo * processoAtual = info->gerenciadorProcesso->cpu[cpu-1].processo_atual;
    if (processoAtual != NULL){
        printf("%sCPU numero: %s%d%s\n", AZUL,BRANCO,cpu,RESET);
        printf(AZUL "----------------------------------------\n" RESET);
        printf("%sIndice da proxima instrucao (PC):  %s%d\n%s", AZUL, BRANCO, info->gerenciadorProcesso->cpu[cpu-1].registradorPC, RESET);
        printf("%sProcesso atual:  %s%d\n%s", AZUL, BRANCO, processoAtual->pid, RESET);
        printf("%sQuantum alocado para o processo atual:  %s%d\n%s", AZUL, BRANCO, info->gerenciadorProcesso->cpu[cpu-1].quantum_total, RESET);
        printf(AZUL "----------------------------------------\n" RESET);
        printf("\n");
    }else{
        printf("%sCPU %s%d%s ociosa!%s\n", AZUL,BRANCO,cpu,AZUL,RESET);
    }
}