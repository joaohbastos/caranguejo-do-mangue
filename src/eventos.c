#include <stdlib.h>

#include "colonia.h"
#include "eventos.h"
#include "game_state.h"

#define EVENTOS_REAIS 3

static int contarColonia(Caranguejo *inicio) {
    int n = 0;
    for (Caranguejo *a = inicio; a != NULL; a = a->proximo) {
        n++;
    }
    return n;
}

static Caranguejo *caranguejoNoIndice(Caranguejo *inicio, int indice) {
    Caranguejo *a = inicio;
    while (a != NULL && indice > 0) {
        a = a->proximo;
        indice--;
    }
    return a;
}

static void somarFome(Caranguejo *alvo, int delta) {
    if (alvo == NULL) {
        return;
    }
    alvo->nivelFome += delta;
    if (alvo->nivelFome > MAX_FOME) {
        alvo->nivelFome = MAX_FOME;
    }
    if (alvo->nivelFome < 0) {
        alvo->nivelFome = 0;
    }
}

TipoEvento sortearEvento(void) {
    return (TipoEvento)(EVENTO_MARE + (rand() % EVENTOS_REAIS));
}

void aplicarMare(GameState *gs) {
    if (gs == NULL) {
        return;
    }
    for (Caranguejo *a = gs->colonia; a != NULL; a = a->proximo) {
        somarFome(a, 1);
    }
}

SiriResultado aplicarSiriIgnorado(GameState *gs) {
    SiriResultado r;
    r.ids[0] = 0;
    r.ids[1] = 0;
    r.n      = 0;

    if (gs == NULL) {
        return r;
    }

    int n = contarColonia(gs->colonia);
    if (n == 0) {
        return r;
    }

    int alvos = n < 2 ? n : 2;
    for (int i = 0; i < alvos; i++) {
        int idx = rand() % n;
        Caranguejo *vitima = caranguejoNoIndice(gs->colonia, idx);
        if (vitima != NULL) {
            r.ids[r.n++] = vitima->id;
            somarFome(vitima, 1);
        }
    }
    return r;
}

int inserirRei(GameState *gs, int proximoId) {
    if (gs == NULL) {
        return 0;
    }
    Caranguejo *novo = criarCaranguejo(proximoId, 1, 0);
    if (novo == NULL) {
        return 0;
    }
    inserirCaranguejo(&gs->colonia, novo);
    return 1;
}
