#ifndef JOGO_H
#define JOGO_H

#include "colonia.h"

#define SPOTS_POR_RODADA 3

typedef struct {
    int faseId;              /* 1, 2 ou 3 */
    const char *nome;
    int incrementoFome;
    int novosCaranguejos;
    int chanceEvento;        /* 0-100 */
} ParametrosFase;

/* Retorna os parametros da fase correspondente ao numero da rodada. */
ParametrosFase aplicarFaseAtual(int rodada);

/* Reduz nivelFome do caranguejo de id informado em 2 (minimo 0).
   Retorna 1 se encontrou e alimentou, 0 caso contrario. */
int alimentarCaranguejo(Caranguejo *inicio, int id);

/* Loop principal de uma partida. Roda multiplas rodadas ate game over. */
void jogarPartida(Caranguejo **inicio);

#endif
