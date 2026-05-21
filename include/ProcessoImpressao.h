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
    int qtdProcessosBloqueados;
    int qtdProcessosEmExecucao;
    int qtdProcessos;
    processo *processoAtual;
}Informacoes;

void ColetaInformacoes(Informacoes * info, GerenciadorProcesso * gerenciadorProcesso);
void Imprime(GerenciadorProcesso * gerenciadorProcesso, int opcao);
void ImprimeTodosOsProcessos(Informacoes * info);
void ImprimeProcessosEmExecucao(Informacoes * info);
void ImprimeProcessosProntos(Informacoes * info);
void ImprimeProcessosBloqueados(Informacoes * info);
void ImprimeInformacoesGerais(Informacoes * info);


#endif