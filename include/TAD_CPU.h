#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TAD_processo_simulado.h"
#include "TAD_LeituraArquivo.h"
#ifndef TAD_CPU_H
#define TAD_CPU_H

//representa nossa CPU
typedef struct{
//seria bom eu ter um ponteiro para as instruçoes dentro do processo?
//e sobre esse vetor de memoria? seria onde esta os processos?
    //Ponteiro para o processo simulado atualmente na CPU.
    processo *processo_atual;

    instrucao *listaInstrucao;
    int *variaveis;
    
    //Índice da próxima instrução a ser executada do processso simulado.
    int registradorPC;

    // Quantum (fatia de tempo) total que foi alocado para o processo atual na CPU.
    int quantum_total;

    // Tempo de CPU já consumido pelo processo dentro do quantum_total.
    // Incrementado a cada unidade de tempo 'U' que o processo executa.
    int quantum_usado;

}cpu_s;

void inicializarCPU(cpu_s* cpu);
//atualiza a cpu para os dados do processo em execução atual
void AtualizarRegistradorCPU(cpu_s*cpu,processo*proc, int quantum);
void IncrementarQuantum_usado(cpu_s *cpu);
// Salva o contexto do processo atual da CPU.
void SalvarContextoCPU(cpu_s *cpu);

void executaInstrucoes(cpu_s *cpu);

#endif