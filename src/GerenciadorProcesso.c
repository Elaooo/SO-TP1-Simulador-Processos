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
    // extrai as flag do escalonador e multiplas cpu's passada pela thread principal
    ArgsGerenciador *args = (ArgsGerenciador *)arg;
    int cpuFlag = args->qtdCPU;
    int escFlag = args->tipoEscalonador;

    Comando msg;
    TItem novoItem;

    GerenciadorProcesso gp;
    inicializaGerenciadorProcessos(&gp,cpuFlag);
    processo init;
    leituraProcessoInit(&init);
    //printf("PID init: %d, nInstrucoes: %d\n", init.pid, init.nInstrucoes);

    inserirProcessoTabela(&gp.tabelaProcessos,&init);
    novoItem.Chave=init.pid;

    TFila processosCriados;
    FazFilaVazia(&processosCriados);

    if (escFlag == MLFQ){
        FilaEnfileira(&gp.estadoPronto[init.prioridade],&novoItem);//escalonador MLFQ
    }
    else if (escFlag == FIFO){
        FilaEnfileira(&gp.estadoPronto[0], &novoItem);
    }

    //init.quantum = quantumPorPrioridade[init.prioridade];


    printf("[Gerenciador] Iniciado como Thread. A aguardar comandos (U, I, M)...\n");
    
    //loop infinito baseado nas variáveis de condição do pthreads
    while (1)
    {
        //pega um comando da fila
        msg = desenfileiraComando(&filaComandos);

        if (msg.tipo == 'U')
        {
            TItem novoItem;
            //decrementa tempo dos processos bloqueados e move para pronto se zerar
            atualizarProcessosBloqueados(&gp, escFlag);

            // escalonamento
            for (int k=0;k<(cpuFlag+1);k++){
                if (!gp.cpu[k].emUso)
                {
                    if (escFlag == MLFQ)
                    {

                        int pidEscalonado = escalonadorMLFQ(&gp,k);
                        if (pidEscalonado != -1)
                        {
                            gp.cpu[k].processo_atual = buscarProcessoTabela(&gp.tabelaProcessos, pidEscalonado);
                            // gp.indiceEstadoExecucao = 0; // CPU agora tem um processo
                            AtualizarRegistradorCPU(&gp.cpu[k], gp.cpu[k].processo_atual);

                            novoItem.Chave = gp.cpu[k].processo_atual->pid;
                            FilaEnfileira(&gp.estadoEmExecucao,&novoItem);

                            printf("[Gerenciador] Processo %d escalonado para execução na CPU %d.\n", pidEscalonado,k);
                        }
                    }
                    else
                    {

                        int pidEscalonado = escalonadorFIFO(&gp,k);
                        if (pidEscalonado != -1)
                        {
                            gp.cpu[k].processo_atual = buscarProcessoTabela(&gp.tabelaProcessos, pidEscalonado);
                            // gp.indiceEstadoExecucao = 0; // CPU agora tem um processo
                            AtualizarRegistradorCPU(&gp.cpu[k], gp.cpu[k].processo_atual);

                            novoItem.Chave = gp.cpu[k].processo_atual->pid;
                            FilaEnfileira(&gp.estadoEmExecucao,&novoItem);

                            printf("[Gerenciador] Processo %d escalonado para execução na CPU %d.\n", pidEscalonado,k);
                        }
                    }
                }else if(gp.cpu[k].emUso)
                {
                // troca de contexto
                    if (escFlag == MLFQ && gp.cpu[k].quantum_usado >= gp.cpu[k].quantum_total)
                    {
                        processo *procAtual = gp.cpu[k].processo_atual;

                        quantumEsgotado(&gp.cpu[k]);

                        if (escFlag == FIFO) {
                            FilaEnfileira(&gp.estadoPronto[0], &novoItem);
                        }
                        // Remove da fila de em execução
                        FilaRemovePorChave(&gp.estadoEmExecucao, procAtual->pid);

                        // Recalcula prioridade correta, sem esperar o próximo ciclo U.
                        mlfqReinserirProcesso(&gp, procAtual);

                        printf("[CPU %d] Quantum máximo atingido. Troca de contexto.\n", k);
                    }
                }
                // execução
                if(gp.cpu[k].emUso)
                {
                    printf("[CPU %d] --- Executando %c...\n",k,gp.cpu[k].listaInstrucao[gp.cpu[k].registradorPC].tipo);
                    executaInstrucoes(&gp.cpu[k]);
                    IncrementarQuantum_usado(&gp.cpu[k]);

                    if (gp.cpu[k].listaInstrucao[gp.cpu[k].registradorPC].tipo == 'B')
                    {
                        novoItem.Chave = gp.cpu[k].processo_atual->pid;
                        int pidSalvo = novoItem.Chave;
                        SalvarContextoCpu(&gp.cpu[k]);

                        FilaEnfileira(&gp.estadoBloquado, &novoItem);
                        FilaRemovePorChave(&gp.estadoEmExecucao,pidSalvo);

                        imprimirProcesso(buscarProcessoTabela(&gp.tabelaProcessos,pidSalvo));
                    }
                    else if (gp.cpu[k].listaInstrucao[gp.cpu[k].registradorPC].tipo == 'T')
                    {

                        novoItem.Chave = gp.cpu[k].processo_atual->pid;
                        int pidSalvo = novoItem.Chave;
                        SalvarContextoCpu(&gp.cpu[k]);

                        FilaEnfileira(&gp.finalizados, &novoItem);
                        FilaRemovePorChave(&gp.estadoEmExecucao,pidSalvo);

                        imprimirProcesso(buscarProcessoTabela(&gp.tabelaProcessos,pidSalvo));
                        gp.totalProcessosFinalizados++;


                    }
                    else if (gp.cpu[k].listaInstrucao[gp.cpu[k].registradorPC].tipo == 'F')
                    {

                        processo *processoFilhinho = clonaProcesso(&gp.cpu[k]);

                        inserirProcessoTabela(&gp.tabelaProcessos, processoFilhinho);
                        novoItem.Chave = processoFilhinho->pid;

                        FilaEnfileira(&processosCriados,&novoItem);

                        gp.cpu[k].registradorPC+=gp.cpu[k].listaInstrucao[gp.cpu[k].registradorPC].n;
                    }

                    gp.cpu[k].registradorPC++;
                }

            }

            computaProcessosCriados(&gp,&processosCriados,escFlag);
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
int inicializaGerenciadorProcessos(GerenciadorProcesso *gerenciadorProcessos, int cpuFlag)
{

    inicializarTabelaProcessos(&gerenciadorProcessos->tabelaProcessos);
    for(int j=0;j<cpuFlag;j++){
        inicializarCPU(&gerenciadorProcessos->cpu[j]);
    }
    InicializaTempo(&gerenciadorProcessos->tempo);
    for (int i = 0; i < 4; i++)
    {
        FazFilaVazia(&gerenciadorProcessos->estadoPronto[i]);
    }
    FazFilaVazia(&gerenciadorProcessos->estadoEmExecucao);
    FazFilaVazia(&gerenciadorProcessos->estadoBloquado);
    FazFilaVazia(&gerenciadorProcessos->finalizados);
    gerenciadorProcessos->totalProcessosFinalizados = 0;
    gerenciadorProcessos->nCPUs=cpuFlag;

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
    procFilho->pidPai=procPai->pid;

    procFilho->pcCounter = cpu->registradorPC + 1;
    procFilho->estado = PRONTO;

    procFilho->quantum = procPai->quantum;
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

void computaProcessosCriados(GerenciadorProcesso* gp, TFila* processosCriados, int escFlag){
    TItem criado;
    while (!FilaEhVazia(processosCriados)) {

        if (FilaDesenfileira(processosCriados, &criado)) {

            processo* processoFilhinho = buscarProcessoTabela(&gp->tabelaProcessos,criado.Chave);

            if (processoFilhinho != NULL) {

                TItem novoItem;
                novoItem.Chave = processoFilhinho->pid;

                if (escFlag == MLFQ) {
                    FilaEnfileira(&gp->estadoPronto[processoFilhinho->prioridade],&novoItem);
                }
                else if (escFlag == FIFO) {
                    FilaEnfileira(&gp->estadoPronto[0],&novoItem);
                }

                processoFilhinho->estado = PRONTO;
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
