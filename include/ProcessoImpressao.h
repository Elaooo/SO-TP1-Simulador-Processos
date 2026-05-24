#ifndef PROCESSOIMPRESSAO_H
#define PROCESSOIMPRESSAO_H

#include "../include/GerenciadorProcesso.h"
#include "../include/Fila.h"
#include "../include/Tempo.h"
#include "../include/Cpu.h"
#include "../include/TabelaProcessos.h"

typedef struct{
    GerenciadorProcesso * gerenciadorProcesso;
    int qtdProcessosProntos;
    int qtdProcessosProntosPrioridade[4];
    int qtdProcessosBloqueados;
    int qtdProcessosEmExecucao;
    int qtdProcessos;
    int escFlag;
    int nCPUs;
}Informacoes;

void ColetaInformacoes(Informacoes * info, GerenciadorProcesso * gerenciadorProcesso, int escFlag);
void Imprime(GerenciadorProcesso * gerenciadorProcesso, int opcao, int escFlag);
void ImprimeTodosOsProcessos(Informacoes * info);
void ImprimeProcessosEmExecucao(Informacoes * info);
void ImprimeProcessosProntos(Informacoes * info);
void ImprimeProcessosBloqueados(Informacoes * info);

void ImprimeTodasAsCPUs(Informacoes * info);
void ImprimeCPU(Informacoes * info, int cpu);

#endif