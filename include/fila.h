#ifndef TABELA_H
#define TABELA_H

typedef int TipoChave;
#define MaxTam 10

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
} TFila;

void FazFilaVazia(TFila * pFila);

int FilaEhVazia(TFila* pFila);

int FilaEnfileira(TFila* pFila,TItem* pItem);

int FilaDesenfileira(TFila* pFila,TItem* pItem);
void ImprimeFila(TFila* pFila);

#endif