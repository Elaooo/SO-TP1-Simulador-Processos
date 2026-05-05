#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/TAD_processo_simulado.h"


int leituraArquivoProcesso(){

     char caminho[256] = "data/file_a.txt";
    FILE *arquivo = fopen(caminho, "r");

    if(!arquivo){
        printf("Falha ao abrir arquivo\n");
        return 0;
    }

    char linha[200];
    char comando;
    int x;
    int n;

    while (fgets(linha, sizeof(linha), arquivo)) {

    if (sscanf(linha, " %c", &comando) != 1) 
        continue;

    switch (comando) {

        case 'N':
            sscanf(linha, " %c %d", &comando, &n);
            printf("Comando N\n");
            printf("N: %d\n\n", n);
            break;

        case 'D':
            sscanf(linha, " %c %d", &comando, &x);
            printf("Comando D\n");
            printf("X: %d\n\n", x);
            break;

        case 'V':
            sscanf(linha, " %c %d %d", &comando, &x, &n);
            printf("Comando V\n");
            printf("X: %d | N: %d\n\n", x, n);
            break;

        case 'A':
        case 'S':
            sscanf(linha, " %c %d %d", &comando, &x, &n);
            printf("Comando %c\n", comando);
            printf("X: %d | N: %d\n\n", x, n);
            break;

        case 'R':
            sscanf(linha, " %c %s", &comando, caminho);
            printf("Comando R\n");
            printf("Arquivo: %s\n\n", caminho);
            break;

        case 'F':
            sscanf(linha, " %c %d", &comando, &x);
            printf("Comando F\n");
            printf("X: %d\n\n", x);
            break;
        case 'T':
            sscanf(linha, " %c %d", &comando);
            printf("Comando %c\n", comando);
            printf("\n");
            printf("-----encerra execucao-----");
            break;

        default:
            printf("Comando desconhecido: %c\n\n", comando);
            break;
    }
}

    fclose(arquivo);

    return 1;
}