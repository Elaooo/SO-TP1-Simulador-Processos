#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/TAD_CPU.h"

void inicializarCPU(cpu_s *cpu){
    cpu->processo_atual = NULL; // Inicializa o ponteiro para o processo atual como NULL
    cpu->registradorPC = 0; // Inicializa o PC
    cpu->quantum_total = 0; // Inicializa o quantum total alocado
    cpu->quantum_usado = 0; // Inicializa o tempo executado neste quantum
    cpu->variaveis = (int*) malloc(sizeof(int) * (cpu->processo_atual->nVariaveis));
}

void IncrementarQuantum_usado(cpu_s *cpu){
    if(cpu->processo_atual == NULL){
        printf("Erro:Tentativa de incrementar tempo executado falhou.\n");
        return;
    }
    cpu->quantum_usado++; // Incrementa o tempo executado neste quantum
}

void AtualizarRegistradorCPU(cpu_s*cpu,processo* proc, int quantum){
    if(proc == NULL){
        printf("Erro:Tentativa de atualizar registradores com processo nulo\n");
        return;
    }
    cpu->processo_atual = proc; // Atualiza o processo atual
    cpu->registradorPC = proc->pcCounter; // Atualiza o PC com o valor do processo
    cpu->quantum_total = quantum; // Atualiza o quantum total alocado
    cpu->quantum_usado = 0; // Reinicia o tempo executado neste quantum
    cpu->listaInstrucao = proc->listaInstrucoes;
    
    for(int i = 0; i<proc->nVariaveis; i++){
        cpu->variaveis[i] = proc->variaveis[i];
    }
}

void SalvarContextoCPU(cpu_s *cpu){
    if(cpu->processo_atual == NULL){
        fprintf(stderr,"Erro:Tentativa de salvar contexto com processo nulo\n");
        return;
    }
    cpu->processo_atual->pcCounter = cpu->registradorPC; // Salva o PC do processo atual
    cpu->processo_atual->quantum_usado_CPUatual = cpu->quantum_usado; // Salva o tempo usado no quantum atual
    
    if(cpu->quantum_usado<cpu->processo_atual->quantum){ //foi bloqueado por uma instrucao B
        cpu->
    }
    else{
        cpu->processo_atual->estado = PRONTO; // Atualiza o estado do processo para pronto para reinserção na fila
    }
    // Adiciona o processo na fila de prontos (Não sei se isso é feito na cpu ou no gerenciador, verificar isso)
}

void executaInstrucoes(cpu_s* cpu){

    
    instrucao instrucaoAtual = cpu->listaInstrucao[cpu->registradorPC];
    char comando = instrucaoAtual.tipo;
    
    switch (comando) {

        case 'V':
            cpu->variaveis[instrucaoAtual.x]=instrucaoAtual.n;
            break;

        case 'A':
            cpu->variaveis[instrucaoAtual.x] = cpu->variaveis[instrucaoAtual.x] + instrucaoAtual.n;
            break;
        case 'S':
            cpu->variaveis[instrucaoAtual.x] = cpu->variaveis[instrucaoAtual.x] - instrucaoAtual.n;
            break;
        case 'B':
            //o processo passara para o estado bloqueado, logo, os dados da cpu serao guardados de volta do processo
            // e consequentemente na tabela de processos, e a cpu sera liberada 
            break;
        case 'R':
            leituraArquivoProcesso(instrucaoAtual.caminhoArquivo);

            break;

        case 'F':
            sscanf(linha, " %c %d", &comando, &x);
            printf("Comando F\n");
            printf("X: %d\n\n", x);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            break;
        case 'T':
            sscanf(linha, " %c", &comando);
            printf("Comando %c\n", comando);
            listaInstrucoes[iterador].tipo = comando;
            printf("\n");
            printf("-----encerra execucao-----");
            break;

        default:
            printf("Comando desconhecido: %c\n\n", comando);
            break;
    }
}


