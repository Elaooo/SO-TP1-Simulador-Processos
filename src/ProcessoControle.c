#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "../include/TAD_GerenciadorProcesso.h"
#include "../include/ProcessoControle.h"

#define AZUL "\033[34m"
#define AMARELO "\033[33m"
#define VERMELHO "\033[31m"
#define BRANCO "\033[37m"
#define RESET "\033[0m"

int inicializaProcessoControle(int argc, char *argv[])
{
    int fd[2];

    int op = -999;
    do
    {
        printf("Escolha qual escalonador usar:\n");
        printf("0 - Escalonador MLFQ:\n");
        printf("1 - Escalonador FIFO:\n");
        scanf("%d", &op);
    } while (op != 1 && op != 0);
    if (pipe(fd) == -1)
    {
        perror("Erro ao criar pipe");
        exit(1);
    }
    char comando;
    ComandoPipe msg;
    msg.tipo = comando;
    msg.opcaoImpressao = -1;
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Erro no fork");
        exit(1);
    }

    if (pid == 0)
    {
        // Processo filho: gerenciador de processos
        close(fd[1]);                // filho não escreve no pipe
        rodarGerenciador(fd[0], op, msg); // receber do usuario
    }
    else
    {
        // Processo pai: processo controle
        close(fd[0]); // pai não lê do pipe

        FILE *entrada = stdin;

        if (argc > 1)
        {
            entrada = fopen(argv[1], "r");
            if (entrada == NULL)
            {
                perror("Erro ao abrir arquivo de comandos");
                close(fd[1]);
                wait(NULL);
                exit(1);
            }
        }

        

        while (fscanf(entrada, " %c", &comando) == 1)
        {
            comando = toupper(comando);

            if (comando != 'U' && comando != 'I' && comando != 'M')
            {
                printf("[Controle] Comando inválido ignorado: %c\n", comando);
                continue;
            }

            if (comando == 'I' || comando == 'M')
            {
                msg.opcaoImpressao = lerOpcaoImpressao();
            }

            write(fd[1], &msg, sizeof(ComandoPipe));

            if (comando == 'M')
            {
                break;
            }
        }
        if (entrada != stdin)
        {
            fclose(entrada);
        }

        close(fd[1]); // avisa EOF ao gerenciador
        wait(NULL);   // espera o gerenciador terminar

        printf("[Controle] Simulação finalizada.\n");
    }

    return 0;
}

int lerOpcaoImpressao()
{
    int opcao = -1;

    printf(AZUL "----------------------------------------------------------\n" RESET);
    printf(AZUL "Bem vindo ao menu de impressao do seu sistema operacional!\n" RESET);
    printf(AZUL "----------------------------------------------------------\n" RESET);

    do
    {
        printf(AZUL "O que deseja visualizar?\n" RESET);
        printf(AZUL "1-" BRANCO " Todos os processos\n" RESET);
        printf(AZUL "2-" BRANCO " Processos em execucao\n" RESET);
        printf(AZUL "3-" BRANCO " Processos prontos para executar\n" RESET);
        printf(AZUL "4-" BRANCO " Processos bloqueados\n" RESET);
        printf(AZUL "5-" BRANCO " Informacoes gerais\n" RESET);
        printf(AZUL "Opcao: " RESET);
        fflush(stdin);
        scanf("%d", &opcao);

        if (opcao < 1 || opcao > 5)
        {
            printf(VERMELHO "Opcao invalida! Tente novamente\n" RESET);
        }

    } while (opcao < 1 || opcao > 5);
    fflush(stdin);
    return opcao;
}