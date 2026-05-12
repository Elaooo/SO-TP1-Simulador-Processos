#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <../include/TAD_GerenciadorProcesso.h>
// Em TAD_GerenciadorProcesso.c
//do escalonador
static int quantumPorPrioridade[NUM_PRIORIDADES] = {
    QUANTUM_PRIORIDADE_0,
    QUANTUM_PRIORIDADE_1,
    QUANTUM_PRIORIDADE_2,
    QUANTUM_PRIORIDADE_3
};

//retorna o pid do processo que vai entrar va cpu
//troca de contexto responsavel por retirar o que esta na cpu(uma vez que ele que é responsavel por colocar o processo que esta la na fila)
//e passar o que vai entrar
int escalonadorMLFQ(GerenciadorProcesso* gerenciador) {

    //PASSO 1 — Tratar o processo que estava na CPU
    //caso tenha
    if (gerenciador->cpu.processo_atual != NULL) {

        processo* procAtual = gerenciador->cpu.processo_atual;
        int quantumUsado    = gerenciador->cpu.quantum_usado;
        int quantumTotal    = gerenciador->cpu.quantum_total;

        if (quantumUsado >= quantumTotal) {
            //Esgotou o quantum: diminui prioridade (aumenta o índice)
            if (procAtual->prioridade < NUM_PRIORIDADES - 1) {
                procAtual->prioridade++;
                printf("[Gerenciador] Processo %d teve prioridade diminuida para %d (quantum estourado)\n",
                procAtual->pid, procAtual->prioridade);
            }
            //Atualiza o quantum do processo para o novo nível
            procAtual->quantum = quantumPorPrioridade[procAtual->prioridade];
            procAtual->quantum_usado_CPUatual = 0;
            procAtual->estado = PRONTO;

            //Reinicia o contexto da CPU(acho que isso fica com a troca de contexto

            gerenciador->cpu.processo_atual = NULL;
            gerenciador->cpu.registradorPC = 0;
            gerenciador->cpu.quantum_total = 0;
            gerenciador->cpu.quantum_usado = 0;

            //Reinsere na fila de prontos do novo nível de prioridade
            TItem item;
            item.Chave = procAtual->pid;
            FilaEnfileira(&gerenciador->estadoPronto[procAtual->prioridade], &item);
            printf("[Gerenciador] Processo %d reinserido na fila de prontos (Prioridade: %d)\n",
            procAtual->pid, procAtual->prioridade);

        } else {
            //Não esgotou o quantum: foi bloqueado. Aumenta prioridade
            if (procAtual->prioridade > 0) {
                procAtual->prioridade--;
                printf("[Gerenciador] Processo %d teve prioridade aumentada para %d (bloqueado antes de usar todo o quantum)\n",
                procAtual->pid, procAtual->prioridade);
            }
            procAtual->quantum = quantumPorPrioridade[procAtual->prioridade];
            procAtual->quantum_usado_CPUatual = 0;
            procAtual->estado = BLOQUEADO;

            //Reinicia o contexto da CPU(acho que isso fica com a troca de contexto
            gerenciador->cpu.processo_atual = NULL;
            gerenciador->cpu.registradorPC = 0;
            gerenciador->cpu.quantum_total = 0;
            gerenciador->cpu.quantum_usado = 0;

            //Insere na fila de bloqueados
            TItem item;
            item.Chave = procAtual->pid;
            FilaEnfileira(&gerenciador->estadoBloquado, &item);
        }
    }

    //PASSO 2 — Selecionar o próximo processo
    processo* proximoProcesso = NULL;

    for (int prioridade = 0; prioridade < NUM_PRIORIDADES; prioridade++) {

        if (!FilaEhVazia(&gerenciador->estadoPronto[prioridade])) {

            TItem itemRetirado;
            if (FilaDesenfileira(&gerenciador->estadoPronto[prioridade], &itemRetirado)) {

                proximoProcesso = buscarProcessoTabela(
                    &gerenciador->tabelaProcessos,
                    itemRetirado.Chave
                );

                if (proximoProcesso != NULL) {
                    break;//Achou — para a busca
                }
            }
        }
    }

    //PASSO 3 — Resultado
    if (proximoProcesso == NULL) {
        //Nenhum processo pronto: CPU ociosa
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

void rodarGerenciador(int fd_leitura) {
    char comando;
    int bytesLidos;
    gerenciadorProcessos gp;
    inicializaGerenciadorProcessos(gp);
    printf("[Gerenciador] Iniciado. A aguardar comandos (U, I, M) do pipe...\n");


    while ((bytesLidos = read(fd_leitura, &comando, sizeof(char))) > 0) {
        
        if (comando == 'U') {
            //escalonamento
            
            if (gp.cpu.processo_atual == NULL) {
                int pidEscalonado = escalonadorMLFQ(&gp);
                if (pidEscalonado != -1) {
                    gp.cpu.processo_atual = buscarProcessoTabela(&gp.tabelaProcessos, pidEscalonado);
                    gp.indiceEstadoExecucao = 0; // CPU agora tem um processo
                    printf("[Gerenciador] Processo %d escalonado para execução.\n", pidEscalonado);
                }
            }

            // execução
            if (gp->cpu.processo_atual != NULL) {
                // alguma função que lê a instrução no PC atual e faz a operação
         
                
                gp->cpu.registradorPC++;
                IncrementarQuantum_usado(&gp->cpu);
            }

            IncrementaTempo(&gp->tempo);

            // troca de contexto
            if (gp->cpu.processo_atual != NULL) {
                if (gp->cpu.quantum_usado >= gp->cpu.quantum_total) {

                    SalvarContextoCPU(&gp->cpu);
                    
                    TItem novoItem;
                    novoItem.Chave = gp->cpu.processo_atual->pid;
                    FilaEnfileira(&gp->estadoPronto, &novoItem);
                    
                    gp->cpu.processo_atual = NULL; 
                    gp->indiceEstadoExecucao = -1; // CPU fica livre
                }
            }

        } else if (comando == 'I') {
            printf("\n--- ESTADO DO SISTEMA NO TEMPO %d ---\n", gp->tempo.valor);
            // fazer função de print

        } else if (comando == 'M') {
            printf("\n[Gerenciador] A encerrar simulação. Tempo final: %d\n", gp->tempo.valor);
            break; 
            
        } else {
            printf("[Gerenciador] Comando ignorado: %c\n", comando);
        }
    }
}

int leituraProcessoInit(TabelaDeProcessos *tabelaProcessos){
    //variaveis leitura
    char caminho[256] = "data/init.txt";
    char linha[256];
    char comando;
    int x;
    int n;
    int iterador = 0;
    
    FILE *arquivo = fopen(caminho, "r");
    if(!arquivo){
        
        printf("Falha ao abrir arquivo, necessita arquivo init.txt em /data/\n");
        return 0;
    }
    
    //variaveis processo
    int qntdInstruções = contarLinhasArquivo(caminho);
    int pidInicial = 0;
    processo processo;
    
    instrucao *listaInstrucoes = malloc(qntdInstruções * sizeof(instrucao));;

    if (!listaInstrucoes) {
        printf("Erro de alocacao\n");
    return 0;
    }

    //aqui comeca leitura
    while (fgets(linha, sizeof(linha), arquivo)) {

    comando = ' ';
    n = 0;
    x = 0;

    if (sscanf(linha, " %c", &comando) != 1) 
        continue;

    switch (comando) {

        case 'N':
            sscanf(linha, " %c %d", &comando, &n);
            printf("Comando N\n");
            printf("N: %d\n\n", n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].n = n;
            break;

        case 'D':
            sscanf(linha, " %c %d", &comando, &x);
            printf("Comando D\n");
            printf("X: %d\n\n", x);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            break;

        case 'V':
            sscanf(linha, " %c %d %d", &comando, &x, &n);
            printf("Comando V\n");
            printf("X: %d | N: %d\n\n", x, n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            listaInstrucoes[iterador].n = n;
            break;

        case 'A':
        case 'S':
            sscanf(linha, " %c %d %d", &comando, &x, &n);
            printf("Comando %c\n", comando);
            printf("X: %d | N: %d\n\n", x, n);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            listaInstrucoes[iterador].n = n;
            break;

        case 'R':
            sscanf(linha, " %c %s", &comando, caminho);
            printf("Comando R\n");
            printf("Arquivo: %s\n\n", caminho);
            listaInstrucoes[iterador].tipo = comando;
            strcpy(listaInstrucoes[iterador].caminhoArquivo,caminho);
            break;

        case 'F':
            sscanf(linha, " %c %d", &comando, &x);
            printf("Comando F\n");
            printf("X: %d\n\n", x);
            listaInstrucoes[iterador].tipo = comando;
            listaInstrucoes[iterador].x = x;
            break;
        case 'T':
            sscanf(linha, " %c", &comando);
            printf("Comando %c\n", comando);
            listaInstrucoes[iterador].tipo = comando;
            printf("\n");
            printf("-----encerra execucao-----");
            break;

        default:
            printf("Comando desconhecido: %c\n\n", comando);
            break;
    }
    iterador++;
}
    // printf("Instruçoes guardadas\n");
    // inicializarProcessoInit(&processo, pidInicial,listaInstrucoes,qntdInstruções);
    
    // imprimirInstrucoes(processo.listaInstrucoes,qntdInstruções);

    inserirProcessoTabela(tabelaProcessos, &processo);
    free(listaInstrucoes);


    fclose(arquivo);

    return 1;
}

int inicializaGerenciadorProcessos(GerenciadorProcesso *gerenciadorProcessos){

    inicializarTabelaProcessos(&gerenciadorProcessos->tabelaProcessos);
    inicializarCPU(&gerenciadorProcessos->cpu);
    InicializaTempo(&gerenciadorProcessos->tempo);
    FazFilaVazia(&gerenciadorProcessos->estadoPronto);
    FazFilaVazia(&gerenciadorProcessos->estadoEmExecucao);
    FazFilaVazia(&gerenciadorProcessos->estadoBloquado);

    if(gerenciadorProcessos == NULL){
        return 0;
    }else{
        return 1;
    }

}
