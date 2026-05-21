#ifndef LEITURAARQUIVO_H
#define LEITURAARQUIVO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/Cpu.h"

int contarLinhasArquivo(char *caminho);
int leituraArquivoProcesso(char *caminho, cpu_s *cpu);

//int leituraProcessoInit();

#endif