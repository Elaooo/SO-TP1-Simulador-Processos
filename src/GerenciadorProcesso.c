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


void rodarGerenciador(int fd_leitura, int escFlag, int cpuFlag)
{   
    ComandoPipe msg;
    TItem novoItem;
    int bytesLidos;

    //Inicialização do gerenciador
    GerenciadorProcesso gp;
    inicializaGerenciadorProcessos(&gp,cpuFlag+1);

    //Criação do processso Init e inserção na tabela e fila de prontos
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
    ///////////////////////////////////////////////////////////////////////////

    TFila processosCriados;
    FazFilaVazia(&processosCriados);


    //Leitura da operação U,M,I
    printf("[Gerenciador] Iniciado. A aguardar comandos (U, I, M) do pipe...\n");
    while ((bytesLidos = read(fd_leitura, &msg, sizeof(ComandoPipe))) > 0)
    {
        if (msg.tipo == 'U')
        {
            TItem novoItem;
            //decrementa tempo dos processos bloqueados e move para pronto se zerar
            atualizarProcessosBloqueados(&gp, escFlag);

            // Executa a operação U para a quantidade de CPUS selecionada
            for (int k=0;k<(cpuFlag+1);k++){

                //Se a CPU é identificada como vazia, o escalonador escolhido é acionado para trazer um processo, se possível
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
                }
                
                //Se já existe um processo na CPU, é verificado se 
                //o processo possui quantum suficiente para utilizá-la ou realiza a troca de contexto, caso contrário
                else if(gp.cpu[k].emUso)
                {
                    if (escFlag == MLFQ && gp.cpu[k].quantum_usado >= gp.cpu[k].quantum_total)
                    {
                        processo *procAtual = gp.cpu[k].processo_atual;

                        //Salva as infos da cpu de volta ao processo
                        quantumEsgotado(&gp.cpu[k]);

                        // Remove da fila de em execução
                        FilaRemovePorChave(&gp.estadoEmExecucao, procAtual->pid);

                        // Recalcula prioridade correta, sem esperar o próximo ciclo U.
                        mlfqReinserirProcesso(&gp, procAtual);
                        
                        printf("[CPU %d] Quantum máximo atingido. Troca de contexto.\n", k);
                    }
                }
                // Realiza a execução de instruções e as trocas de contexto, se necessário
                if(gp.cpu[k].emUso)
                {
                    printf("[CPU %d] --- Executando %c...\n",k,gp.cpu[k].listaInstrucao[gp.cpu[k].registradorPC].tipo);

                    executaInstrucoes(&gp.cpu[k]);          //Executa a instrução ref ao contador de programa atual
                    IncrementarQuantum_usado(&gp.cpu[k]);   //Contabiliza a utilização de qunatum



                    if (gp.cpu[k].listaInstrucao[gp.cpu[k].registradorPC].tipo == 'B')
                    {
                        novoItem.Chave = gp.cpu[k].processo_atual->pid;
                        int pidSalvo = novoItem.Chave;

                        //Se um instrução B é executada, acontece troca de contexto

                        //As infos da cpu são guardadas no processo
                        //O processo deixa a cpu e a fila de processos em execução 
                        //e é inserido na lista de processos bloqueados
                        SalvarContextoCpu(&gp.cpu[k]);
                        FilaEnfileira(&gp.estadoBloquado, &novoItem);
                        FilaRemovePorChave(&gp.estadoEmExecucao,pidSalvo);

                        imprimirProcesso(buscarProcessoTabela(&gp.tabelaProcessos,pidSalvo));
                    }
                    else if (gp.cpu[k].listaInstrucao[gp.cpu[k].registradorPC].tipo == 'T')
                    {
                        
                        novoItem.Chave = gp.cpu[k].processo_atual->pid;
                        int pidSalvo = novoItem.Chave;

                        //Se um instrução T é executada, o processo termina 

                        //As infos da cpu são guardadas no processo
                        //O processo deixa a cpu e a fila de processos em execução 
                        //e é inserido na lista de processos finalizados
                        SalvarContextoCpu(&gp.cpu[k]);
                        FilaEnfileira(&gp.finalizados, &novoItem);
                        FilaRemovePorChave(&gp.estadoEmExecucao,pidSalvo);

                        imprimirProcesso(buscarProcessoTabela(&gp.tabelaProcessos,pidSalvo));

                        //O processo é removido da tabela de processos pois finalizou a execução
                        removerProcessoTabela(&gp.tabelaProcessos,pidSalvo);
                        gp.totalProcessosFinalizados++;


                    }
                    else if (gp.cpu[k].listaInstrucao[gp.cpu[k].registradorPC].tipo == 'F')
                    {

                        //Se um instrução F é executada, um processo filho idêntico ao pai é criado 
                        //e inserido na tabela de processos

                        processo *processoFilhinho = clonaProcesso(&gp.cpu[k]);
                        
                        inserirProcessoTabela(&gp.tabelaProcessos, processoFilhinho);
                        novoItem.Chave = processoFilhinho->pid;

                        FilaEnfileira(&processosCriados,&novoItem);
                        
                    }

                    gp.cpu[k].registradorPC++;
                }

            }
            //Se processos filhos foram criados nessa unidade de tempo, são inseridos na lista de prontos
            computaProcessosCriados(&gp,&processosCriados,escFlag);
            //Contabiliza a passagem da unidade de tempo
            IncrementaTempo(&gp.tempo);
        }
        //Chama o processo impressão
        else if (msg.tipo == 'I')
        {  
            pid_t pid = fork();
            //Processo filho: Impressao
            if (pid < 0) {
                perror("Erro ao criar processo impressao");
            }
            else if (pid == 0) {
                Imprime(&gp, msg.opcaoImpressao, escFlag);
                _exit(0);
            }
            else {
                waitpid(pid, NULL, 0);
            }
        }
        // Chama o processo impressão e encerra o programa
        else if (msg.tipo == 'M')
        {
            pid_t pid = fork();

            if (pid < 0) {
                perror("Erro ao criar processo impressao");
            }
            else if (pid == 0) {
                Imprime(&gp, msg.opcaoImpressao,escFlag);
                _exit(0);
            }
            else {
                waitpid(pid, NULL, 0);
                close(fd_leitura);
                exit(0);
            }
        }
        else
        {
            printf("[Gerenciador] Comando ignorado: %c\n", msg.tipo);
        }
    }
}

//Inicializa todas estruturas do gerenciador
int inicializaGerenciadorProcessos(GerenciadorProcesso *gerenciadorProcessos,int cpuFlag)
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

//Cria um processo filho idêntido ao pai
processo *clonaProcesso(cpu_s *cpu)
{

    processo *procFilho = (processo *)malloc(sizeof(processo));
    processo *procPai = cpu->processo_atual;

    procFilho->pid = proximoPidDisponivel;          // Recebe um novo ID
    proximoPidDisponivel++;
    procFilho->pidPai=procPai->pid;

    procFilho->pcCounter = cpu->registradorPC + 1;  //Pc counter é definido para a instrução seguinte
    procFilho->estado = PRONTO;

    procFilho->quantum = procPai->quantum;
    procFilho->quantum_usado_CPUatual = 0;          //Reinicia tempo de cpu usado

    procFilho->tempoBloqueado = 0;

    procFilho->prioridade = procPai->prioridade;    //Recebe a mesma prioridade do pai
    
    procFilho->nInstrucoes = procPai->nInstrucoes;
    procFilho->listaInstrucoes = (instrucao *)malloc(sizeof(instrucao) * procFilho->nInstrucoes);

    if (procFilho->listaInstrucoes != NULL)
    {
        for (int i = 0; i < procPai->nInstrucoes; i++)
        {
            procFilho->listaInstrucoes[i] = procPai->listaInstrucoes[i]; //Copia a lista de instruções
        }
    }else{
        printf("Falha ao copiar instruções do processo pai");
    }

    procFilho->nVariaveis = procPai->nVariaveis;
    procFilho->variaveis = (int*) malloc(sizeof(int) * procFilho->nVariaveis);

    if(procFilho->variaveis != NULL){
        for(int j = 0; j < procFilho->nVariaveis; j++){
            procFilho->variaveis[j]=procPai->variaveis[j];  //Copia a lista de variáveis
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

//Adiona processos filhos à lista de prontos
void computaProcessosCriados(GerenciadorProcesso* gp, TFila* processosCriados, int escFlag){
    TItem criado;
    //Se foram criados processos, são inseridos em prontos
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
