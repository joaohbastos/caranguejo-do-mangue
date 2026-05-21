#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "colonia.h"
#include "game_state.h"
#include "ranking.h"
#include "render_terminal.h"

#define OP_JOGAR  1
#define OP_CONFIG 2
#define OP_SOBRE  3
#define OP_SAIR   4

#define CARANGUEJOS_INICIAIS 4

static void exibirMenu(void);
static int  lerOpcao(void);
static void descartarLinha(void);
static void iniciarPartida(Registro **ranking);
static void lerNomeJogador(char *destino, size_t tamanho);

int main(void) {
    int opcao = 0;
    Registro *ranking = NULL;

    srand((unsigned)time(NULL));

    carregarPlacar(&ranking);

    do {
        exibirMenu();
        opcao = lerOpcao();

        switch (opcao) {
            case OP_JOGAR:
                iniciarPartida(&ranking);
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

    liberarPlacar(&ranking);
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



