#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../include/TAD_GerenciadorProcesso.h"
#include "../include/TAD_LeituraArquivo.h"
#include "../include/TAD_LeituraArquivo.h"
#include "../include/globais.h"
#include "../include/processoImpressao.h"
#include "../include/ProcessoControle.h"

// Em TAD_GerenciadorProcesso.c
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
    int bytesLidos;
    GerenciadorProcesso gp;
    inicializaGerenciadorProcessos(&gp);
    processo init;
    leituraProcessoInit(&init);
    init.quantum = quantumPorPrioridade[init.prioridade];
    AtualizarRegistradorCPU(&gp.cpu,&init);

    printf("[Gerenciador] Iniciado. A aguardar comandos (U, I, M) do pipe...\n");
    while ((bytesLidos = read(fd_leitura, &msg, sizeof(ComandoPipe))) > 0)
    {
        printf("AAAAAA");
        if (msg.tipo == 'U')
        {
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
                        printf("[Gerenciador] Processo %d escalonado para execução.\n", pidEscalonado);
                    }
                }
            }else if(gp.cpu.emUso)
            {
            // troca de contexto
                if (gp.cpu.quantum_usado >= gp.cpu.quantum_total)
                {

                    processo *procAtual = gp.cpu.processo_atual;

                    SalvarContextoCpuQuantum(&gp.cpu);

                    TItem novoItem;
                    novoItem.Chave = procAtual->pid;

                    if (escFlag == MLFQ) {
                        FilaEnfileira(&gp.estadoPronto[procAtual->prioridade], &novoItem);
                    } else if (escFlag == FIFO) {
                        FilaEnfileira(&gp.estadoPronto[0], &novoItem);
                    }
                    // mudar a flag de cpu livre aqui
                    // gp.indiceEstadoExecucao = -1; // CPU fica livre
                }
            }
            // execução
            if(gp.cpu.emUso)
            {

                // se a instrucao que esta no pc counter for F, chamar a funcao de clonagem (transferir ela da cpu para o gerenciador talvez?) e colocar o processo filho na lista de pronto
                //  alguma função que lê a instrução no PC atual e faz a operação
                executaInstrucoes(&gp.cpu);
                TItem novoItem;

                if (gp.cpu.listaInstrucao[gp.cpu.registradorPC].tipo == 'B')
                {
                    SalvarContextoCpuBloqueio(&gp.cpu);
                }
                else if (gp.cpu.listaInstrucao[gp.cpu.registradorPC].tipo == 'T')
                {
                    processo *procAtual = gp.cpu.processo_atual;

                    SalvarContextoCpuTermino(&gp.cpu);

                    novoItem.Chave = procAtual->pid;
                    FilaEnfileira(&gp.finalizados, &novoItem);
                }
                else if (gp.cpu.listaInstrucao[gp.cpu.registradorPC].tipo == 'F')
                {
                    processo *processoFilhinho = clonaProcesso(&gp.cpu);
                    novoItem.Chave = gp.cpu.processo_atual->pid;

                    if (escFlag == MLFQ){
                        FilaEnfileira(&gp.estadoPronto[processoFilhinho->prioridade],&novoItem);//escalonador MLFQ
                    }
                    else if (escFlag == FIFO){
                        FilaEnfileira(&gp.estadoPronto[0], &novoItem);
                    }
                }

                gp.cpu.registradorPC++;
                IncrementarQuantum_usado(&gp.cpu);
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
    ;

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
            break;

        case 'D':
            sscanf(linha, " %c %d", &comando, &x);
            // printf("Comando D\n");
            // printf("X: %d\n\n", x);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            break;

        case 'V':
            sscanf(linha, " %c %d %d", &comando, &x, &n);
            // printf("Comando V\n");
            // printf("X: %d | N: %d\n\n", x, n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            listaInstrucoes[iterador].n = n;
            break;

        case 'A':
        case 'S':
            sscanf(linha, " %c %d %d", &comando, &x, &n);
            // printf("Comando %c\n", comando);
            // printf("X: %d | N: %d\n\n", x, n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            listaInstrucoes[iterador].n = n;
            break;

        case 'R':
            sscanf(linha, " %c %s", &comando, caminho);
            // printf("Comando R\n");
            // printf("Arquivo: %s\n\n", caminho);
            listaInstrucoes[iterador].tipo = comando;
            strcpy(listaInstrucoes[iterador].caminhoArquivo, caminho);
            break;

        case 'F':
            sscanf(linha, " %c %d", &comando, &n);
            // printf("Comando F\n");
            // printf("X: %d\n\n", x);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].n = n;
            break;
        case 'T':
            sscanf(linha, " %c", &comando);
            // printf("Comando %c\n", comando);
            listaInstrucoes[iterador].tipo = comando;
            // printf("\n");
            // printf("-----encerra execucao-----");
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
    procFilho->nInstrucoes = procPai->nInstrucoes;

    procFilho->pcCounter = procPai->pcCounter + 1;
    procFilho->estado = PRONTO;

    procFilho->quantum = 0;
    procFilho->quantum_usado_CPUatual = 0;

    procFilho->tempoBloqueado = 0;

    procFilho->prioridade = procPai->prioridade;

    procFilho->listaInstrucoes = (instrucao *)malloc(sizeof(instrucao) * procFilho->nInstrucoes);
    if (procFilho->listaInstrucoes != NULL)
    {
        for (int i = 0; i < procPai->nInstrucoes; i++)
        {
            procFilho->listaInstrucoes[i] = procPai->listaInstrucoes[i];
        }
    }

    return procFilho;
}
