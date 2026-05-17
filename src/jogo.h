#ifndef JOGO_H
#define JOGO_H

#include "colonia.h"
#include "game_state.h"

#define SPOTS_POR_RODADA 3

typedef struct {
    int faseId;              /* 1, 2 ou 3 */
    const char *nome;
    int incrementoFome;
    int novosCaranguejos;
    int chanceEvento;        /* 0-100 */
} ParametrosFase;

/* Retorna os parametros da fase correspondente ao numero da rodada. */
ParametrosFase aplicarFaseAtual(int rodada);

/* Reduz nivelFome do caranguejo de id informado em 2 (minimo 0).
   Retorna 1 se encontrou e alimentou, 0 caso contrario. */
int alimentarCaranguejo(Caranguejo *inicio, int id);

/* Aumenta a fome de cada caranguejo segundo o incremento da fase.
   O Caranguejo Rei sofre o dobro. */
void aplicarFomeRodada(GameState *gs, int delta);

/* Aplica o bonus do Rei: -2 de fome para todos (clampeado a 0). */
void aplicarBonusRei(GameState *gs);

/* Incrementa rodadasNaColonia de cada caranguejo. */
void incrementarRodadasNaColonia(GameState *gs);

/* Cria e insere um novo caranguejo comum com o proximo id disponivel.
   Em sucesso retorna 1 e armazena o id atribuido em *idAlocado.
   Em falha de alocacao retorna 0. */
int inserirNovoCaranguejo(GameState *gs, int *idAlocado);

/* Retorna o ponteiro para o Rei na colonia, ou NULL se nao houver. */
Caranguejo *encontrarRei(Caranguejo *inicio);

/* Retorna o proximo id livre na colonia (max(id) + 1, ou 1 se vazia). */
int proximoIdDisponivel(Caranguejo *inicio);

#endif
