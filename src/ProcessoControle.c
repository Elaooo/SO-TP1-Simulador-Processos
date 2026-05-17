#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "../include/TAD_GerenciadorProcesso.h"
#include "../include/ProcessoControle.h"

int inicializaProcessoControle(int argc, char *argv[]){
    int fd[2];

    int op = -999;
    do{
        printf("Escolha qual escalonador usar:\n");
        printf("0 - Escalonador MLFQ:\n");
        printf("1 - Escalonador FIFO:\n");
        scanf("%d",&op);
    }while(op != 1 && op != 0);
    if (pipe(fd) == -1) {
        perror("Erro ao criar pipe");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Erro no fork");
        exit(1);
    }

    if (pid == 0) {
        //Processo filho: gerenciador de processos
        close(fd[1]); // filho não escreve no pipe
        rodarGerenciador(fd[0],op); // receber do usuario
    } else {
        //Processo pai: processo controle
        close(fd[0]); // pai não lê do pipe

        FILE *entrada = stdin;

        if (argc > 1) {
            entrada = fopen(argv[1], "r");
            if (entrada == NULL) {
                perror("Erro ao abrir arquivo de comandos");
                close(fd[1]);
                wait(NULL);
                exit(1);
            }
        }

        char comando;

        while (fscanf(entrada, " %c", &comando) == 1) {
            comando = toupper(comando);

            if (comando != 'U' && comando != 'I' && comando != 'M') {
                printf("[Controle] Comando inválido ignorado: %c\n", comando);
                continue;
            }

            ComandoPipe msg;
            msg.tipo = comando;
            msg.opcaoImpressao = -1;

            if (comando == 'I') {
            msg.opcaoImpressao = lerOpcaoImpressao();
            }

            if (comando == 'M') {
                msg.opcaoImpressao = 5;
            }

            write(fd[1], &msg, sizeof(ComandoPipe));

            if (comando == 'M') {
                break;
            }
        }
        if (entrada != stdin) {
            fclose(entrada);
        }

        close(fd[1]); // avisa EOF ao gerenciador
        wait(NULL);   // espera o gerenciador terminar

        printf("[Controle] Simulação finalizada.\n");
    }

    return 0;
}

int lerOpcaoImpressao() {
    int opcao = -1;

    do {
        printf("\nO que deseja visualizar?\n");
        printf("1 - Todos os processos\n");
        printf("2 - Processos em execucao\n");
        printf("3 - Processos prontos\n");
        printf("4 - Processos bloqueados\n");
        printf("5 - Informacoes gerais\n");
        printf("0 - Sair\n");
        printf("Opcao: ");

        if (scanf("%d", &opcao) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            opcao = -1;
        }

    } while (opcao < 0 || opcao > 5);

    return opcao;
}