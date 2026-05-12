#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ranking.h"

void registrarPlacar(Registro **inicio, const char *nome, int pontuacao) {
    if (inicio == NULL || nome == NULL) {
        return;
    }
    Registro *novo = (Registro *)malloc(sizeof(Registro));
    if (novo == NULL) {
        printf("Falha ao alocar registro do ranking.\n");
        return;
    }
    strncpy(novo->nome, nome, sizeof(novo->nome) - 1);
    novo->nome[sizeof(novo->nome) - 1] = '\0';
    novo->pontuacao = pontuacao;
    novo->proximo   = *inicio;
    *inicio = novo;
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

void exibirPlacar(Registro *inicio) {
    printf("\n================ TOP %d - RANKING ================\n", TOP_PLACAR);
    if (inicio == NULL) {
        printf("  (ranking vazio)\n");
        printf("==================================================\n");
        return;
    }
    int pos = 1;
    for (Registro *r = inicio; r != NULL && pos <= TOP_PLACAR; r = r->proximo, pos++) {
        printf("  %2d. %-40s %5d\n", pos, r->nome, r->pontuacao);
    }
    printf("==================================================\n");
}

void salvarPlacar(Registro *inicio) {
    FILE *f = fopen(ARQUIVO_PLACAR, "w");
    if (f == NULL) {
        printf("Falha ao abrir %s para escrita.\n", ARQUIVO_PLACAR);
        return;
    }
    for (Registro *r = inicio; r != NULL; r = r->proximo) {
        fprintf(f, "%d %s\n", r->pontuacao, r->nome);
    }
    fclose(f);
}

void carregarPlacar(Registro **inicio) {
    if (inicio == NULL) {
        return;
    }
    FILE *f = fopen(ARQUIVO_PLACAR, "r");
    if (f == NULL) {
        return; /* primeira execucao: sem arquivo ainda */
    }

    char linha[128];
    while (fgets(linha, sizeof(linha), f) != NULL) {
        int pontuacao;
        char nome[50];
        if (sscanf(linha, "%d %49[^\n]", &pontuacao, nome) == 2) {
            registrarPlacar(inicio, nome, pontuacao);
        }
    }
    fclose(f);
    ordenarPlacar(inicio);
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
