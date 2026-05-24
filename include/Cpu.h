#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ProcessoSimulado.h"
#include "LeituraArquivo.h"
#ifndef CPU_H
#define CPU_H
#define MAX_VARIAVEIS_CPU 31

typedef struct{

    //Indica se a CPU está ou não sendo usada no momento
    int emUso;

    //Ponteiro para o processo simulado atualmente na CPU.
    processo *processo_atual;

    //Ponteiro para a lista de instruções do processo atual
    instrucao *listaInstrucao;

    //A CPU contém 31 registradores.
    int variaveis[MAX_VARIAVEIS_CPU];
    
    //Índice da próxima instrução a ser executada do processso simulado.
    int registradorPC;

    // Quantum (fatia de tempo) total que foi alocado para o processo atual na CPU.
    int quantum_total;

    // Tempo de CPU já consumido pelo processo dentro do quantum_total.
    // Incrementado a cada unidade de tempo 'U' que o processo executa.
    int quantum_usado;

}cpu_s;

void inicializarCPU(cpu_s* cpu);                       //Inicialização da estrutura
int AtualizarRegistradorCPU(cpu_s*cpu,processo*proc);  //Atualiza a cpu com os dados do processo recém chegado
void IncrementarQuantum_usado(cpu_s *cpu);             //Contabiliza a utilização de quantum
void imprimirCPU(cpu_s *cpu);                          //Imprime o estado atual de todos atributos
void executaInstrucoes(cpu_s *cpu);                    //Executa a instrução atual
void SalvarContextoCpu(cpu_s *cpu);                    //Salva contexto da cpu em caso de bloqueio e término
void quantumEsgotado(cpu_s *cpu);                      //Salva contexto da cpu em caso de quantum total utilizado
void esvaziaCpu(cpu_s *cpu);                           //Retira processo da CPU e indica que ela está livre

#endif

