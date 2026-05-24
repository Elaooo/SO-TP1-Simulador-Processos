#ifndef GERENCIADORPROCESSO_H
#define GERENCIADORPROCESSO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/Fila.h"
#include "../include/Tempo.h"
#include "../include/Cpu.h"
#include "../include/TabelaProcessos.h"
#include "../include/ProcessoControle.h"
//para o escalonador

//Flag pioraidade
#define FIFO 1
#define MLFQ 0

typedef struct{

    TFila estadoPronto[4];              //Processos em estado pronto, separados por prioridade
                                        //No FIFO usamos somente o estadoPronto[0]

    TFila estadoBloquado;               //Processos em estado bloqueado
    TFila estadoEmExecucao;             //Processos em execução
    TFila finalizados;                  //Processos finalizados
    Tempo tempo;                        // Registro de unidades de tempo
    cpu_s cpu[4];                       // Cpus disponíveis para utilização
    TabelaDeProcessos tabelaProcessos;  //Tabela que registra todos os processo em atividade
    int totalProcessosFinalizados;      //Quantidade de processos finalizados
    int nCPUs;                          // Número de cpus selecionadas para execução.

}GerenciadorProcesso;

//Inicialização de todas estruturas
int inicializaGerenciadorProcessos(GerenciadorProcesso *gerenciadorProcessos, int cpuFlag);

//Coordena a execução das funcionalidades de execução do sistema
void rodarGerenciador(int fd_leitura, int escFlag, int cpuFlag);

//Monitora e atualiza a lista de processos bloqueados a acada unidade de tempo
void atualizarProcessosBloqueados(GerenciadorProcesso *gerenciador, int escFlag);

// Cria um processo filho idêntico ao pai
processo* clonaProcesso(cpu_s *cpu);

//Registra processos filhos na lista de processos prontos
void computaProcessosCriados(GerenciadorProcesso* gp, TFila* processosCriados, int escFlag);




#endif
