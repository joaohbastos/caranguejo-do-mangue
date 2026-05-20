#ifndef COLONIA_H
#define COLONIA_H

#define MAX_FOME 5

typedef struct Caranguejo {
    int id;
    int nivelFome;
    int rodadasNaColonia;
    int ehRei;
    struct Caranguejo *proximo;
} Caranguejo;

Caranguejo *criarCaranguejo(int id, int ehRei);

void inserirCaranguejo(Caranguejo **inicio, Caranguejo *novo);

int removerCaranguejo(Caranguejo **inicio, int id);

void aumentarFome(Caranguejo *inicio);

int verificarMortes(Caranguejo **inicio);

void ordenarPorFome(Caranguejo **inicio);

#endif
