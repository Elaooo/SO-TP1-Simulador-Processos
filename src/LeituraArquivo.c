#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/ProcessoSimulado.h"
#include "../include/Cpu.h"
#include "../include/Globais.h"

int contarLinhasArquivo(char *caminho) {
    FILE *arquivo = fopen(caminho, "r");
    if (!arquivo) {
        printf("Erro ao abrir arquivo\n");
        return -1;
    }

    char linha[200];
    int contador = 0;

    while (fgets(linha, sizeof(linha), arquivo)) {
        contador++;
    }

    fclose(arquivo);
    return contador;
}

int leituraArquivoProcesso(char *caminho, cpu_s *cpu){
    char temp[256];

    snprintf(temp, sizeof(temp), "data/%s.txt", caminho);
    strcpy(caminho, temp);
    //variaveis leitura
    char linha[256];
    char comando;
    int x;
    int n;
    int iterador = 0;

    
    FILE *arquivo = fopen(caminho, "r");
    if(!arquivo){
        
        printf("Falha ao abrir arquivo, não existe %s em data\n", caminho);
        return 0;
    }
    
    //variaveis processo
    int qntdInstrucoes = contarLinhasArquivo(caminho);
    instrucao *listaInstrucoes = malloc(qntdInstrucoes * sizeof(instrucao));

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
            //printf("Comando N\n");
            //printf("N: %d\n\n", n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].n = n;
            listaInstrucoes[iterador].x = 0;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;

        case 'D':
            sscanf(linha, " %c %d", &comando, &x);
            //printf("Comando D\n");
            //printf("X: %d\n\n", x);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            listaInstrucoes[iterador].n = 0;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;

            case 'V':
            sscanf(linha, " %c %d %d", &comando, &x, &n);
            //printf("Comando V\n");
            //printf("X: %d | N: %d\n\n", x, n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            listaInstrucoes[iterador].n = n;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;

            case 'A':
            case 'S':
                sscanf(linha, " %c %d %d", &comando, &x, &n);
                //printf("Comando %c\n", comando);
                //printf("X: %d | N: %d\n\n", x, n);
                listaInstrucoes[iterador].tipo = comando;
                listaInstrucoes[iterador].x = x;
                listaInstrucoes[iterador].n = n;
                strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
                break;

            case 'B':
                sscanf(linha, " %c %d", &comando, &n);
                //printf("Comando N\n");
                //printf("N: %d\n\n", n);
                listaInstrucoes[iterador].tipo = comando;
                listaInstrucoes[iterador].x = 0;
                listaInstrucoes[iterador].n = n;
                strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
                break;

            case 'R':
                sscanf(linha, " %c %s", &comando, caminho);
                //printf("Comando R\n");
                //printf("Arquivo: %s\n\n", caminho);
                listaInstrucoes[iterador].tipo = comando;
                strcpy(listaInstrucoes[iterador].caminhoArquivo,caminho);
                break;

            case 'F':
                sscanf(linha, " %c %d", &comando, &n);
                //printf("Comando F\n");
                //printf("X: %d\n\n", x);
                listaInstrucoes[iterador].tipo = comando;
                listaInstrucoes[iterador].n = n;
                listaInstrucoes[iterador].x = 0;
                strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
                break;
            case 'T':
                sscanf(linha, " %c", &comando);
                //printf("Comando %c\n", comando);
                listaInstrucoes[iterador].tipo = comando;
                listaInstrucoes[iterador].n = 0;
                listaInstrucoes[iterador].x = 0;
                strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
                //printf("\n");
                //printf("-----encerra execucao-----");
                break;

            default:
                printf("Comando desconhecido: %c\n\n", comando);
                break;
        }
        iterador++;
}
    printf("--- Instruçoes guardadas.\n");
    inicializarProcesso(cpu->processo_atual,listaInstrucoes,qntdInstrucoes);
    //imprimirInstrucoes(cpu->processo_atual->listaInstrucoes,qntdInstrucoes);
    free(listaInstrucoes);

    fclose(arquivo);

    return 1;
}

int leituraProcessoInit(processo *processo)
{
    // variaveis leitura
    char caminho[256] = "data/init.txt";
    char linha[256];
    char comando;
    int x;
    int n;
    int iterador = 0;

    FILE *arquivo = fopen(caminho, "r");
    if (!arquivo)
    {

        printf("Falha ao abrir arquivo, necessita arquivo init.txt em /data/\n");
        return 0;
    }

    // variaveis processo
    int qntdInstrucoes = contarLinhasArquivo(caminho);

    instrucao *listaInstrucoes = malloc(qntdInstrucoes * sizeof(instrucao));

    if (!listaInstrucoes)
    {
        printf("Erro de alocacao\n");
        return 0;
    }

    // aqui comeca leitura
    while (fgets(linha, sizeof(linha), arquivo))
    {

        comando = ' ';
        n = 0;
        x = 0;

        if (sscanf(linha, " %c", &comando) != 1)
            continue;

        switch (comando)
        {

        case 'N':
            sscanf(linha, " %c %d", &comando, &n);
            // printf("Comando N\n");
            // printf("N: %d\n\n", n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].n = n;
            listaInstrucoes[iterador].x = 0;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;

        case 'D':
            sscanf(linha, " %c %d", &comando, &x);
            // printf("Comando D\n");
            // printf("X: %d\n\n", x);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            listaInstrucoes[iterador].n = 0;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;

        case 'B':
            sscanf(linha, " %c %d", &comando, &n);
            // printf("Comando V\n");
            // printf("X: %d | N: %d\n\n", x, n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = 0;
            listaInstrucoes[iterador].n = n;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;

        case 'V':
            sscanf(linha, " %c %d %d", &comando, &x, &n);
            // printf("Comando V\n");
            // printf("X: %d | N: %d\n\n", x, n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            listaInstrucoes[iterador].n = n;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;

        case 'A':
        case 'S':
            sscanf(linha, " %c %d %d", &comando, &x, &n);
            // printf("Comando %c\n", comando);
            // printf("X: %d | N: %d\n\n", x, n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            listaInstrucoes[iterador].n = n;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;

        case 'R':
            sscanf(linha, " %c %s", &comando, caminho);
            // printf("Comando R\n");
            // printf("Arquivo: %s\n\n", caminho);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].n = 0;
            listaInstrucoes[iterador].x = 0;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, caminho);
            break;
            break;

        case 'F':
            sscanf(linha, " %c %d", &comando, &n);
            // printf("Comando F\n");
            // printf("X: %d\n\n", x);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].n = n;
            listaInstrucoes[iterador].x = 0;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;
        case 'T':
            sscanf(linha, " %c", &comando);
            // printf("Comando %c\n", comando);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].n = 0;
            listaInstrucoes[iterador].x = 0;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            // printf("\n");
            // printf("-----encerra execucao-----");
            break;
            break;

        default:
            printf("Comando desconhecido: %c\n\n", comando);
            break;
        }
        iterador++;
    }
    // printf("Instruçoes guardadas\n");
    inicializarProcessoInit(processo, listaInstrucoes, qntdInstrucoes);

    // imprimirInstrucoes(processo.listaInstrucoes,qntdInstrucoes);
    free(listaInstrucoes);

    fclose(arquivo);

    return 1;
}
//lembrar em casos que podem dar erro: reutilização de variaveis, contagem de instruções antes de atribuir tamanho a listaInstrucoesIncial