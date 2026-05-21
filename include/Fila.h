#ifndef FILA_H
#define FILA_H

typedef int TipoChave;

typedef struct {
    TipoChave Chave;
}TItem;


typedef struct Celula {
    TItem Item;
    struct Celula* pProx;
}Celula;

typedef struct TFila {
    Celula* pFrente;
    Celula* pTras;
    int tam;
} TFila;

void FazFilaVazia(TFila * pFila);

int FilaEhVazia(TFila* pFila);

int FilaEnfileira(TFila* pFila,TItem* pItem);

int FilaDesenfileira(TFila* pFila,TItem* pItem);
void ImprimeFila(TFila* pFila);

#endif