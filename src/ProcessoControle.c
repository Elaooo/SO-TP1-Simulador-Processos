#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "../include/GerenciadorProcesso.h"
#include "../include/ProcessoControle.h"
#include "../include/Globais.h"
#include "../include/FilaComandos.h"

#define AZUL "\033[34m"
#define AMARELO "\033[33m"
#define VERMELHO "\033[31m"
#define BRANCO "\033[37m"
#define RESET "\033[0m"

void *threadControle(void *arg)
{
    ArgsControle *args = (ArgsControle *) arg;
    int argc = args->argc;
    char **argv = args->argv;


    char comando = 'Z';
    FILE *entrada = stdin;

    Comando msg;
    msg.tipo = comando;
    msg.opcaoImpressao = -1;
    if (argc > 1)
    {
        entrada = fopen(argv[1], "r");
        if (entrada == NULL)
        {
            perror("Erro ao abrir arquivo de comandos");
            exit(1);
        }
    }

    // Leitura dos comandos U,I,M
    while (fscanf(entrada, " %c", &comando) == 1){
        comando = toupper(comando);

        if (comando != 'U' && comando != 'I' && comando != 'M'){
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
        enfileiraComando(&filaComandos, msg);
        //pthread_mutex_lock(&filaComandos.mutex);

        //filaComandos.fila[filaComandos.fim] = msg;
        //filaComandos.fim++;

        //pthread_cond_signal(&filaComandos.cond);

        //pthread_mutex_unlock(&filaComandos.mutex);
        
        if (comando == 'M'){
            break;
        }

    }
    if (entrada != stdin)
        {
            fclose(entrada);
        }
    printf("[Controle] Simulação finalizada.\n");
    return NULL;
}

void escolherArgsGerenciador(ArgsGerenciador * aux) {
    char op = ' ';
    char op2 = ' ';
    aux->tipoEscalonador = -1;
    aux->qtdCPU = -1;
    do {
        printf("Escolha qual escalonador usar:\n");
        printf("0 - Escalonador MLFQ:\n");
        printf("1 - Escalonador FIFO:\n");
        scanf(" %c", &op);
        if (op != '1' && op != '0') {
            printf("Opcao invalida! Tente novamente.\n");
        }
    } while (op != '1' && op != '0');

    if (op == '1') {
        aux->tipoEscalonador = FIFO;
    } else {
        aux->tipoEscalonador = MLFQ;
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
        aux->qtdCPU = 0;
    }else if (op2 == '2'){
        aux->qtdCPU = 1;
    }else{
        aux->qtdCPU = 3;
    }
    return;
}

int lerOpcaoImpressao(int nCPUs)
{
    char opcao = ' ';
    int opcaoEscolhida = -1;

    printf(AZUL "----------------------------------------------------------\n" RESET);
    printf(AZUL "Bem vindo ao menu de impressao do seu sistema operacional!\n" RESET);
    printf(AZUL "----------------------------------------------------------\n" RESET);
    int flag = 0;
    do
    {
        flag = 0;
        printf(AZUL "O que deseja visualizar?\n" RESET);
        printf(AZUL "1-" BRANCO " Todos os processos\n" RESET);
        printf(AZUL "2-" BRANCO " Processos em execucao\n" RESET);
        printf(AZUL "3-" BRANCO " Processos prontos para executar\n" RESET);
        printf(AZUL "4-" BRANCO " Processos bloqueados\n" RESET);
        printf(AZUL "5-" BRANCO " Informacoes de todas as CPUs\n" RESET);
        for(int i = 0;i<nCPUs;i++){

            printf("%s%d-%s Informacoes CPU numero %d%s\n", AZUL, 6+i, BRANCO, i+1, RESET);

        }
        printf(AZUL "Opcao: " RESET);
        scanf(" %c", &opcao);

        switch (nCPUs)
        {
        case 1:
            if (opcao != '1' && opcao != '2' && opcao != '3' && opcao != '4' && opcao != '5' && opcao != '6')
                {
                    printf(VERMELHO "Opcao invalida! Tente novamente\n" RESET);
                    flag = 1;
                }
            break;
        case 2:
            if (opcao != '1' && opcao != '2' && opcao != '3' && opcao != '4' && opcao != '5' && opcao != '6' && opcao != '7')
                {
                    printf(VERMELHO "Opcao invalida! Tente novamente\n" RESET);
                    flag = 1;
                }
            break;
        case 4:
            if (opcao != '1' && opcao != '2' && opcao != '3' && opcao != '4' && opcao != '5' && opcao != '6' && opcao != '7' && opcao != '8' && opcao != '9')
                {
                    printf(VERMELHO "Opcao invalida! Tente novamente\n" RESET);
                    flag = 1;
                }
            break;
        default:
            break;
        }


    } while (flag == 1);
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
    case '6':
        opcaoEscolhida = 6;
        break;
    case '7':
        opcaoEscolhida = 7;
        break;
    case '8':
        opcaoEscolhida = 8;
        break;
    case '9':
        opcaoEscolhida = 9;
        break;
    default:
        break;
    }

    return opcaoEscolhida;
}