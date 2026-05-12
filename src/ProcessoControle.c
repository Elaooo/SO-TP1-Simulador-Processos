#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <../include/TAD_GerenciadorProcesso.h>

int inicializaProcessoControle(int argc, char *argv[]);



int inicializaProcessoControle(int argc, char *argv[]){
    int fd[2];

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
        rodarGerenciador(fd[0]);
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

            write(fd[1], &comando, sizeof(char));

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