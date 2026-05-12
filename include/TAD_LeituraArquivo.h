#ifndef TAD_LEITURAARQUIVO_H
#define TAD_LEITURAARQUIVO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/TAD_CPU.h"

int contarLinhasArquivo(char *caminho);
int leituraArquivoProcesso(char *caminho, cpu_s *cpu);

//int leituraProcessoInit();

#endif