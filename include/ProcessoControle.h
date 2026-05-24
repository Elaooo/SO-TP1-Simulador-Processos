#ifndef PROCESSOCONTROLE_H
#define PROCESSOCONTROLE_H
#include "Globais.h"


void *threadControle(void *arg);
void escolherArgsGerenciador(ArgsGerenciador * aux);
int lerOpcaoImpressao();
#endif