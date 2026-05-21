#ifndef TEMPO_H
#define TEMPO_H

typedef struct{

    int valor;

}Tempo;

void InicializaTempo(Tempo * tempo);
int ZeraTempo(Tempo * tempo);
int IncrementaTempo(Tempo * tempo);


#endif