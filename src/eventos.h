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

/* Sorteia um evento real (MARE, SIRI ou REI). Requer srand previo. */
TipoEvento sortearEvento(void);

/* Aplica o evento MARE: +1 de fome em todos os caranguejos. */
void aplicarMare(GameState *gs);

/* Aplica o efeito do siri quando o jogador escolhe Ignorar:
   ate 2 caranguejos sorteados ganham +2 de fome.
   Retorna ids afetados (na ordem do sorteio) e a quantidade. */
SiriResultado aplicarSiriIgnorado(GameState *gs);

/* Insere um novo Caranguejo Rei na colonia.
   Retorna 1 em sucesso, 0 em falha de alocacao. */
int inserirRei(GameState *gs, int proximoId);

#endif
