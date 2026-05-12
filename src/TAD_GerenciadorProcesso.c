#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <../include/TAD_GerenciadorProcesso.h>
#include "../include/TAD_LeituraArquivo.h"


int leituraProcessoInit(TabelaDeProcessos *tabelaProcessos){
    //variaveis leitura
    char caminho[256] = "data/init.txt";
    char linha[256];
    char comando;
    int x;
    int n;
    int iterador = 0;
    
    FILE *arquivo = fopen(caminho, "r");
    if(!arquivo){
        
        printf("Falha ao abrir arquivo, necessita arquivo init.txt em /data/\n");
        return 0;
    }
    
    //variaveis processo
    int qntdInstruções = contarLinhasArquivo(caminho);
    int pidInicial = 0;
    processo processo;
    
    instrucao *listaInstrucoes = malloc(qntdInstruções * sizeof(instrucao));;

    if (!listaInstrucoes) {
        printf("Erro de alocacao\n");
    return 0;
    }

    //aqui comeca leitura
    while (fgets(linha, sizeof(linha), arquivo)) {

        comando = ' ';
        n = 0;
        x = 0;

        if (sscanf(linha, " %c", &comando) != 1) 
            continue;

        switch (comando) {

            case 'N':
                sscanf(linha, " %c %d", &comando, &n);
                printf("Comando N\n");
                printf("N: %d\n\n", n);
                listaInstrucoes[iterador].tipo = comando;
                listaInstrucoes[iterador].n = n;
                break;

            case 'D':
                sscanf(linha, " %c %d", &comando, &x);
                printf("Comando D\n");
                printf("X: %d\n\n", x);
                listaInstrucoes[iterador].tipo = comando;
                listaInstrucoes[iterador].x = x;
                break;

            case 'V':
                sscanf(linha, " %c %d %d", &comando, &x, &n);
                printf("Comando V\n");
                printf("X: %d | N: %d\n\n", x, n);
                listaInstrucoes[iterador].tipo = comando;
                listaInstrucoes[iterador].x = x;
                listaInstrucoes[iterador].n = n;
                break;

            case 'A':
            case 'S':
                sscanf(linha, " %c %d %d", &comando, &x, &n);
                printf("Comando %c\n", comando);
                printf("X: %d | N: %d\n\n", x, n);
                listaInstrucoes[iterador].tipo = comando;
                listaInstrucoes[iterador].x = x;
                listaInstrucoes[iterador].n = n;
                break;

            case 'R':
                sscanf(linha, " %c %s", &comando, caminho);
                printf("Comando R\n");
                printf("Arquivo: %s\n\n", caminho);
                listaInstrucoes[iterador].tipo = comando;
                strcpy(listaInstrucoes[iterador].caminhoArquivo,caminho);
                break;

            case 'F':
                sscanf(linha, " %c %d", &comando, &x);
                printf("Comando F\n");
                printf("X: %d\n\n", x);
                listaInstrucoes[iterador].tipo = comando;
                listaInstrucoes[iterador].x = x;
                break;
            case 'T':
                sscanf(linha, " %c", &comando);
                printf("Comando %c\n", comando);
                listaInstrucoes[iterador].tipo = comando;
                printf("\n");
                printf("-----encerra execucao-----");
                break;

            default:
                printf("Comando desconhecido: %c\n\n", comando);
                break;
        }
    iterador++;
}
    // printf("Instruçoes guardadas\n");
    inicializarProcessoInit(&processo, pidInicial,listaInstrucoes,qntdInstruções);
    
    // imprimirInstrucoes(processo.listaInstrucoes,qntdInstruções);

    inserirProcessoTabela(tabelaProcessos, &processo);
    free(listaInstrucoes);


    fclose(arquivo);

    return 1;
}

int inicializaGerenciadorProcessos(GerenciadorProcesso *gerenciadorProcessos){

    inicializarTabelaProcessos(&gerenciadorProcessos->tabelaProcessos);
    inicializarCPU(&gerenciadorProcessos->cpu);
    InicializaTempo(&gerenciadorProcessos->tempo);
    FazFilaVazia(&gerenciadorProcessos->estadoPronto);
    FazFilaVazia(&gerenciadorProcessos->estadoEmExecucao);
    FazFilaVazia(&gerenciadorProcessos->estadoBloquado);

    if(gerenciadorProcessos == NULL){
        return 0;
    }else{
        return 1;
    }

}