#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "../include/ProcessoControle.h"
#include "../include/GerenciadorProcesso.h"
#include "../include/FilaComandos.h"
#include "../include/Globais.h"


int main(int argc, char *argv[]) {

    printf("\n===== Simulador de Gerenciamento de Processos com threads =====\n\n");

    int escFlag = escolherEscalonador(); // escolhe antes de tudo

    pthread_t tidControle;
    pthread_t tidGerenciador;

    
    ArgsControle *args = malloc(sizeof(ArgsControle) + argc * sizeof(char *));

    args->argc = argc;
    for (int i = 0; i < argc; i++) {
    args->argv[i] = argv[i];
    }

    inicializaFilaComandos(&filaComandos);
    /*
        Cria thread controle
    */
    if (pthread_create(&tidControle, NULL, threadControle, args) != 0) {
        perror("Erro ao criar thread controle");
        exit(1);
    }

    /*
        Cria thread gerenciador
    */
    if (pthread_create(&tidGerenciador, NULL, rodarGerenciador, &escFlag) != 0) {
        perror("Erro ao criar thread gerenciador");
        exit(1);
    }

    /*
        Espera ambas terminarem
    */
    pthread_join(tidControle, NULL);
    pthread_join(tidGerenciador, NULL);

    printf("Simulacao finalizada.\n");

    return 0;
}



// int main(int argc, char *argv[]){

//     inicializaProcessoControle(argc, argv);

    

//     char caminho[256] = "file_a.txt";
//     leituraProcessoInit();
//     leituraArquivoProcesso(caminho);
   
// // ===== Criando dados iniciais do processo 1 =====
//     int variaveisIniciais[3] = {10, 20, 30};

//     instrucao lista[2];

//     lista[0].tipo = 'A';
//     lista[0].x = 1;
//     lista[0].n = 5;

//     lista[1].tipo = 'B';
//     lista[1].x = 2;
//     lista[1].n = 3;

//     processo p1;

//     inicializarProcesso(&p1, 1, 3, 2, variaveisIniciais, 1, lista);

//     printf("Processo 1 criado:\n");
//     printf("PID: %d\n", p1.pid);
//     printf("PC inicial: %d\n", p1.pcCounter);

//     // ===== Criando CPU =====
//     cpu_s cpu;
//     inicializarCPU(&cpu);

//     // ===== Colocando processo 1 na CPU =====
//     AtualizarRegistradorCPU(&cpu, &p1, 5);

//     printf("\nProcesso 1 colocado na CPU\n");
//     printf("Quantum total: %d\n", cpu.quantum_total);

//     // ===== Simulando execução do processo 1 =====
//     IncrementarQuantum_usado(&cpu);
//     IncrementarQuantum_usado(&cpu);

//     printf("Quantum usado na CPU (P1): %d\n", cpu.quantum_usado);

//     // Simulando avanço do PC
//     cpu.registradorPC = 1;

//     // ===== Salvando contexto do processo 1 =====
//     SalvarContextoCPU(&cpu);

//     printf("\nContexto salvo no processo 1\n");
//     printf("Novo PC do processo 1: %d\n", p1.pcCounter);

//     // ======================================================
//     // ===== Criando Processo 2 =============================
//     // ======================================================

//     int variaveisIniciais2[3] = {100, 200, 300};

//     processo p2;

//     inicializarProcesso(&p2, 2, 3, 2, variaveisIniciais2, 2, lista);

//     printf("\nProcesso 2 criado:\n");
//     printf("PID: %d\n", p2.pid);

//     // ===== Colocando processo 2 na CPU (troca de contexto) =====
//     AtualizarRegistradorCPU(&cpu, &p2, 4);

//     printf("\nProcesso 2 colocado na CPU\n");
//     printf("Quantum total (P2): %d\n", cpu.quantum_total);
//     printf("Quantum usado na CPU apos troca (deve ser 0): %d\n", cpu.quantum_usado);
/*int main(){

    processo proc;
    cpu_s cpu;
    inicializarCPU(&cpu);
    leituraProcessoInit(&proc);
    printf("%d", proc.pid);
    imprimirInstrucoes(proc.listaInstrucoes,proc.nInstrucoes);

    AtualizarRegistradorCPU(&cpu,&proc);
    
    imprimirCPU(&cpu);
    
    while(cpu.registradorPC<proc.nInstrucoes){
        executaInstrucoes(&cpu);
    }


    imprimirProcesso(&proc);
   

    return 0;
}*/