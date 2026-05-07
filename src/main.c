#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/TAD_processo_simulado.h"
#include "../include/TAD_LeituraArquivo.h"

int main(){

    char caminho[256] = "file_a.txt";
    leituraProcessoInit();
    leituraArquivoProcesso(caminho);
   
    return 0;
}