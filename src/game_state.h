#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "colonia.h"

#define POLUICAO_MAX 10

typedef struct {
    Caranguejo *colonia;
    int rodadaAtual;
    int fase;
    int poluicao;
    int spotsRestantes;
    int pontuacao;
    int streak;
    int reiAtivo;
    int reiRodadasVivo;
    int siriAtivo;
} GameState;

void gameStateInicializar(GameState *gs);
void gameStateLiberar(GameState *gs);

#endif
