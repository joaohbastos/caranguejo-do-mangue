#ifndef RENDER_TERMINAL_H
#define RENDER_TERMINAL_H

#include "colonia.h"
#include "game_state.h"
#include "ranking.h"

/* Exibe no terminal o estado atual da colonia em formato de tabela. */
void renderExibirColonia(Caranguejo *inicio);

/* Exibe no terminal o ranking dos melhores jogadores. */
void renderExibirPlacar(Registro *inicio);

/* Roda uma partida completa no terminal sobre o GameState fornecido.
   Espera que gs->colonia ja contenha os caranguejos iniciais.
   Atualiza gs->rodadaAtual, gs->fase, gs->pontuacao etc. durante o jogo.
   Retorna o numero de rodadas completamente sobrevividas (= pontuacao). */
int jogarPartidaTerminal(GameState *gs);

#endif
