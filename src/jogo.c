#include <stdlib.h>
#include "colonia.h"
#include "eventos.h"
#include "game_state.h"
#include "jogo.h"

ParametrosFase aplicarFaseAtual(int rodada) {
    ParametrosFase p;
    if (rodada <= 5) {
        p.faseId           = 1;
        p.nome             = "Mare Baixa";
        p.incrementoFome   = 1;
        p.novosCaranguejos = 1;
        p.chanceEvento     = 30;
    } else if (rodada <= 10) {
        p.faseId           = 2;
        p.nome             = "Mare Cheia";
        p.incrementoFome   = 2;
        p.novosCaranguejos = 2;
        p.chanceEvento     = 60;
    } else {
        p.faseId           = 3;
        p.nome             = "Tempestade no Capibaribe";
        p.incrementoFome   = 3;
        p.novosCaranguejos = 3;
        p.chanceEvento     = 100;
    }
    return p;
}

int proximoIdDisponivel(Caranguejo *inicio) {
    int max = 0;
    for (Caranguejo *a = inicio; a != NULL; a = a->proximo) {
        if (a->id > max) {
            max = a->id;
        }
    }
    return max + 1;
}

Caranguejo *encontrarRei(Caranguejo *inicio) {
    for (Caranguejo *a = inicio; a != NULL; a = a->proximo) {
        if (a->ehRei) {
            return a;
        }
    }
    return NULL;
}

void aplicarFomeRodada(GameState *gs, int delta) {
    if (gs == NULL) {
        return;
    }
    for (Caranguejo *a = gs->colonia; a != NULL; a = a->proximo) {
        int inc = a->ehRei ? delta * 2 : delta;
        a->nivelFome += inc;
        if (a->nivelFome > MAX_FOME) {
            a->nivelFome = MAX_FOME;
        }
    }
}

void aplicarBonusRei(GameState *gs) {
    if (gs == NULL) {
        return;
    }
    for (Caranguejo *a = gs->colonia; a != NULL; a = a->proximo) {
        a->nivelFome -= 2;
        if (a->nivelFome < 0) {
            a->nivelFome = 0;
        }
    }
}

void incrementarRodadasNaColonia(GameState *gs) {
    if (gs == NULL) {
        return;
    }
    for (Caranguejo *a = gs->colonia; a != NULL; a = a->proximo) {
        a->rodadasNaColonia++;
    }
}

int inserirNovoCaranguejo(GameState *gs, int *idAlocado) {
    if (gs == NULL) {
        return 0;
    }
    int id = proximoIdDisponivel(gs->colonia);
    Caranguejo *novo = criarCaranguejo(id, 0);
    if (novo == NULL) {
        return 0;
    }
    inserirCaranguejo(&gs->colonia, novo);
    if (idAlocado != NULL) {
        *idAlocado = id;
    }
    return 1;
}
