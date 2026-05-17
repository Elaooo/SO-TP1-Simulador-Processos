#ifndef PROCESSO_CONTROLE_H
#define PROCESSO_CONTROLE_H

typedef struct {
    char tipo;
    int opcaoImpressao;
} ComandoPipe;

int inicializaProcessoControle(int argc, char *argv[]);
int lerOpcaoImpressao();
#endif