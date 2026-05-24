#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#define QUANTUM_PRIORIDADE_0 1
#define QUANTUM_PRIORIDADE_1 2
#define QUANTUM_PRIORIDADE_2 4
#define QUANTUM_PRIORIDADE_3 8
#define NUM_PRIORIDADES 4


#include "../include/GerenciadorProcesso.h"


int escalonadorMLFQ(GerenciadorProcesso *gerenciador,int idCPU);
int escalonadorFIFO(GerenciadorProcesso *gerenciador, int idCPU);
// Recalcula a prioridade do processo cujo quantum esgotou e o reinsere
// imediatamente na fila de prontos do MLFQ correta.
// Deve ser chamada pelo GerenciadorProcesso logo após quantumEsgotado().
void mlfqReinserirProcesso(GerenciadorProcesso *gerenciador, processo *proc);

#endif
