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

// Em GerenciadorProcesso.c
// do escalonador
static int quantumPorPrioridade[NUM_PRIORIDADES] = {
    QUANTUM_PRIORIDADE_0,
    QUANTUM_PRIORIDADE_1,
    QUANTUM_PRIORIDADE_2,
    QUANTUM_PRIORIDADE_3};

// retorna o pid do processo que vai entrar va cpu
// troca de contexto responsavel por retirar o que esta na cpu(uma vez que ele que é responsavel por colocar o processo que esta la na fila)
// e passar o que vai entrar
int escalonadorMLFQ(GerenciadorProcesso *gerenciador)
{

    // PASSO 1 — Tratar o processo que estava na CPU
    // caso tenha
    if (gerenciador->cpu.processo_atual != NULL)
    {

        processo *procAtual = gerenciador->cpu.processo_atual;
        int quantumUsado = gerenciador->cpu.quantum_usado;
        int quantumTotal = gerenciador->cpu.quantum_total;

        if (quantumUsado >= quantumTotal)
        {
            // Esgotou o quantum: diminui prioridade (aumenta o índice)
            if (procAtual->prioridade < NUM_PRIORIDADES - 1)
            {
                procAtual->prioridade++;
                printf("[Gerenciador] Processo %d teve prioridade diminuida para %d (quantum estourado)\n",
                       procAtual->pid, procAtual->prioridade);
            }
            // Atualiza o quantum do processo para o novo nível
            procAtual->quantum = quantumPorPrioridade[procAtual->prioridade];
            procAtual->quantum_usado_CPUatual = 0;

            // Reinicia o contexto da CPU(acho que isso fica com a troca de contexto

            gerenciador->cpu.processo_atual = NULL;
            gerenciador->cpu.registradorPC = 0;
            gerenciador->cpu.quantum_total = 0;
            gerenciador->cpu.quantum_usado = 0;

            // Reinsere na fila de prontos do novo nível de prioridade
            TItem item;
            item.Chave = procAtual->pid;
            FilaEnfileira(&gerenciador->estadoPronto[procAtual->prioridade], &item);
            printf("[Gerenciador] Processo %d reinserido na fila de prontos (Prioridade: %d)\n",
                   procAtual->pid, procAtual->prioridade);
        }
        else
        {
            // Não esgotou o quantum: foi bloqueado. Aumenta prioridade
            if (procAtual->prioridade > 0 && procAtual->estado == BLOQUEADO)
            {
                procAtual->prioridade--;
                printf("[Gerenciador] Processo %d teve prioridade aumentada para %d (bloqueado antes de usar todo o quantum)\n",
                       procAtual->pid, procAtual->prioridade);
            }
            procAtual->quantum = quantumPorPrioridade[procAtual->prioridade];
            procAtual->quantum_usado_CPUatual = 0;

            // Reinicia o contexto da CPU(acho que isso fica com a troca de contexto
            gerenciador->cpu.processo_atual = NULL;
            gerenciador->cpu.registradorPC = 0;
            gerenciador->cpu.quantum_total = 0;
            gerenciador->cpu.quantum_usado = 0;

            // Insere na fila de bloqueados
            TItem item;
            item.Chave = procAtual->pid;
            FilaEnfileira(&gerenciador->estadoBloquado, &item);
        }
    }

    // PASSO 2 — Selecionar o próximo processo
    processo *proximoProcesso = NULL;

    for (int prioridade = 0; prioridade < NUM_PRIORIDADES; prioridade++)
    {

        if (!FilaEhVazia(&gerenciador->estadoPronto[prioridade]))
        {

            TItem itemRetirado;
            if (FilaDesenfileira(&gerenciador->estadoPronto[prioridade], &itemRetirado))
            {

                proximoProcesso = buscarProcessoTabela(
                    &gerenciador->tabelaProcessos,
                    itemRetirado.Chave);

                if (proximoProcesso != NULL)
                {
                    break; // Achou — para a busca
                }
            }
        }
    }

    // PASSO 3 — Resultado
    if (proximoProcesso == NULL)
    {
        // Nenhum processo pronto: CPU ociosa
        printf("[Escalonador] CPU ociosa — nenhum processo pronto.\n");
        return -1;
    }

    printf("[Escalonador] Processo %d escalonado (prioridade=%d, quantum=%d, PC=%d)\n",
           proximoProcesso->pid,
           proximoProcesso->prioridade,
           quantumPorPrioridade[proximoProcesso->prioridade],
           proximoProcesso->pcCounter);

    return proximoProcesso->pid;
}
// retorna o pid do processo que vai entrar na cpu
// FIFO nao utiliza de quantum, quantum usado e nem de prioridade

int escalonadorFIFO(GerenciadorProcesso *gerenciador)
{

    // PASSO 1 — Tratar o processo que estava na CPU
    // se tiver processo na cpu
    if (gerenciador->cpu.processo_atual != NULL)
    {

        processo *procAtual = gerenciador->cpu.processo_atual;
        TItem item;

        if (procAtual->estado == BLOQUEADO)
        {
            // insere na fila de bloqueados
            item.Chave = procAtual->pid;
            FilaEnfileira(&gerenciador->estadoBloquado, &item);
            printf("[FIFO] Processo %d inserido na fila Bloqueado\n", procAtual->pid);
        }
        if (procAtual->estado == TERMINADO)
        {
            gerenciador->totalProcessosFinalizados++;
            printf("[FIFO] Processo %d Terminou\n", procAtual->pid);
        }
        if (procAtual->estado == PRONTO)
        {
            // insere na fila de prontos, caso necessario
            item.Chave = procAtual->pid;
            FilaEnfileira(&gerenciador->estadoPronto[0], &item);
            printf("[FIFO] Processo %d inserido na fila de prontos\n", procAtual->pid);
        }
        // Reinicia a cpu para o processo novo(retira o que colocamos na fila no passo acima)
        gerenciador->cpu.processo_atual = NULL;
        gerenciador->cpu.registradorPC = 0;
    }
    // PASSO 2 — Selecionar o próximo processo
    processo *proximoProcesso = NULL;
    if (!FilaEhVazia(&gerenciador->estadoPronto[0]))
    {
        TItem itemRetirado;
        if (FilaDesenfileira(&gerenciador->estadoPronto[0], &itemRetirado))
        {
            proximoProcesso = buscarProcessoTabela(&gerenciador->tabelaProcessos, itemRetirado.Chave);
        }
    }

    // PASSO 3 — Resultado
    if (proximoProcesso == NULL)
    {
        // Nenhum processo pronto: CPU ociosa
        printf("[FIFO] CPU ociosa — nenhum processo pronto.\n");
        return -1;
    }

    printf("[FIFO] Processo %d escalonado .\n",
           proximoProcesso->pid);

    return proximoProcesso->pid;
}
void rodarGerenciador(int fd_leitura, int escFlag)
{   
    ComandoPipe msg;
    TItem novoItem;


    int bytesLidos;
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


    printf("[Gerenciador] Iniciado. A aguardar comandos (U, I, M) do pipe...\n");
    while ((bytesLidos = read(fd_leitura, &msg, sizeof(ComandoPipe))) > 0)
    {
        if (msg.tipo == 'U')
        {
            TItem novoItem;
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
                        // gp.indiceEstadoExecucao = 0; // CPU agora tem um processo
                        AtualizarRegistradorCPU(&gp.cpu, gp.cpu.processo_atual);

                        novoItem.Chave = gp.cpu.processo_atual->pid;
                        FilaEnfileira(&gp.estadoEmExecucao,&novoItem);

                        printf("[Gerenciador] Processo %d escalonado para execução.\n", pidEscalonado);
                    }
                }
                else
                {

                    int pidEscalonado = escalonadorFIFO(&gp);
                    if (pidEscalonado != -1)
                    {
                        gp.cpu.processo_atual = buscarProcessoTabela(&gp.tabelaProcessos, pidEscalonado);
                        // gp.indiceEstadoExecucao = 0; // CPU agora tem um processo
                        AtualizarRegistradorCPU(&gp.cpu, gp.cpu.processo_atual);

                        novoItem.Chave = gp.cpu.processo_atual->pid;
                        FilaEnfileira(&gp.estadoEmExecucao,&novoItem);
                        
                        printf("[Gerenciador] Processo %d escalonado para execução.\n", pidEscalonado);
                    }
                }
            }else if(gp.cpu.emUso)
            {
            // troca de contexto
                if (escFlag == MLFQ && gp.cpu.quantum_usado >= gp.cpu.quantum_total)
                {

                    processo *procAtual = gp.cpu.processo_atual;

                    SalvarContextoCpu(&gp.cpu);

                    TItem novoItem;
                    novoItem.Chave = procAtual->pid;

                    if (escFlag == MLFQ) {
                        FilaEnfileira(&gp.estadoPronto[procAtual->prioridade], &novoItem);
                    } else if (escFlag == FIFO) {
                        FilaEnfileira(&gp.estadoPronto[0], &novoItem);
                    }
                    
                    FilaDesenfileira(&gp.estadoEmExecucao,&novoItem);

                    printf("Quantum máximo atingido. Troca de contexto.\n");
                }
            }
            // execução
            if(gp.cpu.emUso)
            {
                printf("--- Executando %c...\n",gp.cpu.listaInstrucao[gp.cpu.registradorPC].tipo);
                executaInstrucoes(&gp.cpu);
                IncrementarQuantum_usado(&gp.cpu);

                if (gp.cpu.listaInstrucao[gp.cpu.registradorPC].tipo == 'B')
                {
                    novoItem.Chave = gp.cpu.processo_atual->pid;
                    SalvarContextoCpu(&gp.cpu);

                    FilaEnfileira(&gp.estadoBloquado, &novoItem);
                    FilaDesenfileira(&gp.estadoEmExecucao,&novoItem);

                    imprimirProcesso(buscarProcessoTabela(&gp.tabelaProcessos,novoItem.Chave));
                }
                else if (gp.cpu.listaInstrucao[gp.cpu.registradorPC].tipo == 'T')
                {
                    
                    novoItem.Chave = gp.cpu.processo_atual->pid;
                    SalvarContextoCpu(&gp.cpu);

                    FilaEnfileira(&gp.finalizados, &novoItem);
                    FilaDesenfileira(&gp.estadoEmExecucao,&novoItem);

                    imprimirProcesso(buscarProcessoTabela(&gp.tabelaProcessos,novoItem.Chave));


                }
                else if (gp.cpu.listaInstrucao[gp.cpu.registradorPC].tipo == 'F')
                {

                    processo *processoFilhinho = clonaProcesso(&gp.cpu);
                    
                    inserirProcessoTabela(&gp.tabelaProcessos, processoFilhinho);
                    novoItem.Chave = processoFilhinho->pid;
                    
                    if (escFlag == MLFQ){
                        FilaEnfileira(&gp.estadoPronto[processoFilhinho->prioridade],&novoItem);//escalonador MLFQ
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
            pid_t pid = fork();
            //Processo filho: Impressao
            if (pid < 0) {
                perror("Erro ao criar processo impressao");
            }
            else if (pid == 0) {
                Imprime(&gp, msg.opcaoImpressao);
                _exit(0);
            }
            else {
                waitpid(pid, NULL, 0);
            }
        }
        else if (msg.tipo == 'M')
        {
            pid_t pid = fork();

            if (pid < 0) {
                perror("Erro ao criar processo impressao");
            }
            else if (pid == 0) {
                Imprime(&gp, msg.opcaoImpressao);
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

int leituraProcessoInit(processo *processo)
{
    // variaveis leitura
    char caminho[256] = "data/init.txt";
    char linha[256];
    char comando;
    int x;
    int n;
    int iterador = 0;

    FILE *arquivo = fopen(caminho, "r");
    if (!arquivo)
    {

        printf("Falha ao abrir arquivo, necessita arquivo init.txt em /data/\n");
        return 0;
    }

    // variaveis processo
    int qntdInstrucoes = contarLinhasArquivo(caminho);

    instrucao *listaInstrucoes = malloc(qntdInstrucoes * sizeof(instrucao));

    if (!listaInstrucoes)
    {
        printf("Erro de alocacao\n");
        return 0;
    }

    // aqui comeca leitura
    while (fgets(linha, sizeof(linha), arquivo))
    {

        comando = ' ';
        n = 0;
        x = 0;

        if (sscanf(linha, " %c", &comando) != 1)
            continue;

        switch (comando)
        {

        case 'N':
            sscanf(linha, " %c %d", &comando, &n);
            // printf("Comando N\n");
            // printf("N: %d\n\n", n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].n = n;
            listaInstrucoes[iterador].x = 0;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;

        case 'D':
            sscanf(linha, " %c %d", &comando, &x);
            // printf("Comando D\n");
            // printf("X: %d\n\n", x);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            listaInstrucoes[iterador].n = 0;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;

        case 'B':
            sscanf(linha, " %c %d", &comando, &n);
            // printf("Comando V\n");
            // printf("X: %d | N: %d\n\n", x, n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = 0;
            listaInstrucoes[iterador].n = n;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;

        case 'V':
            sscanf(linha, " %c %d %d", &comando, &x, &n);
            // printf("Comando V\n");
            // printf("X: %d | N: %d\n\n", x, n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            listaInstrucoes[iterador].n = n;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;

        case 'A':
        case 'S':
            sscanf(linha, " %c %d %d", &comando, &x, &n);
            // printf("Comando %c\n", comando);
            // printf("X: %d | N: %d\n\n", x, n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            listaInstrucoes[iterador].n = n;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;

        case 'R':
            sscanf(linha, " %c %s", &comando, caminho);
            // printf("Comando R\n");
            // printf("Arquivo: %s\n\n", caminho);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].n = 0;
            listaInstrucoes[iterador].x = 0;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, caminho);
            break;
            break;

        case 'F':
            sscanf(linha, " %c %d", &comando, &n);
            // printf("Comando F\n");
            // printf("X: %d\n\n", x);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].n = n;
            listaInstrucoes[iterador].x = 0;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            break;
        case 'T':
            sscanf(linha, " %c", &comando);
            // printf("Comando %c\n", comando);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].n = 0;
            listaInstrucoes[iterador].x = 0;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
            // printf("\n");
            // printf("-----encerra execucao-----");
            break;
            break;

        default:
            printf("Comando desconhecido: %c\n\n", comando);
            break;
        }
        iterador++;
    }
    // printf("Instruçoes guardadas\n");
    inicializarProcessoInit(processo, listaInstrucoes, qntdInstrucoes);

    // imprimirInstrucoes(processo.listaInstrucoes,qntdInstrucoes);
    free(listaInstrucoes);

    fclose(arquivo);

    return 1;
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

// int leituraProcessoInit(processo *processo)
// {
//     int iterador = 0;

//     // variaveis processo
//     int qntdInstrucoes = 12;

//     instrucao *listaInstrucoes = malloc(qntdInstrucoes * sizeof(instrucao));

//     if (!listaInstrucoes)
//     {
//         printf("Erro de alocacao\n");
//         return 0;
//     }

//     // N 2
//     listaInstrucoes[iterador].tipo = 'N';
//     listaInstrucoes[iterador].n = 2;
//     listaInstrucoes[iterador].x = 0;
//     strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
//     iterador++;

//     // D 0
//     listaInstrucoes[iterador].tipo = 'D';
//     listaInstrucoes[iterador].x = 0;
//     listaInstrucoes[iterador].n = 0;
//     strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
//     iterador++;

//     // D 1
//     listaInstrucoes[iterador].tipo = 'D';
//     listaInstrucoes[iterador].x = 1;
//     listaInstrucoes[iterador].n = 0;
//     strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
//     iterador++;

//     // V 0 1000
//     listaInstrucoes[iterador].tipo = 'V';
//     listaInstrucoes[iterador].x = 0;
//     listaInstrucoes[iterador].n = 1000;
//     strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
//     iterador++;

//     // V 1 500
//     listaInstrucoes[iterador].tipo = 'V';
//     listaInstrucoes[iterador].x = 1;
//     listaInstrucoes[iterador].n = 500;
//     strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
//     iterador++;

//     // A 0 19
//     listaInstrucoes[iterador].tipo = 'A';
//     listaInstrucoes[iterador].x = 0;
//     listaInstrucoes[iterador].n = 19;
//     strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
//     iterador++;

//     // A 0 20
//     listaInstrucoes[iterador].tipo = 'A';
//     listaInstrucoes[iterador].x = 0;
//     listaInstrucoes[iterador].n = 20;
//     strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
//     iterador++;

//     // S 1 53
//     listaInstrucoes[iterador].tipo = 'S';
//     listaInstrucoes[iterador].x = 1;
//     listaInstrucoes[iterador].n = 53;
//     strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
//     iterador++;

//     // A 1 55
//     listaInstrucoes[iterador].tipo = 'A';
//     listaInstrucoes[iterador].x = 1;
//     listaInstrucoes[iterador].n = 55;
//     strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
//     iterador++;

//     // F 1
//     listaInstrucoes[iterador].tipo = 'F';
//     listaInstrucoes[iterador].n = 1;
//     listaInstrucoes[iterador].x = 0;
//     strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
//     iterador++;

//     // R file_a
//     listaInstrucoes[iterador].tipo = 'R';
//     listaInstrucoes[iterador].n = 0;
//     listaInstrucoes[iterador].x = 0;
//     strcpy(listaInstrucoes[iterador].caminhoArquivo, "file_a");
//     iterador++;

//     // T
//     listaInstrucoes[iterador].tipo = 'T';
//     listaInstrucoes[iterador].n = 0;
//     listaInstrucoes[iterador].x = 0;
//     strcpy(listaInstrucoes[iterador].caminhoArquivo, "vazio");
//     iterador++;

//     inicializarProcessoInit(processo, listaInstrucoes, qntdInstrucoes);

//     free(listaInstrucoes);

//     return 1;
// }