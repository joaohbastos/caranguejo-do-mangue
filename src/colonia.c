#include <stdlib.h>

#include "colonia.h"

Caranguejo *criarCaranguejo(int id, int ehRei) {
    Caranguejo *novo = (Caranguejo *)malloc(sizeof(Caranguejo));
    if (novo == NULL) {
        return NULL;
    }
    novo->id               = id;
    novo->nivelFome        = 0;
    novo->rodadasNaColonia = 0;
    novo->ehRei            = ehRei ? 1 : 0;
    novo->proximo          = NULL;
    return novo;
}

void inserirCaranguejo(Caranguejo **inicio, Caranguejo *novo) {
    if (inicio == NULL || novo == NULL) {
        return;
    }
    novo->proximo = NULL;

    if (*inicio == NULL) {
        *inicio = novo;
        return;
    }

    Caranguejo *atual = *inicio;
    while (atual->proximo != NULL) {
        atual = atual->proximo;
    }
    atual->proximo = novo;
}

int removerCaranguejo(Caranguejo **inicio, int id) {
    if (inicio == NULL || *inicio == NULL) {
        return 0;
    }

    Caranguejo *atual    = *inicio;
    Caranguejo *anterior = NULL;

    while (atual != NULL) {
        if (atual->id == id) {
            if (anterior == NULL) {
                *inicio = atual->proximo;
            } else {
                anterior->proximo = atual->proximo;
            }
            free(atual);
            return 1;
        }
        anterior = atual;
        atual    = atual->proximo;
    }
    return 0;
}

void aumentarFome(Caranguejo *inicio) {
    Caranguejo *atual = inicio;
    while (atual != NULL) {
        atual->nivelFome++;
        if (atual->nivelFome > MAX_FOME) {
            atual->nivelFome = MAX_FOME;
        }
        atual = atual->proximo;
    }
}

int verificarMortes(Caranguejo **inicio) {
    if (inicio == NULL || *inicio == NULL) {
        return 0;
    }

    int mortos = 0;
    Caranguejo *atual    = *inicio;
    Caranguejo *anterior = NULL;

    while (atual != NULL) {
        if (atual->nivelFome >= MAX_FOME) {
            Caranguejo *morto = atual;
            if (anterior == NULL) {
                *inicio = atual->proximo;
            } else {
                anterior->proximo = atual->proximo;
            }
            atual = atual->proximo;
            free(morto);
            mortos++;
        } else {
            anterior = atual;
            atual    = atual->proximo;
        }
    }
    return mortos;
}

void ordenarPorFome(Caranguejo **inicio) {
    if (inicio == NULL || *inicio == NULL || (*inicio)->proximo == NULL) {
        return;
    }

    Caranguejo *ordenada = NULL;
    Caranguejo *atual    = *inicio;

    while (atual != NULL) {
        Caranguejo *proximo = atual->proximo;

        if (ordenada == NULL || ordenada->nivelFome < atual->nivelFome) {
            atual->proximo = ordenada;
            ordenada = atual;
        } else {
            Caranguejo *busca = ordenada;
            while (busca->proximo != NULL &&
                   busca->proximo->nivelFome >= atual->nivelFome) {
                busca = busca->proximo;
            }
            atual->proximo = busca->proximo;
            busca->proximo = atual;
        }

        atual = proximo;
    }

    *inicio = ordenada;
}
