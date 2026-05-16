#include "../include/processoImpressao.h"

#include <stdio.h>
#include <unistd.h>

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
    fflush(stdout);

    _exit(0);
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

    printf(AZUL"----------PROCESSOS BLOQUEADOS----------\n"RESET);
    printf("%squantidade de processos bloqueados:  %s%d %s\n",AZUL, BRANCO,info->qtdProcessosBloqueados, RESET);
    printf(AZUL"Fila de processos bloqueados: \n\n"RESET);

    Celula *celulaBloquados = info->gerenciadorProcesso->estadoBloquado.pFrente->pProx;
    processo *processoBloqueado;
    printf(AZUL"  ID  |"BRANCO"  pcCounter  "AZUL"|  prioridade  |"BRANCO"  quantum  "AZUL"|\n"RESET);
    for (int i = 0; i < info->qtdProcessosBloqueados; i++)
    {
        processoBloqueado = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaBloquados->Item.Chave);

        printf("%s  %d  %s  %d  %s  %d  %s  %d\n%s",AZUL,
               processoBloqueado->pid,BRANCO, processoBloqueado->pcCounter,AZUL,
               processoBloqueado->prioridade,BRANCO, processoBloqueado->quantum, RESET);

        celulaBloquados = celulaBloquados->pProx;
    }
    
    printf("\n");

    /////////////////////////////////////////////////////////////////

    printf(AZUL"-----------PROCESSOS PRONTOS-----------\n"RESET);
    printf("%squantidade de processos prontos para executar: %s%d\n%s", AZUL,BRANCO,info->qtdProcessosProntos,RESET);
    printf(AZUL"Filas de processos prontos: \n\n"RESET);

    for (int k = 0; k < 4; k++)
    {

        printf("%sFila Prontos [%s%d%s]\n%s", AZUL,BRANCO,k,AZUL,RESET);

        TFila *filaProntos = &info->gerenciadorProcesso->estadoPronto[k];
        Celula *celulaProntos = filaProntos->pFrente->pProx;

        processo *processoPronto;
        printf(AZUL"  ID  |"BRANCO"  pcCounter  "AZUL"|  prioridade  |"BRANCO"  quantum  "AZUL"|\n"RESET);
        for (int i = 0; i < info->qtdProcessosProntos; i++)
        {
            processoPronto = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaProntos->Item.Chave);

            printf("%s  %d  %s  %d  %s  %d  C  %d\n%s",AZUL,
               processoPronto->pid,BRANCO, processoPronto->pcCounter,AZUL,
               processoPronto->prioridade,BRANCO, processoPronto->quantum, RESET);

            celulaProntos = celulaProntos->pProx;
        }
        
        printf("\n");
    }
    /////////////////////////////////////////////////////////////////

    printf(AZUL"----------PROCESSOS EM EXECUCAO----------\n"RESET);

    printf("%squantidade de processos em execucao: %s%d\n%s", AZUL,BRANCO,info->qtdProcessosEmExecucao,RESET);
    printf(AZUL"Fila de processos em execucao: \n\n"RESET);

    Celula *celulaEmExecucao = info->gerenciadorProcesso->estadoEmExecucao.pFrente->pProx;
    processo *processoEmExecucao;
    printf(AZUL"  ID  |"BRANCO"  pcCounter  "AZUL"|  prioridade  |"BRANCO"  quantum  "AZUL"|\n"RESET);
    for (int i = 0; i < info->qtdProcessosEmExecucao; i++)
    {
        processoEmExecucao = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaEmExecucao->Item.Chave);

        printf("%s  %d  %s  %d  %s  %d  %s  %d\n%s",AZUL,
               processoEmExecucao->pid,BRANCO, processoEmExecucao->pcCounter,AZUL,
               processoEmExecucao->prioridade,BRANCO, processoEmExecucao->quantum, RESET);

        celulaEmExecucao = celulaEmExecucao->pProx;
    }
    

    printf("\n");
    printf(AMARELO"Voltando para o menu...\n"RESET);
    printf("\n");


}

void ImprimeProcessosEmExecucao(Informacoes * info){

    printf(AZUL"----------PROCESSOS EM EXECUCAO----------\n"RESET);

    printf("%squantidade de processos em execucao: %s%d\n%s", AZUL,BRANCO,info->qtdProcessosEmExecucao,RESET);
    printf(AZUL"Fila de processos em execucao: \n\n"RESET);

    Celula *celulaEmExecucao = info->gerenciadorProcesso->estadoEmExecucao.pFrente->pProx;
    processo *processoEmExecucao;
    printf(AZUL"  ID  |"BRANCO"  pcCounter  "AZUL"|  prioridade  |"BRANCO"  quantum  "AZUL"|\n"RESET);
    for (int i = 0; i < info->qtdProcessosEmExecucao; i++)
    {
        processoEmExecucao = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaEmExecucao->Item.Chave);

        printf("%s  %d  %s  %d  %s  %d  %s  %d\n%s",AZUL,
               processoEmExecucao->pid,BRANCO, processoEmExecucao->pcCounter,AZUL,
               processoEmExecucao->prioridade,BRANCO, processoEmExecucao->quantum, RESET);

        celulaEmExecucao = celulaEmExecucao->pProx;
    }
    
    printf("\n");
    printf(AMARELO"Voltando para o menu...\n"RESET);
    printf("\n");

}

void ImprimeProcessosProntos(Informacoes * info){

    printf(AZUL"-----------PROCESSOS PRONTOS-----------\n"RESET);
    printf("%squantidade de processos prontos para executar: %s%d\n%s",AZUL,BRANCO, info->qtdProcessosProntos,RESET);
    printf(AZUL"Filas de processos prontos: \n\n"RESET);

    for (int k = 0; k < 4; k++)
    {

        printf("%sFila Prontos [%s%d%s]\n%s", AZUL,BRANCO,k,AZUL,RESET);

        TFila *filaProntos = &info->gerenciadorProcesso->estadoPronto[k];
        Celula *celulaProntos = filaProntos->pFrente->pProx;

        processo *processoPronto;
        printf(AZUL"  ID  |"BRANCO"  pcCounter  "AZUL"|  prioridade  |"BRANCO"  quantum  "AZUL"|\n"RESET);
        for (int i = 0; i < info->qtdProcessosProntos; i++)
        {
            processoPronto = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaProntos->Item.Chave);

            printf("%s  %d  %s  %d  %s  %d  C  %d\n%s",AZUL,
               processoPronto->pid,BRANCO, processoPronto->pcCounter,AZUL,
               processoPronto->prioridade,BRANCO, processoPronto->quantum, RESET);

            celulaProntos = celulaProntos->pProx;
        }
        
        printf("\n");
    }
    printf("\n");
    printf(AMARELO"Voltando para o menu...\n"RESET);
    printf("\n");
}

void ImprimeProcessosBloqueados(Informacoes * info){

    printf(AZUL"----------PROCESSOS BLOQUEADOS----------\n"RESET);
    printf("%squantidade de processos bloqueados:  %s%d %s\n",AZUL, BRANCO,info->qtdProcessosBloqueados, RESET);
    printf(AZUL"Fila de processos bloqueados: \n\n"RESET);

    Celula *celulaBloquados = info->gerenciadorProcesso->estadoBloquado.pFrente->pProx;
    processo *processoBloqueado;
    printf(AZUL"  ID  |"BRANCO"  pcCounter  "AZUL"|  prioridade  |"BRANCO"  quantum  "AZUL"|\n"RESET);
    for (int i = 0; i < info->qtdProcessosBloqueados; i++)
    {
        processoBloqueado = buscarProcessoTabela(&info->gerenciadorProcesso->tabelaProcessos, celulaBloquados->Item.Chave);

        printf("%s  %d  %s  %d  %s  %d  %s  %d\n%s",AZUL,
               processoBloqueado->pid,BRANCO, processoBloqueado->pcCounter,AZUL,
               processoBloqueado->prioridade,BRANCO, processoBloqueado->quantum, RESET);

        celulaBloquados = celulaBloquados->pProx;
    }
    
    printf("\n");
    printf(AMARELO"Voltando para o menu...\n"RESET);
    printf("\n");
}

void ImprimeInformacoesGerais(Informacoes * info){

    printf(AZUL"Informacoes do Gerenciador de Processos:\n"RESET);
    printf("Total de processo finalizados:  %s%d\n%s",AZUL,BRANCO, info->gerenciadorProcesso->totalProcessosFinalizados,RESET);
    printf("%sTempo:  %s%d\n%s",AZUL, BRANCO ,info->gerenciadorProcesso->tempo.valor,RESET);
    
    printf("\n");

    printf(AZUL"Informacoes da CPU:\n"RESET);
    printf("%sIndice da proxima instrucao (PC):  %s%d\n%s", AZUL,BRANCO,info->gerenciadorProcesso->cpu.registradorPC,RESET);
    printf("%sProcesso atual:  %s%d\n%s",AZUL, BRANCO, info->processoAtual, RESET);
    printf("%sQuantum alocado para o processo atual:  %s%d\n%s",AZUL, BRANCO ,info->gerenciadorProcesso->cpu.quantum_total,RESET);
    
    printf("\n");
    printf(AMARELO"Voltando para o menu...\n"RESET);
    printf("\n");
}