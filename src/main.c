#include <stdio.h>
#include <stdlib.h>

#include "colonia.h"
#include "eventos.h"
#include "jogo.h"
#include "ranking.h"

#define OP_JOGAR  1
#define OP_CONFIG 2
#define OP_SOBRE  3
#define OP_SAIR   4

static void exibirMenu(void);
static int  lerOpcao(void);
static void descartarLinha(void);

/* TEMPORARIO - sera removido apos confirmacao do Stage 2 */
static void testeListaColonia(void);
static void liberarColonia(Caranguejo **inicio);

int main(void) {
    int opcao = 0;

    testeListaColonia();

    do {
        exibirMenu();
        opcao = lerOpcao();

        switch (opcao) {
            case OP_JOGAR:
                printf("\n[Jogar] Em construcao.\n\n");
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

/* ------------------------------------------------------------------
   TESTE TEMPORARIO - Stage 3
   Verifica aumentarFome, verificarMortes e ordenarPorFome.
   ------------------------------------------------------------------ */
static void testeListaColonia(void) {
    Caranguejo *colonia = NULL;

    printf("\n==== TESTE - Lista da Colonia (Stage 3) ====\n\n");

    Caranguejo *c1 = criarCaranguejo(1, 0); c1->nivelFome = 1;
    Caranguejo *c2 = criarCaranguejo(2, 0); c2->nivelFome = 4;
    Caranguejo *c3 = criarCaranguejo(3, 1); c3->nivelFome = 2;
    Caranguejo *c4 = criarCaranguejo(4, 0); c4->nivelFome = 3;

    inserirCaranguejo(&colonia, c1);
    inserirCaranguejo(&colonia, c2);
    inserirCaranguejo(&colonia, c3);
    inserirCaranguejo(&colonia, c4);

    printf("Estado inicial (insercao na ordem 1, 2, 3-Rei, 4, fomes 1/4/2/3):\n");
    exibirColonia(colonia);

    ordenarPorFome(&colonia);
    printf("\nApos ordenarPorFome (decrescente):\n");
    exibirColonia(colonia);

    aumentarFome(colonia);
    aumentarFome(colonia);
    printf("\nApos aumentarFome chamado 2 vezes:\n");
    exibirColonia(colonia);

    int mortos = verificarMortes(&colonia);
    printf("\nverificarMortes removeu %d caranguejo(s).\n", mortos);
    printf("Colonia restante:\n");
    exibirColonia(colonia);

    liberarColonia(&colonia);

    printf("\n==== Fim do teste ====\n\n");
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
