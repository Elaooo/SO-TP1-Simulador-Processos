#define NUM_NIVEIS_PRIORIDADE 4 // Prioridades 0 (mais alta) a 3
// Quantum por nível de prioridade (usado pelo Gerenciador)
#define QUANTUM_PRIORIDADE_0 1  // Prioridade mais alta
#define QUANTUM_PRIORIDADE_1 2
#define QUANTUM_PRIORIDADE_2 4
#define QUANTUM_PRIORIDADE_3 8  // Prioridade mais baixa

void escalonadorMLFQ(){
//tenho que olhar para o processo que estava na cpu
//if tem processo na cpu
// guardo o id do processo que estava na cpu
//guarda o quantum usado ate este momento
//11111verifica o quanto usou do quantum >= quantum
    //verifica se a prioridade atual e menor que 3(menor prioridade)
        //se usou tudo ele deve diminuir a prioridade(++)
        //lembrar de mudar o quantum dele tambem pq a prioridade mudou
        //zerar o quantum_usado atual
        //reescalonar_mesmo_processo_com_nova_prioridade = 1; // Marca para resetar o tempo da CPU se ele for o próximo
    // Já está na prioridade mais baixa e estourou o quantum,
        // simplesmente volta para o fim da fila da sua prioridade.
        // O tempo_usado_no_quantum_atual dele também zera para um novo ciclo.
        //processo_que_estava_na_cpu->tempo_usado_no_quantum_atual = 0;
        //reescalonar_mesmo_processo_com_nova_prioridade = 1; // Marca para resetar o tempo da CPU se ele for o próximo
    //processo nao usou todo o quantum e foi bloqueado aumenta prioridade e poe na fila de bloqueado
    //Se o processo não terminou nem bloqueou, ele volta para a fila de prontos
//222222Selecionar prox processo
// 2. SELECIONAR O PRÓXIMO PROCESSO PARA EXECUTAR
// A regra de ouro do MLFQ é: "Nenhum processo de prioridade menor roda se houver alguém na fila de prioridade maior".

// Como pensar a escolha:
// Primeiro, você deve percorrer as filas da maior prioridade (0) até a menor (3).
// Use um laço (como um 'for') que itera sobre os níveis de prioridade.

// Dentro desse laço:
    // A pergunta principal é: "A fila deste nível atual possui algum processo pronto?"

    // SE SIM (achou um processo):
        // 1. Esse é o seu processo escolhido!
        // 2. Pela regra FIFO (First-In, First-Out) dentro da própria fila, você pega sempre
        //    o que está na 'cabeça' (o que chegou primeiro naquela fila específica).
        // 3. Você remove ele da fila de prontos e interrompe a busca (break), pois já achou quem vai rodar.

    // SE NÃO (fila vazia):
        // 1. Você passa para o próximo nível de prioridade (ex: de 0 para 1) e repete a pergunta.

// Ao final da busca:
// Se você percorreu todos os níveis e não encontrou ninguém, a CPU fica OCIOSA.
// Se encontrou, você terá o PID ou o ponteiro do processo para carregar na CPU no Passo 3.
//33333
//escolha
//configurar cpu para prox processo
//    printf("[Gerenciador] Processo %d escalonado para execução (PC=%d, Prioridade=%d, Quantum=%d)\n",
             //proximo_processo_a_executar->pid, proximo_processo_a_executar->pc,
             //proximo_processo_a_executar->prioridade, gerenciador->cpu_sistema.quantum_total_alocado);

}