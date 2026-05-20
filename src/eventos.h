#ifndef EVENTOS_H
#define EVENTOS_H

#include "colonia.h"
#include "game_state.h"

#define REI_RODADAS_PARA_BONUS 5

typedef enum {
    EVENTO_NENHUM = 0,
    EVENTO_MARE   = 1,
    EVENTO_SIRI   = 2,
    EVENTO_REI    = 3
} TipoEvento;

typedef struct {
    int ids[2];
    int n;
} SiriResultado;

TipoEvento sortearEvento(void);

void aplicarMare(GameState *gs);

SiriResultado aplicarSiriIgnorado(GameState *gs);

int inserirRei(GameState *gs, int proximoId);

#endif
