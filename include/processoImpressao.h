#ifndef PROCESSO_IMPRESSAO_H
#define PROCESSO_IMPRESSAO_H

#include <TAD_GerenciadorProcesso.h>
#include <fila.h>
#include <tempo.h>
#include <TAD_CPU.h>
#include <TAD_TabelaProcessos.h>

typedef struct{
    GerenciadorProcesso * gerenciadorProcesso;
    int qtdProcessosProntos;
    int qtdProcessosBloqueados;
    int qtdProcessosEmExecucao;
    int qtdProcessos;
    processo *processoAtual;
}Informacoes;

void ColetaInformacoes(Informacoes * info, GerenciadorProcesso * gerenciadorProcesso);
void Imprime(GerenciadorProcesso * gerenciadorProcesso);
void ImprimeTodosOsProcessos(Informacoes * info);
void ImprimeProcessosEmExecucao(Informacoes * info);
void ImprimeProcessosProntos(Informacoes * info);
void ImprimeProcessosBloqueados(Informacoes * info);
void ImprimeInformacoesGerais(Informacoes * info);


#endif