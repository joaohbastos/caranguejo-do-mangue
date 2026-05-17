#include <stdlib.h>

#include "game_state.h"

void gameStateInicializar(GameState *gs) {
    if (gs == NULL) {
        return;
    }
    gs->colonia        = NULL;
    gs->rodadaAtual    = 0;
    gs->fase           = 0;
    gs->poluicao       = 0;
    gs->spotsRestantes = 0;
    gs->pontuacao      = 0;
    gs->streak         = 0;
    gs->reiAtivo       = 0;
    gs->reiRodadasVivo = 0;
    gs->siriAtivo      = 0;
}

void gameStateLiberar(GameState *gs) {
    if (gs == NULL) {
        return;
    }
    Caranguejo *atual = gs->colonia;
    while (atual != NULL) {
        Caranguejo *prox = atual->proximo;
        free(atual);
        atual = prox;
    }
    gs->colonia = NULL;
}
