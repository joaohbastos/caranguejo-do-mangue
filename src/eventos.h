#ifndef EVENTOS_H
#define EVENTOS_H

#include "colonia.h"

#define REI_RODADAS_PARA_BONUS 5

typedef enum {
    EVENTO_NENHUM = 0,
    EVENTO_MARE   = 1,
    EVENTO_SIRI   = 2,
    EVENTO_REI    = 3
} TipoEvento;

/* Sorteia um evento real (MARE, SIRI ou REI). Requer srand previo. */
TipoEvento sortearEvento(void);

/* Apresenta o evento ao jogador e aplica as consequencias.
   Para EVENTO_REI usa proximoId como id do novo Caranguejo Rei.
   Retorna 1 se o Rei foi aceito (id consumido), 0 caso contrario. */
int aplicarEscolha(Caranguejo **inicio, TipoEvento evento, int proximoId);

/* Pergunta ao jogador se aceita o Caranguejo Rei e o insere se aceito.
   Retorna 1 se aceito, 0 caso contrario. */
int gerenciarRei(Caranguejo **inicio, int proximoId);

#endif
