#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "../include/GerenciadorProcesso.h"
#include "../include/ProcessoControle.h"

#define AZUL "\033[34m"
#define AMARELO "\033[33m"
#define VERMELHO "\033[31m"
#define BRANCO "\033[37m"
#define RESET "\033[0m"

int inicializaProcessoControle(int argc, char *argv[])
{
    int fd[2];

    char op = ' ';
    int opcaoEscolhida = -1;
    char op2 = ' ';
    int opcaoEscolhida2 = -1;
    do
    {
        printf("Escolha qual escalonador usar:\n");
        printf("0 - Escalonador MLFQ:\n");
        printf("1 - Escalonador FIFO:\n");
        scanf(" %c", &op);
        if(op != '1' && op != '0'){
            printf("Opcao invalida! Tente novamente.\n");
        }
    } while (op != '1' && op != '0');
    if(op == '1'){
        opcaoEscolhida = 1;
    }else{
        opcaoEscolhida = 0;
    }
    do
    {
        printf("Escolha quantas CPUs utilizar:\n");
        printf("1 - Uma CPU:\n");
        printf("2 - Duas CPUs:\n");
        printf("3 - Quatro CPUs:\n");
        scanf(" %c", &op2);
        if(op2 != '1' && op2 != '2' && op2 != '3'){
            printf("Opcao invalida! Tente novamente.\n");
        }
    } while (op2 != '1' && op2 != '2' && op2 != '3');
    if(op2 == '1'){
        opcaoEscolhida2 = 0;
    }else if (op2 == '2'){
        opcaoEscolhida2 = 1;
    }else{
        opcaoEscolhida2 = 3;
    }

    if (pipe(fd) == -1)
    {
        perror("Erro ao criar pipe");
        exit(1);
    }
    char comando = 'Z';

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
        rodarGerenciador(fd[0], opcaoEscolhida,opcaoEscolhida2); // receber do usuario
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

        
        // Leitura dos comandos U,I,M
        while (fscanf(entrada, " %c", &comando) == 1)
        {
            comando = toupper(comando);

            if (comando != 'U' && comando != 'I' && comando != 'M')
            {
                printf("[Controle] Comando inválido ignorado: %c\n", comando);
                continue;
            }
            msg.tipo = comando;
            if (comando == 'I' || comando == 'M')
            {
                if(argc > 1){
                    fscanf(entrada, " %d", &msg.opcaoImpressao);
                }else{
                    msg.opcaoImpressao = lerOpcaoImpressao();
                }
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
    char opcao = ' ';
    int opcaoEscolhida = -1;

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
        scanf(" %c", &opcao);

        if (opcao != '1' && opcao != '2' && opcao != '3' && opcao != '4' && opcao != '5')
        {
            printf(VERMELHO "Opcao invalida! Tente novamente\n" RESET);
        }

    } while (opcao != '1' && opcao != '2' && opcao != '3' && opcao != '4' && opcao != '5');
    fflush(stdin);
    switch (opcao)
    {
    case '1':
        opcaoEscolhida = 1;
        break;
    case '2':
        opcaoEscolhida = 2;
        break;
    case '3':
        opcaoEscolhida = 3;
        break;
    case '4':
        opcaoEscolhida = 4;
        break;
    case '5':
        opcaoEscolhida = 5;
        break;
    default:
        break;
    }

    return opcaoEscolhida;
}