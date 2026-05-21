#include "../include/Escalonador.h"

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