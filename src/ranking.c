#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ranking.h"

int registrarPlacar(Registro **inicio, const char *nome, int pontuacao) {
    if (inicio == NULL || nome == NULL) {
        return 0;
    }
    Registro *novo = (Registro *)malloc(sizeof(Registro));
    if (novo == NULL) {
        return 0;
    }
    strncpy(novo->nome, nome, sizeof(novo->nome) - 1);
    novo->nome[sizeof(novo->nome) - 1] = '\0';
    novo->pontuacao = pontuacao;
    novo->proximo   = *inicio;
    *inicio = novo;
    return 1;
}

void ordenarPlacar(Registro **inicio) {
    if (inicio == NULL || *inicio == NULL || (*inicio)->proximo == NULL) {
        return;
    }

    Registro *ordenada = NULL;
    Registro *atual    = *inicio;

    while (atual != NULL) {
        Registro *proxima = atual->proximo;

        if (ordenada == NULL || ordenada->pontuacao < atual->pontuacao) {
            atual->proximo = ordenada;
            ordenada = atual;
        } else {
            Registro *p = ordenada;
            while (p->proximo != NULL && p->proximo->pontuacao >= atual->pontuacao) {
                p = p->proximo;
            }
            atual->proximo = p->proximo;
            p->proximo = atual;
        }
        atual = proxima;
    }

    *inicio = ordenada;
}

int salvarPlacar(Registro *inicio) {
    FILE *f = fopen(ARQUIVO_PLACAR, "w");
    if (f == NULL) {
        return 0;
    }
    for (Registro *r = inicio; r != NULL; r = r->proximo) {
        fprintf(f, "%s;%d\n", r->nome, r->pontuacao);
    }
    fclose(f);
    return 1;
}

int carregarPlacar(Registro **inicio) {
    if (inicio == NULL) {
        return 0;
    }
    FILE *f = fopen(ARQUIVO_PLACAR, "r");
    if (f == NULL) {
        return 0;
    }

    char linha[128];
    while (fgets(linha, sizeof(linha), f) != NULL) {
        char *sep = strrchr(linha, ';');
        if (sep == NULL) continue;
        *sep = '\0';
        int pontuacao = atoi(sep + 1);
        registrarPlacar(inicio, linha, pontuacao);
    }
    fclose(f);
    ordenarPlacar(inicio);
    return 1;
}

void liberarPlacar(Registro **inicio) {
    if (inicio == NULL) {
        return;
    }
    Registro *atual = *inicio;
    while (atual != NULL) {
        Registro *prox = atual->proximo;
        free(atual);
        atual = prox;
    }
    *inicio = NULL;
}
