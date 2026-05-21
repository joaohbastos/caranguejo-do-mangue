#ifndef RENDER_TERMINAL_H
#define RENDER_TERMINAL_H

#include "colonia.h"
#include "game_state.h"
#include "ranking.h"

void renderExibirColonia(Caranguejo *inicio);

void renderExibirPlacar(Registro *inicio);

int jogarPartidaTerminal(GameState *gs);

#endif
