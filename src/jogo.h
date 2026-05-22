#ifndef JOGO_H
#define JOGO_H

#include "colonia.h"
#include "game_state.h"

#define SPOTS_POR_RODADA 3

typedef struct {
    int faseId;             
    const char *nome;
    int incrementoFome;
    int novosCaranguejos;
    int chanceEvento;      
} ParametrosFase;

ParametrosFase aplicarFaseAtual(int rodada);

void aplicarFomeRodada(GameState *gs, int delta);

void aplicarBonusRei(GameState *gs);

void incrementarRodadasNaColonia(GameState *gs);

int inserirNovoCaranguejo(GameState *gs, int *idAlocado);

Caranguejo *encontrarRei(Caranguejo *inicio);

int proximoIdDisponivel(Caranguejo *inicio);

#endif
