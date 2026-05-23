#include <stdio.h>
#include <stdlib.h>

#include "../include/Fila.h"

void FazFilaVazia(TFila* pFila){
    pFila->pFrente =(Celula*)malloc(sizeof(Celula));
    pFila->tam=0;
    pFila->pTras = pFila->pFrente;
    pFila->pFrente->pProx = NULL;
}

int FilaEhVazia(TFila* pFila){
    return (pFila->pFrente == pFila->pTras);
} 

int FilaEnfileira(TFila *pFila,TItem* pItem){
    Celula* pNovo;
    pNovo = (Celula*)malloc(sizeof(Celula));
    if (pNovo == NULL) return 0;
    pFila->tam++;
    pFila->pTras->pProx = pNovo;
    pFila->pTras = pNovo;
    pNovo->Item = *pItem;
    pNovo->pProx = NULL;
    return 1;
}

int FilaDesenfileira(TFila* pFila,TItem* pItem){
    Celula* pAux;
    if (FilaEhVazia(pFila)) return 0;
    pFila->tam--;
    pAux = pFila->pFrente;
    pFila->pFrente = pFila->pFrente->pProx;
    *pItem = pFila->pFrente->Item;

    free(pAux);

    return 1;
}

void ImprimeFila(TFila* pFila){
    if(FilaEhVazia(pFila)){
        printf("Fila Vazia!!!\n");
        return;
    }
    Celula* aux = pFila->pFrente->pProx;
    do{
   

        printf("PID: %d\n",aux->Item.Chave);
        
        
        aux = aux->pProx;
      
    }while(aux != NULL);
  
    return;
}

int FilaRemovePorChave(TFila* pFila, TipoChave chave) {
    if (FilaEhVazia(pFila)) {
        return 0;
    }

    Celula* anterior = pFila->pFrente;
    Celula* atual = pFila->pFrente->pProx;

    while (atual != NULL) {
        if (atual->Item.Chave == chave) {
            anterior->pProx = atual->pProx;

            if (atual == pFila->pTras) {
                pFila->pTras = anterior;
            }

            free(atual);
            pFila->tam--;

            return 1;
        }

        anterior = atual;
        atual = atual->pProx;
    }

    return 0;
}