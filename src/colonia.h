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

/* Aloca e inicializa um novo caranguejo. Retorna NULL em falha de alocacao. */
Caranguejo *criarCaranguejo(int id, int ehRei);

/* Insere o novo caranguejo no final da lista da colonia. */
void inserirCaranguejo(Caranguejo **inicio, Caranguejo *novo);

/* Remove e libera o caranguejo cujo id corresponde. Retorna 1 se removeu, 0 caso contrario. */
int removerCaranguejo(Caranguejo **inicio, int id);

/* Incrementa o nivel de fome de cada caranguejo da colonia em 1, respeitando MAX_FOME. */
void aumentarFome(Caranguejo *inicio);

/* Remove e libera os caranguejos com nivelFome == MAX_FOME. Retorna a quantidade de mortes. */
int verificarMortes(Caranguejo **inicio);

/* Ordena a lista por nivelFome decrescente (mais faminto primeiro) usando Insertion Sort. */
void ordenarPorFome(Caranguejo **inicio);

#endif
