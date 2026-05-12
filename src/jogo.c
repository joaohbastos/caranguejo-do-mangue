#include <stdio.h>
#include <stdlib.h>

#include "colonia.h"
#include "eventos.h"
#include "jogo.h"

static void descartarLinhaJogo(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static int lerInteiro(int *destino) {
    if (scanf("%d", destino) != 1) {
        descartarLinhaJogo();
        return 0;
    }
    descartarLinhaJogo();
    return 1;
}

int alimentarCaranguejo(Caranguejo *inicio, int id) {
    Caranguejo *atual = inicio;
    while (atual != NULL) {
        if (atual->id == id) {
            atual->nivelFome -= 2;
            if (atual->nivelFome < 0) {
                atual->nivelFome = 0;
            }
            return 1;
        }
        atual = atual->proximo;
    }
    return 0;
}

ParametrosFase aplicarFaseAtual(int rodada) {
    ParametrosFase p;
    if (rodada <= 5) {
        p.faseId           = 1;
        p.nome             = "Mare Baixa";
        p.incrementoFome   = 1;
        p.novosCaranguejos = 1;
        p.chanceEvento     = 33;
    } else if (rodada <= 10) {
        p.faseId           = 2;
        p.nome             = "Mare Cheia";
        p.incrementoFome   = 2;
        p.novosCaranguejos = 2;
        p.chanceEvento     = 66;
    } else {
        p.faseId           = 3;
        p.nome             = "Tempestade no Capibaribe";
        p.incrementoFome   = 3;
        p.novosCaranguejos = 3;
        p.chanceEvento     = 100;
    }
    return p;
}

static int proximoIdDisponivel(Caranguejo *inicio) {
    int max = 0;
    for (Caranguejo *a = inicio; a != NULL; a = a->proximo) {
        if (a->id > max) {
            max = a->id;
        }
    }
    return max + 1;
}

static Caranguejo *encontrarRei(Caranguejo *inicio) {
    for (Caranguejo *a = inicio; a != NULL; a = a->proximo) {
        if (a->ehRei) {
            return a;
        }
    }
    return NULL;
}

/* Aumenta a fome de cada caranguejo segundo o incremento da fase.
   O Caranguejo Rei sofre o dobro. */
static void aplicarFomeRodada(Caranguejo *inicio, int delta) {
    for (Caranguejo *a = inicio; a != NULL; a = a->proximo) {
        int inc = a->ehRei ? delta * 2 : delta;
        a->nivelFome += inc;
        if (a->nivelFome > MAX_FOME) {
            a->nivelFome = MAX_FOME;
        }
    }
}

static void aplicarBonusRei(Caranguejo *inicio) {
    printf("\n[Caranguejo Rei] Sobreviveu %d rodadas! Bonus para a colonia: -2 de fome em todos.\n",
           REI_RODADAS_PARA_BONUS);
    for (Caranguejo *a = inicio; a != NULL; a = a->proximo) {
        a->nivelFome -= 2;
        if (a->nivelFome < 0) {
            a->nivelFome = 0;
        }
    }
}

static void incrementarRodadasNaColonia(Caranguejo *inicio) {
    for (Caranguejo *a = inicio; a != NULL; a = a->proximo) {
        a->rodadasNaColonia++;
    }
}

static void inserirNovosCaranguejos(Caranguejo **inicio, int *proximoId, int quantos) {
    if (quantos <= 0) {
        return;
    }
    printf("\nChegam %d novo(s) caranguejo(s) ao mangue:\n", quantos);
    for (int i = 0; i < quantos; i++) {
        Caranguejo *novo = criarCaranguejo(*proximoId, 0);
        if (novo == NULL) {
            printf("  Falha ao alocar novo caranguejo.\n");
            return;
        }
        inserirCaranguejo(inicio, novo);
        printf("  + Caranguejo id=%d entrou.\n", *proximoId);
        (*proximoId)++;
    }
}

static int rodadaUnica(Caranguejo **inicio, int numeroRodada, ParametrosFase fase,
                       int *proximoId, int *reiBonusDado) {
    ordenarPorFome(inicio);

    printf("\n=== Rodada %d - %s ===\n", numeroRodada, fase.nome);
    printf("Fome/rodada: +%d | Novos caranguejos: %d | Chance de evento: %d%%\n",
           fase.incrementoFome, fase.novosCaranguejos, fase.chanceEvento);
    printf("Estado da colonia (mais faminto primeiro):\n");
    exibirColonia(*inicio);

    for (int spot = 1; spot <= SPOTS_POR_RODADA; spot++) {
        int id = 0;
        printf("\nSpot %d/%d - id do caranguejo a alimentar (0 para pular): ",
               spot, SPOTS_POR_RODADA);
        if (!lerInteiro(&id)) {
            printf("Entrada invalida. Spot perdido.\n");
            continue;
        }
        if (id == 0) {
            printf("Spot pulado.\n");
            continue;
        }
        if (!alimentarCaranguejo(*inicio, id)) {
            printf("Caranguejo id=%d nao encontrado. Spot perdido.\n", id);
        } else {
            printf("Caranguejo id=%d alimentado.\n", id);
        }
    }

    if (rand() % 100 < fase.chanceEvento) {
        TipoEvento evento = sortearEvento();
        int reiAceito = aplicarEscolha(inicio, evento, *proximoId);
        if (reiAceito) {
            (*proximoId)++;
        }
    } else {
        aplicarEscolha(inicio, EVENTO_NENHUM, 0);
    }

    aplicarFomeRodada(*inicio, fase.incrementoFome);
    printf("\nApos aumentarFome:\n");
    exibirColonia(*inicio);

    int mortos = verificarMortes(inicio);
    if (mortos > 0) {
        printf("\nA crab died -- game over\n");
        return 0;
    }

    incrementarRodadasNaColonia(*inicio);

    Caranguejo *rei = encontrarRei(*inicio);
    if (rei != NULL && !*reiBonusDado &&
        rei->rodadasNaColonia >= REI_RODADAS_PARA_BONUS) {
        aplicarBonusRei(*inicio);
        *reiBonusDado = 1;
    }

    inserirNovosCaranguejos(inicio, proximoId, fase.novosCaranguejos);
    return 1;
}

void jogarPartida(Caranguejo **inicio) {
    if (inicio == NULL || *inicio == NULL) {
        printf("\nColonia vazia. Nada a jogar.\n");
        return;
    }

    int proximoId    = proximoIdDisponivel(*inicio);
    int reiBonusDado = 0;
    int faseAnterior = 0;

    for (int n = 1; ; n++) {
        ParametrosFase fase = aplicarFaseAtual(n);
        if (fase.faseId != faseAnterior) {
            if (faseAnterior == 0) {
                printf("\n>>> Inicio da fase: %s <<<\n", fase.nome);
            } else {
                printf("\n>>> Transicao de fase: agora %s! <<<\n", fase.nome);
            }
            faseAnterior = fase.faseId;
        }

        int sobreviveu = rodadaUnica(inicio, n, fase, &proximoId, &reiBonusDado);
        if (!sobreviveu) {
            printf("\nFim de jogo na rodada %d.\n", n);
            return;
        }
    }
}
