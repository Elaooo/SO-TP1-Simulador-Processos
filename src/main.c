#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "../include/ProcessoControle.h"
#include "../include/GerenciadorProcesso.h"
#include "../include/FilaComandos.h"
#include "../include/Globais.h"


int main(int argc, char *argv[]) {

    printf("\n===== Simulador de Gerenciamento de Processos com threads =====\n\n");
    ArgsGerenciador argsGer;
    escolherArgsGerenciador(&argsGer); // escolhe antes de tudo

    pthread_t tidControle;
    pthread_t tidGerenciador;

    
    ArgsControle *args = malloc(sizeof(ArgsControle) + argc * sizeof(char *));

    args->argc = argc;
    for (int i = 0; i < argc; i++) {
    args->argv[i] = argv[i];
    }

    inicializaFilaComandos(&filaComandos);

    //Cria thread controle
    if (pthread_create(&tidControle, NULL, threadControle, args) != 0) {
        perror("Erro ao criar thread controle");
        exit(1);
    }


    //Cria thread gerenciador
    if (pthread_create(&tidGerenciador, NULL, rodarGerenciador, &argsGer) != 0) {
        perror("Erro ao criar thread gerenciador");
        exit(1);
    }


    //Espera ambas terminarem
    pthread_join(tidControle, NULL);
    pthread_join(tidGerenciador, NULL);

    printf("Simulacao finalizada.\n");

    return 0;
}

