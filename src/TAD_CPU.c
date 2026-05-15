#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/TAD_CPU.h"
#include "../include/TAD_LeituraArquivo.h"
#include "../include/globais.h"

void inicializarCPU(cpu_s *cpu){
    cpu->processo_atual = NULL; // Inicializa o ponteiro para o processo atual como NULL
    cpu->registradorPC = 0; // Inicializa o PC
    cpu->quantum_total = 0; // Inicializa o quantum total alocado
    cpu->quantum_usado = 0; // Inicializa o tempo executado neste quantum
    cpu->variaveis = (int*) malloc(sizeof(int) * (31)); // a cpu deve ter uma memoria de variaveis propria
    cpu->emUso = 0;
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
    printf("");
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
    cpu->emUso=1;
    cpu->processo_atual = proc; // Atualiza o processo atual
    cpu->registradorPC = proc->pcCounter; // Atualiza o PC com o valor do processo
    cpu->quantum_total = quantum; // Atualiza o quantum total alocado
    cpu->quantum_usado = 0; // Reinicia o tempo executado neste quantum
    cpu->listaInstrucao = proc->listaInstrucoes;
    
    for(int i = 0; i<proc->nVariaveis; i++){
        if (proc->variaveis[i] != NULL){
            cpu->variaveis[i] = proc->variaveis[i];
        }
    }
}

void SalvarContextoCPU(cpu_s *cpu){

    if(cpu->processo_atual == NULL){
        printf("Erro:Tentativa de salvar contexto com processo nulo\n");
        return;
    }
    cpu->processo_atual->pcCounter = cpu->registradorPC; // Salva o PC do processo atual
    cpu->processo_atual->quantum_usado_CPUatual = cpu->quantum_usado; // Salva o tempo usado no quantum atual
    
    if(cpu->registradorPC >= cpu->processo_atual->nInstrucoes-1){ //Terminou a execução
        cpu->processo_atual->estado = TERMINADO;
    }
    else if(cpu->quantum_usado<cpu->quantum_total){ //foi bloqueado por uma instrucao B
        cpu->processo_atual->estado = BLOQUEADO;
    }
    else{
        cpu->processo_atual->estado = PRONTO; // Atualiza o estado do processo para pronto para reinserção na fila
    }

    cpu->emUso=0; //indica que a CPU está livre

    for(int i = 0; i<cpu->processo_atual->nVariaveis; i++){
        if (cpu->variaveis[i] != NULL){
            cpu->processo_atual->variaveis[i] = cpu->variaveis[i]; 
        }
    }

    // Adiciona o processo na fila de prontos (Não sei se isso é feito na cpu ou no gerenciador, verificar isso)
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
            SalvarContextoCPU(cpu);
            printf("Processo bloqueado. CPU disponivell\n");
            break;
        case 'R':
            leituraArquivoProcesso(instrucaoAtual.caminhoArquivo, cpu);
            printf("Leu o arquivo %s e iniciou o processo\n", instrucaoAtual.caminhoArquivo);
            break;

        case 'F':{
            // processo* processoFilhinho = clonaProcesso(cpu); // lembrar que o f pula o pcCounter = pcCounter + n + 1
            // imprimirProcesso(processoFilhinho);
            // printf("instrucao F\n");
            cpu->registradorPC++;
            break;
        }
        case 'T':
            SalvarContextoCPU(cpu);
            printf("FIM DO PROCESSO");
            break;

        default:
            printf("Comando desconhecido: %c\n\n", comando);
            break;
        }
        
    cpu->registradorPC++;
}

processo* clonaProcesso(cpu_s *cpu){

    processo *procFilho = (processo*) malloc(sizeof(processo));
    processo *procPai = cpu->processo_atual;

    procFilho->pid = proximoPidDisponivel;
    proximoPidDisponivel++;
    procFilho->nInstrucoes=procPai->nInstrucoes;

    procFilho->pcCounter=procPai->pcCounter+1;
    procFilho->estado=PRONTO;

    procFilho->quantum=0;
    procFilho->quantum_usado_CPUatual=0;

    procFilho->tempoBloqueado=0;

        
    procFilho->listaInstrucoes = (instrucao*) malloc(sizeof(instrucao) * procFilho->nInstrucoes);
    if (procFilho->listaInstrucoes != NULL) {
        for (int i = 0; i < procPai->nInstrucoes; i++) {
            procFilho->listaInstrucoes[i] = procPai->listaInstrucoes[i];
        }
    }

    return procFilho;
}
