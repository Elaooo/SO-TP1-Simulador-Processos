#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../include/GerenciadorProcesso.h"
#include "../include/LeituraArquivo.h"
#include "../include/LeituraArquivo.h"
#include "../include/Globais.h"
#include "../include/ProcessoImpressao.h"
#include "../include/ProcessoControle.h"
#include "../include/Escalonador.h"


void* rodarGerenciador(void* arg)
{   
    // extrai a flag do escalonador passada pela thread principal
    int escFlag = *(int*)arg; 

    Comando msg;
    TItem novoItem;

    GerenciadorProcesso gp;
    inicializaGerenciadorProcessos(&gp);
    processo init;
    leituraProcessoInit(&init);

    inserirProcessoTabela(&gp.tabelaProcessos,&init);
    novoItem.Chave=init.pid;
    
    if (escFlag == MLFQ){
        FilaEnfileira(&gp.estadoPronto[init.prioridade],&novoItem);//escalonador MLFQ
    }
    else if (escFlag == FIFO){
        FilaEnfileira(&gp.estadoPronto[0], &novoItem);
    }

    //init.quantum = quantumPorPrioridade[init.prioridade];
    init.quantum=15;

    printf("[Gerenciador] Iniciado como Thread. A aguardar comandos (U, I, M)...\n");
    
    //loop infinito baseado nas variáveis de condição do pthreads
    while (1)
    {
        // tranca para ler a memória compartilhada de forma segura
        pthread_mutex_lock(&mutex_comando);
        
        // espera até que haja um comando novo
        while (tem_novo_comando == 0) {
            pthread_cond_wait(&cond_comando, &mutex_comando);
        }
        
        msg = msg_compartilhada;
        tem_novo_comando = 0;
        
        //libera
        pthread_mutex_unlock(&mutex_comando);

        if (msg.tipo == 'U')
        {
            //decrementa tempo dos processos bloqueados e move para pronto se zerar
            atualizarProcessosBloqueados(&gp, escFlag);

            // escalonamento
            if (!gp.cpu.emUso)
            {
                if (escFlag == MLFQ)
                {
                    int pidEscalonado = escalonadorMLFQ(&gp);
                    if (pidEscalonado != -1)
                    {
                        gp.cpu.processo_atual = buscarProcessoTabela(&gp.tabelaProcessos, pidEscalonado);
                        AtualizarRegistradorCPU(&gp.cpu, gp.cpu.processo_atual);
                        printf("[Gerenciador] Processo %d escalonado para execução.\n", pidEscalonado);
                    }
                }
                else
                {
                    int pidEscalonado = escalonadorFIFO(&gp);
                    if (pidEscalonado != -1)
                    {
                        gp.cpu.processo_atual = buscarProcessoTabela(&gp.tabelaProcessos, pidEscalonado);
                        AtualizarRegistradorCPU(&gp.cpu, gp.cpu.processo_atual);
                        printf("[Gerenciador] Processo %d escalonado para execução.\n", pidEscalonado);
                    }
                }
            }else if(gp.cpu.emUso)
            {
            // troca de contexto
                if (gp.cpu.quantum_usado >= gp.cpu.quantum_total)
                {
                    processo *procAtual = gp.cpu.processo_atual;
                    SalvarContextoCpu(&gp.cpu); // Nota: Confirme se essa função está no seu TAD_CPU

                    TItem novoItem;
                    novoItem.Chave = procAtual->pid;

                    if (escFlag == MLFQ) {
                        FilaEnfileira(&gp.estadoPronto[procAtual->prioridade], &novoItem);
                    } else if (escFlag == FIFO) {
                        FilaEnfileira(&gp.estadoPronto[0], &novoItem);
                    }
                }
            }
            
            // execução
            if(gp.cpu.emUso)
            {
                printf("--- Executando %c...\n",gp.cpu.listaInstrucao[gp.cpu.registradorPC].tipo);
                executaInstrucoes(&gp.cpu);
                IncrementarQuantum_usado(&gp.cpu);

                TItem novoItem;

                if (gp.cpu.listaInstrucao[gp.cpu.registradorPC].tipo == 'B')
                {
                    novoItem.Chave = gp.cpu.processo_atual->pid;
                    SalvarContextoCpu(&gp.cpu); 

                    FilaEnfileira(&gp.estadoBloquado, &novoItem);
                    imprimirProcesso(buscarProcessoTabela(&gp.tabelaProcessos,novoItem.Chave));
                }
                else if (gp.cpu.listaInstrucao[gp.cpu.registradorPC].tipo == 'T')
                {
                    novoItem.Chave = gp.cpu.processo_atual->pid;
                    SalvarContextoCpu(&gp.cpu);

                    FilaEnfileira(&gp.finalizados, &novoItem);
                    imprimirProcesso(buscarProcessoTabela(&gp.tabelaProcessos,novoItem.Chave));
                }
                else if (gp.cpu.listaInstrucao[gp.cpu.registradorPC].tipo == 'F')
                {
                    processo *processoFilhinho = clonaProcesso(&gp.cpu);
                    
                    inserirProcessoTabela(&gp.tabelaProcessos, processoFilhinho);
                    novoItem.Chave = processoFilhinho->pid;
                    
                    if (escFlag == MLFQ){
                        FilaEnfileira(&gp.estadoPronto[processoFilhinho->prioridade],&novoItem);
                    }
                    else if (escFlag == FIFO){
                        FilaEnfileira(&gp.estadoPronto[0], &novoItem);
                    }
                    gp.cpu.registradorPC+=gp.cpu.listaInstrucao[gp.cpu.registradorPC].n;
                }

                gp.cpu.registradorPC++;
            }
            IncrementaTempo(&gp.tempo);
        }
        else if (msg.tipo == 'I')
        {  
            // CRIAR THREAD DE IMPRESSÃO (substitui fork)
            pthread_t t_impressao;
            ArgsImpressao *args = (ArgsImpressao*) malloc(sizeof(ArgsImpressao));
            
            if (args != NULL) {
                args->gerenciador = &gp;
                args->opcao = msg.opcaoImpressao;
                args->escFlag = escFlag;
                
                if (pthread_create(&t_impressao, NULL, rotinaImpressao, args) == 0) {
                    pthread_join(t_impressao, NULL); // espera a impressão acabar
                } else {
                    perror("Erro ao criar thread de impressao");
                    free(args);
                }
            }
        }
        else if (msg.tipo == 'M')
        {
            // CRIAR THREAD DE IMPRESSÃO (substitui fork)
            pthread_t t_impressao;
            ArgsImpressao *args = (ArgsImpressao*) malloc(sizeof(ArgsImpressao));
            
            if (args != NULL) {
                args->gerenciador = &gp;
                args->opcao = msg.opcaoImpressao;
                args->escFlag = escFlag;
                
                if (pthread_create(&t_impressao, NULL, rotinaImpressao, args) == 0) {
                    pthread_join(t_impressao, NULL); // Espera a impressão acabar
                } else {
                    perror("Erro ao criar thread de impressao");
                    free(args);
                }
            }
            
            break; 
        }
        else
        {
            printf("[Gerenciador] Comando ignorado: %c\n", msg.tipo);
        }
    }
    
    return NULL;
}

int inicializaGerenciadorProcessos(GerenciadorProcesso *gerenciadorProcessos)
{

    inicializarTabelaProcessos(&gerenciadorProcessos->tabelaProcessos);
    inicializarCPU(&gerenciadorProcessos->cpu);
    InicializaTempo(&gerenciadorProcessos->tempo);
    for (int i = 0; i < 4; i++)
    {
        FazFilaVazia(&gerenciadorProcessos->estadoPronto[i]);
    }
    FazFilaVazia(&gerenciadorProcessos->estadoEmExecucao);
    FazFilaVazia(&gerenciadorProcessos->estadoBloquado);
    FazFilaVazia(&gerenciadorProcessos->finalizados);
    gerenciadorProcessos->totalProcessosFinalizados = 0;

    if (gerenciadorProcessos == NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

processo *clonaProcesso(cpu_s *cpu)
{

    processo *procFilho = (processo *)malloc(sizeof(processo));
    processo *procPai = cpu->processo_atual;

    procFilho->pid = proximoPidDisponivel;
    proximoPidDisponivel++;

    procFilho->pcCounter = cpu->registradorPC + 1;
    procFilho->estado = PRONTO;

    procFilho->quantum = cpu->processo_atual->quantum;
    procFilho->quantum_usado_CPUatual = 0;

    procFilho->tempoBloqueado = 0;

    procFilho->prioridade = procPai->prioridade;
    
    procFilho->nInstrucoes = procPai->nInstrucoes;
    procFilho->listaInstrucoes = (instrucao *)malloc(sizeof(instrucao) * procFilho->nInstrucoes);

    if (procFilho->listaInstrucoes != NULL)
    {
        for (int i = 0; i < procPai->nInstrucoes; i++)
        {
            procFilho->listaInstrucoes[i] = procPai->listaInstrucoes[i];
        }
    }else{
        printf("Falha ao copiar instruções do processo pai");
    }

    procFilho->nVariaveis = procPai->nVariaveis;
    procFilho->variaveis = (int*) malloc(sizeof(int) * procFilho->nVariaveis);

    if(procFilho->variaveis != NULL){
        for(int j = 0; j < procFilho->nVariaveis; j++){
            procFilho->variaveis[j]=procPai->variaveis[j];
        }
    }else{
        printf("Falha ao copiar variáveis do processo pai");
    }

    return procFilho;
}

void atualizarProcessosBloqueados(GerenciadorProcesso *gp, int escFlag) {
    
    int tamanhoInicial = gp->estadoBloquado.tam; 

    for (int i = 0; i < tamanhoInicial; i++) {
        TItem itemRetirado;
        
        if (FilaDesenfileira(&gp->estadoBloquado, &itemRetirado)) {
            
            // busca os dados do processo na tabela geral
            processo *proc = buscarProcessoTabela(&gp->tabelaProcessos, itemRetirado.Chave);
            
            if (proc != NULL && proc->estado == BLOQUEADO) {
                proc->tempoBloqueado--; 
                
                if (proc->tempoBloqueado <= 0) {
                    
                    proc->estado = PRONTO; // Atualiza o estado
                    
                    TItem novoItem;
                    novoItem.Chave = proc->pid;
                    
                    // verifica escalonador
                    if (escFlag == MLFQ) {

                        FilaEnfileira(&gp->estadoPronto[proc->prioridade], &novoItem);
                        printf("[Gerenciador] Processo %d desbloqueado. Inserido na fila Prontos (Prioridade %d).\n", proc->pid, proc->prioridade);
                    } 
                    else {
                        // no fifo n precisa preocupar com prioridade
                        FilaEnfileira(&gp->estadoPronto[0], &novoItem);
                        printf("[Gerenciador] Processo %d desbloqueado. Inserido na fila Prontos.\n", proc->pid);
                    }
                } 
                else {
                    // se ainda n zerou, o processo continua bloqueado
                    // é inserido novamente no fim da fila de bloqueados
                    FilaEnfileira(&gp->estadoBloquado, &itemRetirado);
                }
            }
        }
    }
}

void* rotinaImpressao(void* arg) {
    ArgsImpressao *args = (ArgsImpressao*) arg;
    Imprime(args->gerenciador, args->opcao, args->escFlag);
    free(args);
    return NULL;
}
