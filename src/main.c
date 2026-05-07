#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/TAD_processo_simulado.h"
#include "../include/TAD_LeituraArquivo.h"
#include "../include/TAD_CPU.h"

int main(){

// ===== Criando dados iniciais do processo 1 =====
    int variaveisIniciais[3] = {10, 20, 30};

    instrucao lista[2];

    lista[0].tipo = 'A';
    lista[0].x = 1;
    lista[0].n = 5;

    lista[1].tipo = 'B';
    lista[1].x = 2;
    lista[1].n = 3;

    processo p1;

    inicializarProcesso(&p1, 1, 3, 2, variaveisIniciais, PRONTO, 1, lista);

    printf("Processo 1 criado:\n");
    printf("PID: %d\n", p1.pid);
    printf("PC inicial: %d\n", p1.pcCounter);
    printf("Estado: %d\n", p1.estado);

    // ===== Criando CPU =====
    cpu_s cpu;
    inicializarCPU(&cpu);

    // ===== Colocando processo 1 na CPU =====
    AtualizarRegistradorCPU(&cpu, &p1, 5);

    printf("\nProcesso 1 colocado na CPU\n");
    printf("Quantum total: %d\n", cpu.quantum_total);

    // ===== Simulando execução do processo 1 =====
    IncrementarQuantum_usado(&cpu);
    IncrementarQuantum_usado(&cpu);

    printf("Quantum usado na CPU (P1): %d\n", cpu.quantum_usado);

    // Simulando avanço do PC
    cpu.registradorPC = 1;

    // ===== Salvando contexto do processo 1 =====
    SalvarContextoCPU(&cpu);

    printf("\nContexto salvo no processo 1\n");
    printf("Novo PC do processo 1: %d\n", p1.pcCounter);
    printf("Quantum usado salvo no processo 1: %d\n", p1.quantum_usado_CPUatual);
    printf("Estado atual do processo 1: %d\n", p1.estado);

    // ======================================================
    // ===== Criando Processo 2 =============================
    // ======================================================

    int variaveisIniciais2[3] = {100, 200, 300};

    processo p2;

    inicializarProcesso(&p2, 2, 3, 2, variaveisIniciais2, PRONTO, 2, lista);

    printf("\nProcesso 2 criado:\n");
    printf("PID: %d\n", p2.pid);

    // ===== Colocando processo 2 na CPU (troca de contexto) =====
    AtualizarRegistradorCPU(&cpu, &p2, 4);

    printf("\nProcesso 2 colocado na CPU\n");
    printf("Quantum total (P2): %d\n", cpu.quantum_total);
    printf("Quantum usado na CPU apos troca (deve ser 0): %d\n", cpu.quantum_usado);

    return 0;
}