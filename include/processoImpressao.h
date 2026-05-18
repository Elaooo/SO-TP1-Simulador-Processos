#ifndef PROCESSO_IMPRESSAO_H
#define PROCESSO_IMPRESSAO_H

#include "../include/TAD_GerenciadorProcesso.h"
#include "../include/fila.h"
#include "../include/tempo.h"
#include "../include/TAD_CPU.h"
#include "../include/TAD_TabelaProcessos.h"

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