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

static void lerNomeJogador(char *destino, size_t tamanho) {
    if (fgets(destino, (int)tamanho, stdin) == NULL) {
        destino[0] = '\0';
    }
    size_t len = strlen(destino);
    while (len > 0 && (destino[len - 1] == '\n' || destino[len - 1] == '\r')) {
        destino[--len] = '\0';
    }
    if (destino[0] == '\0') {
        strncpy(destino, "Anonimo", tamanho - 1);
        destino[tamanho - 1] = '\0';
    }
}

static void iniciarPartida(Registro **ranking) {
    GameState gs;
    gameStateInicializar(&gs);

    for (int i = 1; i <= CARANGUEJOS_INICIAIS; i++) {
        Caranguejo *novo = criarCaranguejo(i, 0);
        if (novo == NULL) {
            printf("\nFalha ao alocar colonia inicial.\n");
            gameStateLiberar(&gs);
            return;
        }
        novo->nivelFome = i;
        inserirCaranguejo(&gs.colonia, novo);
    }

    printf("\n[Jogar] Partida iniciada com %d caranguejos.\n",
           CARANGUEJOS_INICIAIS);

    int pontuacao = jogarPartidaTerminal(&gs);
    gameStateLiberar(&gs);

    char nome[50];
    printf("\nPartida encerrada. Sua pontuacao: %d rodada(s) sobrevivida(s).\n",
           pontuacao);
    printf("Digite seu nome para o ranking: ");
    lerNomeJogador(nome, sizeof(nome));

    registrarPlacar(ranking, nome, pontuacao);
    ordenarPlacar(ranking);
    renderExibirPlacar(*ranking);
    salvarPlacar(*ranking);
    printf("\n");
}
