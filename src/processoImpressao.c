#include "../include/processoImpressao.h"

#include <stdio.h>

#define AZUL        "\033[34m"
#define AMARELO     "\033[33m"
#define VERMELHO    "\033[31m"
#define BRANCO      "\033[37m"
#define RESET       "\033[0m"


void Imprime(GerenciadorProcesso *gerenciadorProcesso)
{

    Informacoes info;
    ColetaInformacoes(&info, gerenciadorProcesso);

    printf(AZUL"----------------------------------------------------------\n"RESET);
    printf(AZUL"Bem vindo ao menu de impressao do seu sistema operacional!\n"RESET);
    printf(AZUL"----------------------------------------------------------\n"RESET);

    int menuExecutando = 1, opcao = 0;
    while (menuExecutando)
    {
        printf(AZUL"O que deseja visualizar?\n"RESET);
        printf(AZUL"1-"BRANCO" Todos os processos\n"RESET);
        printf(AZUL"2-"BRANCO" Processos em execucao\n"RESET);
        printf(AZUL"3-"BRANCO" Processos prontos para executar\n"RESET);
        printf(AZUL"4-"BRANCO" Processos bloqueados\n"RESET);
        printf(AZUL"5-"BRANCO" Informacoes gerais\n"RESET);
        printf(AZUL"0-"BRANCO" Sair do menu de impressao\n"RESET);
        scanf("%d", &opcao);

        switch (opcao)
        {
        case 0:
            printf(VERMELHO"Fechando o menu de impressao...\n"RESET);
            menuExecutando = 0;
            break;
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
            ImprimeInformacoesGerais(&info);
            break;
        default:
            printf(VERMELHO"Opcao invalida! Tente novamente\n"RESET);
            break;
        }
    }
    printf(AZUL"----------------------------------------\n"RESET);
}

void ColetaInformacoes(Informacoes * info , GerenciadorProcesso * gerenciadorProcesso){

    info->gerenciadorProcesso = gerenciadorProcesso;
    info->qtdProcessosBloqueados = gerenciadorProcesso->estadoBloquado.tam;
    info->qtdProcessosEmExecucao = gerenciadorProcesso->estadoEmExecucao.tam;

    info->qtdProcessos = info->gerenciadorProcesso->estadoPronto[0].tam + info->gerenciadorProcesso->estadoPronto[1].tam +
                          info->gerenciadorProcesso->estadoPronto[2].tam + info->gerenciadorProcesso->estadoPronto[3].tam;

    info->qtdProcessos = info->qtdProcessosBloqueados + info->qtdProcessosEmExecucao + info->qtdProcessosProntos;

    info->processoAtual = info->gerenciadorProcesso->cpu.processo_atual;

}

void ImprimeTodosOsProcessos(Informacoes * info){

    printf(AZUL"----------PROCESSOS BLOQUEADOS----------"RESET);
    printf(AZUL"quantidade de processos bloqueados: "BRANCO"%d \n"RESET, info->qtdProcessosBloqueados);
    printf(AZUL"Fila de processos bloqueados: \n\n"RESET);

    Celula *celulaBloquados = info->gerenciadorProcesso->estadoBloquado.pFrente->pProx;
    processo *processoBloqueado;
    printf(AZUL"  ID  |"BRANCO"  pcCounter  "AZUL"|  prioridade  |"BRANCO"  quantum  "AZUL"|\n"RESET);
    for (int i = 0; i < info->qtdProcessosBloqueados; i++)
    {
        processoBloqueado = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaBloquados->Item.Chave);

        printf(AZUL"  %d  "BRANCO"  %d  "AZUL"  %d  "BRANCO"  %d\n"RESET,
               processoBloqueado->pid, processoBloqueado->pcCounter,
               processoBloqueado->prioridade, processoBloqueado->quantum);

        celulaBloquados = celulaBloquados->pProx;
    }
    free(celulaBloquados);
    printf("\n");

    /////////////////////////////////////////////////////////////////

    printf(AZUL"-----------PROCESSOS PRONTOS-----------"RESET);
    printf(AZUL"quantidade de processos prontos para executar: "BRANCO"%d\n"RESET, info->qtdProcessosProntos);
    printf(AZUL"Filas de processos prontos: \n\n"RESET);

    for (int k = 0; k < 4; k++)
    {

        printf(AZUL"Fila Prontos ["BRANCO"%d"AZUL"]\n"RESET, k);

        TFila *filaProntos = &info->gerenciadorProcesso->estadoPronto[k];
        Celula *celulaProntos = filaProntos->pFrente->pProx;

        processo *processoPronto;
        printf(AZUL"  ID  |"BRANCO"  pcCounter  "AZUL"|  prioridade  |"BRANCO"  quantum  "AZUL"|\n"RESET);
        for (int i = 0; i < info->qtdProcessosProntos; i++)
        {
            processoPronto = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaProntos->Item.Chave);

            printf(AZUL"  %d  "BRANCO"  %d  "AZUL"  %d  "BRANCO"  %d\n"RESET,
                   processoPronto->pid, processoPronto->pcCounter,
                   processoPronto->prioridade, processoPronto->quantum);

            celulaProntos = celulaProntos->pProx;
        }
        free(celulaProntos);
        printf("\n");
    }
    /////////////////////////////////////////////////////////////////

    printf(AZUL"----------PROCESSOS EM EXECUCAO----------\n"RESET);

    printf(AZUL"quantidade de processos em execucao: "BRANCO"%d\n"RESET, info->qtdProcessosEmExecucao);
    printf(AZUL"Fila de processos em execucao: \n\n"RESET);

    Celula *celulaEmExecucao = info->gerenciadorProcesso->estadoEmExecucao.pFrente->pProx;
    processo *processoEmExecucao;
    printf(AZUL"  ID  |"BRANCO"  pcCounter  "AZUL"|  prioridade  |"BRANCO"  quantum  "AZUL"|\n"RESET);
    for (int i = 0; i < info->qtdProcessosEmExecucao; i++)
    {
        processoEmExecucao = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaEmExecucao->Item.Chave);

        printf(AZUL"  %d  "BRANCO"  %d  "AZUL"  %d  "BRANCO"  %d\n"RESET,
               processoEmExecucao->pid, processoEmExecucao->pcCounter,
               processoEmExecucao->prioridade, processoEmExecucao->quantum);

        celulaEmExecucao = celulaEmExecucao->pProx;
    }
    free(celulaEmExecucao);

    printf("\n");
    printf(AMARELO"Voltando para o menu...\n"RESET);
    printf("\n");


}

void ImprimeProcessosEmExecucao(Informacoes * info){

    printf(AZUL"----------PROCESSOS EM EXECUCAO----------\n"RESET);

    printf(AZUL"quantidade de processos em execucao: "BRANCO"%d\n"RESET, info->qtdProcessosEmExecucao);
    printf(AZUL"Fila de processos em execucao: \n\n"RESET);

    Celula *celulaEmExecucao = info->gerenciadorProcesso->estadoEmExecucao.pFrente->pProx;
    processo *processoEmExecucao;
    printf(AZUL"  ID  |"BRANCO"  pcCounter  "AZUL"|  prioridade  |"BRANCO"  quantum  "AZUL"|\n"RESET);
    for (int i = 0; i < info->qtdProcessosEmExecucao; i++)
    {
        processoEmExecucao = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaEmExecucao->Item.Chave);

        printf(AZUL"  %d  "BRANCO"  %d  "AZUL"  %d  "BRANCO"  %d\n"RESET,
               processoEmExecucao->pid, processoEmExecucao->pcCounter,
               processoEmExecucao->prioridade, processoEmExecucao->quantum);

        celulaEmExecucao = celulaEmExecucao->pProx;
    }
    free(celulaEmExecucao);
    printf("\n");
    printf(AMARELO"Voltando para o menu...\n"RESET);
    printf("\n");

}

void ImprimeProcessosProntos(Informacoes * info){

    printf(AZUL"-----------PROCESSOS PRONTOS-----------"RESET);
    printf(AZUL"quantidade de processos prontos para executar: "BRANCO"%d\n"RESET, info->qtdProcessosProntos);
    printf(AZUL"Filas de processos prontos: \n\n"RESET);

    for (int k = 0; k < 4; k++)
    {

        printf(AZUL"Fila Prontos ["BRANCO"%d"AZUL"]\n"RESET, k);

        TFila *filaProntos = &info->gerenciadorProcesso->estadoPronto[k];
        Celula *celulaProntos = filaProntos->pFrente->pProx;

        processo *processoPronto;
        printf(AZUL"  ID  |"BRANCO"  pcCounter  "AZUL"|  prioridade  |"BRANCO"  quantum  "AZUL"|\n"RESET);
        for (int i = 0; i < info->qtdProcessosProntos; i++)
        {
            processoPronto = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaProntos->Item.Chave);

            printf(AZUL"  %d  "BRANCO"  %d  "AZUL"  %d  "BRANCO"  %d\n"RESET,
                   processoPronto->pid, processoPronto->pcCounter,
                   processoPronto->prioridade, processoPronto->quantum);

            celulaProntos = celulaProntos->pProx;
        }
        free(celulaProntos);
        printf("\n");
    }
    printf("\n");
    printf(AMARELO"Voltando para o menu...\n"RESET);
    printf("\n");
}

void ImprimeProcessosBloqueados(Informacoes * info){

    printf(AZUL"----------PROCESSOS BLOQUEADOS----------"RESET);
    printf(AZUL"quantidade de processos bloqueados: "BRANCO"%d \n"RESET, info->qtdProcessosBloqueados);
    printf(AZUL"Fila de processos bloqueados: \n\n"RESET);

    Celula *celulaBloquados = info->gerenciadorProcesso->estadoBloquado.pFrente->pProx;
    processo *processoBloqueado;
    printf(AZUL"  ID  |"BRANCO"  pcCounter  "AZUL"|  prioridade  |"BRANCO"  quantum  "AZUL"|\n"RESET);
    for (int i = 0; i < info->qtdProcessosBloqueados; i++)
    {
        processoBloqueado = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaBloquados->Item.Chave);

        printf(AZUL"  %d  "BRANCO"  %d  "AZUL"  %d  "BRANCO"  %d\n"RESET,
               processoBloqueado->pid, processoBloqueado->pcCounter,
               processoBloqueado->prioridade, processoBloqueado->quantum);

        celulaBloquados = celulaBloquados->pProx;
    }
    free(celulaBloquados);
    printf("\n");
    printf(AMARELO"Voltando para o menu...\n"RESET);
    printf("\n");
}

void ImprimeInformacoesGerais(Informacoes * info){

    printf(AZUL"Informacoes do Gerenciador de Processos:\n"RESET);
    printf(AZUL"Total de processo finalizados:  "BRANCO"%d\n"RESET, info->gerenciadorProcesso->totalProcessosFinalizados);
    printf(AZUL"Tempo:  "BRANCO"%d\n"RESET, info->gerenciadorProcesso->tempo.valor);
    
    printf("\n");

    printf(AZUL"Informacoes da CPU:\n"RESET);
    printf(AZUL"Indice da proxima instrucao (PC):  "BRANCO"%d\n"RESET, info->gerenciadorProcesso->cpu.registradorPC);
    printf(AZUL"Processo atual:  "BRANCO"%d\n"RESET, info->processoAtual);
    printf(AZUL"Quantum alocado para o processo atual:  "BRANCO"%d\n"RESET, info->gerenciadorProcesso->cpu.quantum_total);
    
    printf("\n");
    printf(AMARELO"Voltando para o menu...\n"RESET);
    printf("\n");
}