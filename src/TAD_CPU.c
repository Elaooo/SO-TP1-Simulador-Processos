#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/TAD_CPU.h"
#include "../include/TAD_LeituraArquivo.h"
#include "../include/globais.h"
#define MAX_VARIAVEIS_CPU 31

void inicializarCPU(cpu_s *cpu)
{
    if (cpu == NULL) {
        return;
    }

    cpu->emUso = 0;
    cpu->processo_atual = NULL;
    cpu->listaInstrucao = NULL;
    cpu->registradorPC = 0;
    cpu->quantum_total = 0;
    cpu->quantum_usado = 0;

    cpu->variaveis = malloc(sizeof(int) * MAX_VARIAVEIS_CPU);

    if (cpu->variaveis == NULL) {
        printf("Erro ao alocar variaveis da CPU.\n");
        return;
    }

    for (int i = 0; i < MAX_VARIAVEIS_CPU; i++) {
        cpu->variaveis[i] = 999;
    }
}

void imprimirCPU(cpu_s *cpu){

    if(cpu == NULL){
        printf("CPU inexistente.\n");
        return;
    }

    printf("\n========= ESTADO DA CPU =========\n");

    printf("Em uso: %d\n", cpu->emUso);

    printf("Registrador PC: %d\n", cpu->registradorPC);

    printf("Quantum total: %d\n", cpu->quantum_total);

    printf("Quantum usado: %d\n", cpu->quantum_usado);

    if(cpu->processo_atual != NULL){
        printf("PID processo atual: %d\n",
               cpu->processo_atual->pid);
    }
    else{
        printf("Processo atual: NULL\n");
    }

    printf("=================================\n");

    printf("\n========= REGISTRADORES =========\n");

    for(int i=0;i<31;i++){
        printf("Registrador %d = %d\n",i, cpu->variaveis[i]);
    }
    printf(" ");
}

void IncrementarQuantum_usado(cpu_s *cpu){
    if(cpu->processo_atual == NULL){
        printf("Erro:Tentativa de incrementar tempo executado falhou.\n");
        return;
    }
    cpu->quantum_usado++; // Incrementa o tempo executado neste quantum
}

int AtualizarRegistradorCPU(cpu_s*cpu,processo* proc){
    if(proc == NULL){
        printf("Erro:Tentativa de atualizar registradores com processo nulo\n");
        return 0;
    }
    cpu->emUso=1;
    cpu->processo_atual = proc; // Atualiza o processo atual
    cpu->registradorPC = proc->pcCounter; // Atualiza o PC com o valor do processo
    cpu->quantum_total = proc->quantum; // Atualiza o quantum total alocado
    cpu->quantum_usado = 0; // Reinicia o tempo executado neste quantum
    cpu->listaInstrucao = proc->listaInstrucoes;

    for(int i = 0; i<proc->nVariaveis; i++){

        cpu->variaveis[i] = proc->variaveis[i];
        
    }
    return 1;
}

void SalvarContextoCpuQuantum(cpu_s *cpu){

    if(cpu->processo_atual == NULL){
        printf("Erro:Tentativa de salvar contexto com processo nulo\n");
        return;
    }
    cpu->processo_atual->pcCounter = cpu->registradorPC; // Salva o PC do processo atual
    cpu->processo_atual->quantum_usado_CPUatual = cpu->quantum_usado; // Salva o tempo usado no quantum atual
    
    if(cpu->quantum_usado>=cpu->quantum_total){
        cpu->processo_atual->estado = PRONTO; // Atualiza o estado do processo para pronto para reinserção na fila
    }

    for(int i = 0; i<cpu->processo_atual->nVariaveis; i++){
        cpu->processo_atual->variaveis[i] = cpu->variaveis[i]; 
    }

    esvaziaCpu(cpu); //indica que a CPU está livre

    // Adiciona o processo na fila de prontos (Não sei se isso é feito na cpu ou no gerenciador, verificar isso)
}


void SalvarContextoCpuTermino(cpu_s *cpu){

    if(cpu->processo_atual == NULL){
        printf("Erro:Tentativa de salvar contexto com processo nulo\n");
        return;
    }
    cpu->processo_atual->pcCounter = cpu->registradorPC; // Salva o PC do processo atual
    cpu->processo_atual->quantum_usado_CPUatual = cpu->quantum_usado; // Salva o tempo usado no quantum atual
    
    if(cpu->registradorPC >= cpu->processo_atual->nInstrucoes-1){ //Terminou a execução
        cpu->processo_atual->estado = TERMINADO;
    }

    for(int i = 0; i<cpu->processo_atual->nVariaveis; i++){
        cpu->processo_atual->variaveis[i] = cpu->variaveis[i]; 
    }

    esvaziaCpu(cpu); //indica que a CPU está livre

    // Adiciona o processo na fila de prontos (Não sei se isso é feito na cpu ou no gerenciador, verificar isso)
}



void SalvarContextoCpuBloqueio(cpu_s *cpu){

    if(cpu->processo_atual == NULL){
        printf("Erro:Tentativa de salvar contexto com processo nulo\n");
        return;
    }
    cpu->processo_atual->pcCounter = cpu->registradorPC; // Salva o PC do processo atual
    cpu->processo_atual->quantum_usado_CPUatual = cpu->quantum_usado; // Salva o tempo usado no quantum atual
    
    if(cpu->quantum_usado<cpu->quantum_total){ //foi bloqueado por uma instrucao B
        cpu->processo_atual->tempoBloqueado=cpu->listaInstrucao[cpu->registradorPC].n;
        cpu->processo_atual->estado = BLOQUEADO;
    }
    
    for(int i = 0; i<cpu->processo_atual->nVariaveis; i++){
        cpu->processo_atual->variaveis[i] = cpu->variaveis[i]; 
    }

    esvaziaCpu(cpu); //indica que a CPU está livre
}

void executaInstrucoes(cpu_s* cpu){

    instrucao instrucaoAtual = cpu->listaInstrucao[cpu->registradorPC];
    char comando = instrucaoAtual.tipo;
    
    switch (comando) {

        case 'N':
            printf("instrucao N\n");
            cpu->processo_atual->nVariaveis=instrucaoAtual.n;
            break;
        case 'D':
            cpu->variaveis[instrucaoAtual.x] = 0;
            printf("--- Definiu o registrador (%d) para (0)\n", instrucaoAtual.x);
            break;
        case 'V':
            cpu->variaveis[instrucaoAtual.x]=instrucaoAtual.n;
            printf("Definiu registrador %d para %d\n",instrucaoAtual.x, instrucaoAtual.n);
            break;
        case 'A':
            cpu->variaveis[instrucaoAtual.x] = cpu->variaveis[instrucaoAtual.x] + instrucaoAtual.n;
            printf("somou %d no registrador %d\n",instrucaoAtual.x, instrucaoAtual.n);
            break;
        case 'S':
            cpu->variaveis[instrucaoAtual.x] = cpu->variaveis[instrucaoAtual.x] - instrucaoAtual.n;
            printf("subtraiu %d no registrador %d\n",instrucaoAtual.x, instrucaoAtual.n);
            break;
        case 'B':
            //o processo passara para o estado bloqueado, logo, os dados da cpu serao guardados de volta no processo
            // e consequentemente na tabela de processos, e a cpu sera liberada 
            printf("Processo bloqueado. CPU disponivel\n");
            break;
        case 'R':
            leituraArquivoProcesso(instrucaoAtual.caminhoArquivo, cpu);
            printf("Leu o arquivo %s e iniciou o processo\n", instrucaoAtual.caminhoArquivo);
            break;

        case 'F':
            // processo* processoFilhinho = clonaProcesso(cpu); // lembrar que o f pula o pcCounter = pcCounter + n + 1
            // imprimirProcesso(processoFilhinho);
            // printf("instrucao F\n");
            
            cpu->registradorPC += instrucaoAtual.n;
            break;
        case 'T':
            printf("FIM DO PROCESSO");
            break;

        default:
            printf("Comando desconhecido: %c\n\n", comando);
            break;
        }
    //cpu->registradorPC++;
}



void esvaziaCpu(cpu_s *cpu){
    cpu->processo_atual = NULL; 
    cpu->emUso = 0;
}