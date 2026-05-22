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
    ArgsControle *args = (ArgsControle *) args;
    int argc = args->argc;
    char *argv = args->argv;

    int op = -999;
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
        if (comando == 'I' || comando == 'M'){
            msg.opcaoImpressao = lerOpcaoImpressao();
        }
        pthread_mutex_lock(&filaComandos.mutex);

        filaComandos.fila[filaComandos.fim] = msg;
        filaComandos.fim++;

        pthread_cond_signal(&filaComandos.cond);

        pthread_mutex_unlock(&filaComandos.mutex);
        
        if (comando == 'M'){
            break;
        }
        if (entrada != stdin)
        {
            fclose(entrada);
        }

        
    }
    printf("[Controle] Simulação finalizada.\n");
    return NULL;
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