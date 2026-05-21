#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/Cpu.h"
#include "../include/LeituraArquivo.h"
#include "../include/Globais.h"


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

    for (int i = 0; i < MAX_VARIAVEIS_CPU; i++) {
        cpu->variaveis[i] = 0;
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

    for(int i=0;i<3;i++){
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
        printf("Processo nulo\n");
        return 0;
    }
    cpu->emUso=1;
    cpu->processo_atual = proc; // Atualiza o processo atual
    cpu->registradorPC = proc->pcCounter; // Atualiza o PC com o valor do processo
    cpu->quantum_total = proc->quantum; // Atualiza o quantum total alocado
    cpu->quantum_usado = 0; // Reinicia o tempo executado neste quantum
    cpu->listaInstrucao = proc->listaInstrucoes;
    // printf("IMPRESSAO lista da cpu:\n");
    // imprimirInstrucoes(cpu->listaInstrucao, cpu->processo_atual->nInstrucoes);
    // printf("IMPRESSAO LSITA DO PROCESSO:\n");
    // imprimirInstrucoes(proc->listaInstrucoes, proc->nInstrucoes);

    for(int i = 0; i<proc->nVariaveis; i++){

        cpu->variaveis[i] = proc->variaveis[i];
        
    }
    return 1;
}

void SalvarContextoCpu(cpu_s *cpu){

    if(cpu->processo_atual == NULL || cpu == NULL){
        printf("Processo nulo OR cpu nula\n");
        return;
    }

    processo* p = cpu->processo_atual;
    
    instrucao atual = p->listaInstrucoes[cpu->registradorPC];
    

    p->pcCounter = cpu->registradorPC + 1; // Salva o PC do processo atual
    p->quantum_usado_CPUatual = cpu->quantum_usado; // Salva o tempo usado no quantum atual
    
    if(cpu->quantum_usado>=cpu->quantum_total){
        p->estado = PRONTO; // Atualiza o estado do processo para pronto para reinserção na fila
    }
    else if(atual.tipo == 'B'){
        p->tempoBloqueado=cpu->listaInstrucao[cpu->registradorPC].n;
        p->estado = BLOQUEADO;
    }
    else if(atual.tipo == 'T'){
        p->estado = TERMINADO;
    }

    for(int i = 0; i<cpu->processo_atual->nVariaveis; i++){
        p->variaveis[i] = cpu->variaveis[i]; 
    }

    esvaziaCpu(cpu); //indica que a CPU está livre

}


void executaInstrucoes(cpu_s* cpu){

    instrucao instrucaoAtual = cpu->listaInstrucao[cpu->registradorPC];
    char comando = instrucaoAtual.tipo;
    
    switch (comando) {

        case 'N':
            printf("--- %d variáveis definidas para o processo.\n",instrucaoAtual.n);
            cpu->processo_atual->nVariaveis=instrucaoAtual.n;
            break;
        case 'D':
            cpu->variaveis[instrucaoAtual.x] = 0;
            printf("--- Definiu o registrador (%d) para (0).\n", instrucaoAtual.x);
            break;
        case 'V':
            cpu->variaveis[instrucaoAtual.x]=instrucaoAtual.n;
            printf("--- Definiu registrador %d para %d.\n",instrucaoAtual.x, instrucaoAtual.n);
            break;
        case 'A':
            cpu->variaveis[instrucaoAtual.x] = cpu->variaveis[instrucaoAtual.x] + instrucaoAtual.n;
            printf("--- Somou %d no registrador %d.\n",instrucaoAtual.n,instrucaoAtual.x);
            break;
        case 'S':
            cpu->variaveis[instrucaoAtual.x] = cpu->variaveis[instrucaoAtual.x] - instrucaoAtual.n;
            printf("--- Subtraiu %d no registrador %d.\n",instrucaoAtual.n,instrucaoAtual.x);
            break;
        case 'B':
            printf("--- Processo bloqueado por %d unidades de tempo. CPU disponivel.\n",instrucaoAtual.n);
            break;
        case 'R':
            printf("--- Leu o arquivo %s e iniciou o processo.\n", instrucaoAtual.caminhoArquivo);
            leituraArquivoProcesso(instrucaoAtual.caminhoArquivo, cpu);
            cpu->registradorPC=-1;
            break;

        case 'F':
            // processo* processoFilhinho = clonaProcesso(cpu); // lembrar que o f pula o pcCounter = pcCounter + n + 1
            // imprimirProcesso(processoFilhinho);
            printf("--- Criação de processo filho.\n");
            
            //cpu->registradorPC += instrucaoAtual.n;
            break;
        case 'T':
            printf("\n--- O processo finalizou sua execução.\n");
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