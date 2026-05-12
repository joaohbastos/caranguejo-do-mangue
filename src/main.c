#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "colonia.h"
#include "eventos.h"
#include "jogo.h"
#include "ranking.h"

#define OP_JOGAR  1
#define OP_CONFIG 2
#define OP_SOBRE  3
#define OP_SAIR   4

#define CARANGUEJOS_INICIAIS 4

static void exibirMenu(void);
static int  lerOpcao(void);
static void descartarLinha(void);
static void iniciarPartida(void);
static void liberarColonia(Caranguejo **inicio);

int main(void) {
    int opcao = 0;

    srand((unsigned)time(NULL));

    do {
        exibirMenu();
        opcao = lerOpcao();

        switch (opcao) {
            case OP_JOGAR:
                iniciarPartida();
                break;
            case OP_CONFIG:
                printf("\n[Configuracoes] Em construcao.\n\n");
                break;
            case OP_SOBRE:
                printf("\n[Sobre] Em construcao.\n\n");
                break;
            case OP_SAIR:
                printf("\nAte logo!\n");
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n\n");
        }
    } while (opcao != OP_SAIR);

    return 0;
}

static void exibirMenu(void) {
    printf("=========================================\n");
    printf("       GUARDIOES DO MANGUE\n");
    printf("   Manguezal do Rio Capibaribe - Recife\n");
    printf("=========================================\n");
    printf("  1. Jogar\n");
    printf("  2. Configuracoes\n");
    printf("  3. Sobre\n");
    printf("  4. Sair\n");
    printf("-----------------------------------------\n");
    printf("Escolha uma opcao: ");
}

static int lerOpcao(void) {
    int op;
    if (scanf("%d", &op) != 1) {
        descartarLinha();
        return -1;
    }
    descartarLinha();
    return op;
}

static void descartarLinha(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static void iniciarPartida(void) {
    Caranguejo *colonia = NULL;

    for (int i = 1; i <= CARANGUEJOS_INICIAIS; i++) {
        Caranguejo *novo = criarCaranguejo(i, 0);
        if (novo == NULL) {
            printf("\nFalha ao alocar colonia inicial.\n");
            liberarColonia(&colonia);
            return;
        }
        novo->nivelFome = i;
        inserirCaranguejo(&colonia, novo);
    }

    printf("\n[Jogar] Partida iniciada com %d caranguejos.\n",
           CARANGUEJOS_INICIAIS);

    jogarPartida(&colonia);

    liberarColonia(&colonia);
    printf("\n");
}

static void liberarColonia(Caranguejo **inicio) {
    Caranguejo *atual = *inicio;
    while (atual != NULL) {
        Caranguejo *prox = atual->proximo;
        free(atual);
        atual = prox;
    }
    *inicio = NULL;
}
