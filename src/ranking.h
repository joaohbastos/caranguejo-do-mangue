#ifndef RANKING_H
#define RANKING_H

#define ARQUIVO_PLACAR "ranking.dat"
#define TOP_PLACAR     10

typedef struct Registro {
    char nome[50];
    int pontuacao;
    struct Registro *proximo;
} Registro;

int registrarPlacar(Registro **inicio, const char *nome, int pontuacao);

void ordenarPlacar(Registro **inicio);

int salvarPlacar(Registro *inicio);

int carregarPlacar(Registro **inicio);

void liberarPlacar(Registro **inicio);

#endif
