#include <stdio.h>
#include <stdlib.h>

#include "colonia.h"
#include "eventos.h"

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

static void descartarLinhaEventos(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

TipoEvento sortearEvento(void) {
    return (TipoEvento)(EVENTO_MARE + (rand() % EVENTOS_REAIS));
}

static void aplicarMare(Caranguejo *inicio) {
    printf("\n[Evento] MARE alta no Capibaribe!\n");
    printf("As aguas sobem e a fome de todos sobe junto.\n");
    for (Caranguejo *a = inicio; a != NULL; a = a->proximo) {
        somarFome(a, 1);
    }
}

static void aplicarSiri(Caranguejo *inicio) {
    printf("\n[Evento] SIRI INVASOR aparece no mangue!\n");
    printf("  1. Cacar (gasta 1 spot extra desta rodada)\n");
    printf("  2. Ignorar (2 caranguejos aleatorios ganham +2 de fome)\n");
    printf("Escolha: ");

    int escolha = 0;
    if (scanf("%d", &escolha) != 1) {
        descartarLinhaEventos();
        escolha = 2;
        printf("Entrada invalida. Assumindo Ignorar.\n");
    } else {
        descartarLinhaEventos();
    }

    if (escolha == 1) {
        printf("Voce cacou o siri. A colonia segue em paz.\n");
        return;
    }

    int n = contarColonia(inicio);
    if (n == 0) {
        printf("Nao ha caranguejos para sofrer com o siri.\n");
        return;
    }

    int alvos = n < 2 ? n : 2;
    for (int i = 0; i < alvos; i++) {
        int idx = rand() % n;
        Caranguejo *vitima = caranguejoNoIndice(inicio, idx);
        if (vitima != NULL) {
            printf("  -> Caranguejo id=%d ganhou +2 de fome.\n", vitima->id);
            somarFome(vitima, 2);
        }
    }
}

static void aplicarNenhum(void) {
    printf("\n[Evento] Mangue calmo. Nada de novo nesta rodada.\n");
}

int gerenciarRei(Caranguejo **inicio, int proximoId) {
    printf("\n[Evento] CARANGUEJO REI aparece no mangue!\n");
    printf("  1. Aceitar (entra na colonia, fome cresce 2x mais rapido;\n");
    printf("              bonus -2 de fome para todos se sobreviver %d rodadas)\n",
           REI_RODADAS_PARA_BONUS);
    printf("  2. Rejeitar (nada acontece)\n");
    printf("Escolha: ");

    int escolha = 0;
    if (scanf("%d", &escolha) != 1) {
        descartarLinhaEventos();
        printf("Entrada invalida. Assumindo Rejeitar.\n");
        return 0;
    }
    descartarLinhaEventos();

    if (escolha != 1) {
        printf("O Rei segue seu caminho. Nada acontece.\n");
        return 0;
    }

    Caranguejo *novo = criarCaranguejo(proximoId, 1);
    if (novo == NULL) {
        printf("Falha ao alocar o Rei.\n");
        return 0;
    }
    inserirCaranguejo(inicio, novo);
    printf("Caranguejo Rei id=%d entrou na colonia.\n", proximoId);
    return 1;
}

int aplicarEscolha(Caranguejo **inicio, TipoEvento evento, int proximoId) {
    if (inicio == NULL) {
        return 0;
    }
    switch (evento) {
        case EVENTO_MARE:
            aplicarMare(*inicio);
            return 0;
        case EVENTO_SIRI:
            aplicarSiri(*inicio);
            return 0;
        case EVENTO_REI:
            return gerenciarRei(inicio, proximoId);
        case EVENTO_NENHUM:
        default:
            aplicarNenhum();
            return 0;
    }
}
