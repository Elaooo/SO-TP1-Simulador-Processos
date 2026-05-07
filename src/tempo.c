#include "../include/tempo.h"
#include <stdio.h>


void InicializaTempo(Tempo * tempo){
    tempo->valor = 0;
}

int ZeraTempo(Tempo * tempo){
    if(tempo->valor){
        tempo->valor = 0;
        return 1;
    }
    return 0;
}

int IncrementaTempo(Tempo * tempo){
    tempo->valor++;
    return tempo->valor;
}