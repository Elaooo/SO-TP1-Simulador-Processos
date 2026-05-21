#ifndef PROCESSOCONTROLE_H
#define PROCESSOCONTROLE_H

typedef struct {
    char tipo;
    int opcaoImpressao;
} ComandoPipe;

int inicializaProcessoControle(int argc, char *argv[]);
int lerOpcaoImpressao();
#endif