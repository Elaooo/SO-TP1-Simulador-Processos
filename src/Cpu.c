#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/Cpu.h"
#include "../include/LeituraArquivo.h"
#include "../include/Globais.h"


void inicializarCPU(cpu_s *cpu)
{
    //Inicialização das estruturas
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

    for(int i=0;i<cpu->processo_atual->nVariaveis;i++){
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

//Copia as informações do processo recém chegado para a CPU
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
    cpu->listaInstrucao = proc->listaInstrucoes; //Recebe a lista de instruções

    if(proc->nVariaveis>31){
        printf("Número inválido de variaveis. Máximo=31.\n");
    }
    else{
        for(int i = 0; i<proc->nVariaveis; i++){

            cpu->variaveis[i] = proc->variaveis[i];  //Recebe a lista de variáveis
        }
    }
    return 1;
}

//É usada para a troca de contexto, salvando as infos de volta ao processo quando ele deixa a CPU em caso de bloqueio e término
void SalvarContextoCpu(cpu_s *cpu){

    if(cpu->processo_atual == NULL || cpu == NULL){
        printf("Processo nulo OR cpu nula\n");
        return;
    }

    processo* p = cpu->processo_atual;
    
    instrucao atual = p->listaInstrucoes[cpu->registradorPC];

    p->pcCounter = cpu->registradorPC + 1; // Salva o PC do processo atual
    p->quantum_usado_CPUatual = cpu->quantum_usado; // Salva o tempo usado no quantum atual
    
    //Define o nove estado
    if(atual.tipo == 'B'){
        p->estado = BLOQUEADO;
    }
    if(atual.tipo == 'T'){
        p->estado = TERMINADO;
    }

    //Guarda as variáveis atualizadas
    for(int i = 0; i<cpu->processo_atual->nVariaveis; i++){
        p->variaveis[i] = cpu->variaveis[i]; 
    }

    esvaziaCpu(cpu); //indica que a CPU está livre

}

//É usada para a troca de contexto, salvando as infos de volta ao processo quando ele deixa a CPU em caso de quantum esgotado
void quantumEsgotado(cpu_s *cpu){

    if(cpu == NULL || cpu->processo_atual == NULL){
        printf("Processo nulo OR cpu nula\n");
        return;
    }

    processo* p = cpu->processo_atual;
    
    p->pcCounter = cpu->registradorPC; // Salva o PC do processo atual
    p->quantum_usado_CPUatual = cpu->quantum_usado; // Salva o tempo usado no quantum atual
    
    if(cpu->quantum_usado>=cpu->quantum_total){
        p->estado = PRONTO; // Atualiza o estado do processo para pronto para reinserção na fila
    }

    //Guarda as variáveis atualizadas
    for(int i = 0; i<cpu->processo_atual->nVariaveis; i++){
        p->variaveis[i] = cpu->variaveis[i]; 
    }

    esvaziaCpu(cpu);
}


void executaInstrucoes(cpu_s* cpu){

    instrucao instrucaoAtual = cpu->listaInstrucao[cpu->registradorPC];
    char comando = instrucaoAtual.tipo;
    int pid = cpu->processo_atual->pid;
    
    switch (comando) {

        case 'N': //A instrução N define a quantidade de variáveis que o processo possui

            printf("[Processo %d]--- %d variáveis definidas.\n",pid, instrucaoAtual.n);
            cpu->processo_atual->nVariaveis=instrucaoAtual.n;
            break;

        case 'D': //A instrução D define o valor da variável indicada como zero

            cpu->variaveis[instrucaoAtual.x] = 0;
            printf("[Processo %d]--- Registrador (%d) definido para (0).\n",pid, instrucaoAtual.x);
            break;

        case 'V': //A instrução V atribui um valor determinado à uma variável indicada

            cpu->variaveis[instrucaoAtual.x]=instrucaoAtual.n;
            printf("[Processo %d]--- Registrador %d definido para %d.\n",pid, instrucaoAtual.x, instrucaoAtual.n);
            break;

        case 'A': //A instrução A soma um valor determinado à uma variável indicada

            cpu->variaveis[instrucaoAtual.x] = cpu->variaveis[instrucaoAtual.x] + instrucaoAtual.n;
            printf("[Processo %d]--- Somou %d ao registrador %d.\n",pid, instrucaoAtual.n,instrucaoAtual.x);
            break;

        case 'S': //A instrução A subtrai um valor determinado à uma variável indicada

            cpu->variaveis[instrucaoAtual.x] = cpu->variaveis[instrucaoAtual.x] - instrucaoAtual.n;
            printf("[Processo %d]--- Subtraiu %d no registrador %d.\n",pid, instrucaoAtual.n,instrucaoAtual.x);
            break;

        case 'B': //A instrução B bloqueia o processo por uma determinda quantidade de tempo 
                  //e indica ao gerenciador a necessidade de troca de contexto

            cpu->processo_atual->tempoBloqueado=cpu->listaInstrucao[cpu->registradorPC].n;
            printf("[Processo %d]--- Bloqueado por %d unidades de tempo. CPU disponivel.\n",pid, instrucaoAtual.n);
            break;

        case 'R': //A instrução R  substitui as infos do proc simulado pelas informações do arquivo informado e pccounter é reiniciado

            printf("[Processo %d] --- Leu o arquivo %s e iniciou.\n", pid, instrucaoAtual.caminhoArquivo);
            leituraArquivoProcesso(instrucaoAtual.caminhoArquivo, cpu);
            cpu->listaInstrucao=cpu->processo_atual->listaInstrucoes;
            cpu->registradorPC=-1;
            break;

        case 'F': //A instrução F indica ao gerenciador a necessidade de criar um processo filho
                  // e salta o contador de programa em n posições
            
            printf("[Processo %d] --- Criação de processo filho.\n",pid);
            cpu->registradorPC+=instrucaoAtual.n;

            break;

        case 'T': //A instrução T indica ao gerenciador que o precesso terminou sua execução

            printf("\n--- O processo finalizou sua execução.\n");
            break;

        default:
            printf("Comando desconhecido: %c\n\n", comando);
            break;
        }
}


//Retira o processo e indica que esta livre para uso
void esvaziaCpu(cpu_s *cpu){
    cpu->processo_atual = NULL; 
    cpu->emUso = 0;
}