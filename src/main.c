#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/TAD_processo_simulado.h"
#include "../include/TAD_LeituraArquivo.h"
#include "../include/TAD_TabelaProcessos.h"
#include "../include/TAD_CPU.h"
#include "../include/TAD_GerenciadorProcesso.h"


int main(){

        cpu_s cpu;
        TabelaDeProcessos tabela;
        processo init;
        inicializarTabelaProcessos(&tabela);
        leituraProcessoInit(&tabela);
        // if(buscarProcessoTabela(&tabela, 0) == NULL){
        //     printf("EWEEEEEEEEEEEEEEEEEEEEe");
        // }
        // inicializarCPU(&cpu);
        // AtualizarRegistradorCPU(&cpu,(buscarProcessoTabela(&tabela, 0)), 1);
        //executaInstrucoes(&cpu);
//     char caminho[256] = "file_a.txt";
//     leituraProcessoInit();
//     leituraArquivoProcesso(caminho);
   

    return 0;
}