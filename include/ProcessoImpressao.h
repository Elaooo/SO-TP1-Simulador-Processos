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
void ImprimeProcessosEmExecucaoMLFQ(Informacoes * info);
void ImprimeProcessosProntosMLFQ(Informacoes * info);
void ImprimeProcessosBloqueadosMLFQ(Informacoes * info);
void ImprimeTodasAsCPUsMLFQ(Informacoes * info);
void ImprimeCPUMLFQ(Informacoes * info, int cpu);
void ImprimeProcessosEmExecucaoFIFO(Informacoes * info);
void ImprimeProcessosProntosFIFO(Informacoes * info);
void ImprimeProcessosBloqueadosFIFO(Informacoes * info);
void ImprimeTodasAsCPUsFIFO(Informacoes * info);
void ImprimeCPUFIFO(Informacoes * info, int cpu);

#endif