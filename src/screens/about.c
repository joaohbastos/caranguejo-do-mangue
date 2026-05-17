#include "raylib.h"
#include "screen.h"
#include "theme.h"

/* Tela "Sobre" — sem logica por frame. */
void AboutUpdate(void) { }

/* Renderiza titulo, contexto cultural do manguezal, instrucoes
 * "Como jogar" e creditos da equipe. Botao "Voltar" leva ao menu. */
void AboutDraw(void) {
    const char *title = "SOBRE O JOGO";
    int title_size = 40;
    int tw = gMeasure(title, title_size);
    gText(title, (SCREEN_W - tw) / 2, 40, title_size, COR_PRIMARIA);

    int x = 80;
    int y = 110;

    gText("Guardioes do Mangue", x, y, 28, COR_ACENTO);
    y += 44;

    /* One-paragraph mangrove / Capibaribe context, broken into lines that
     * fit comfortably in 1000 px at font size 18. */
    const char *paragraph[] = {
        "Voce e o guardiao de uma colonia de caranguejos-uca nos manguezais",
        "do Rio Capibaribe, em Recife. As mares trazem comida e perigos: gerencie",
        "a fome, atenda o Caranguejo Rei e mantenha sua colonia viva o maximo de",
        "rodadas possivel diante das tempestades e da poluicao do mangue."
    };
    for (int i = 0; i < 4; i++) {
        gText(paragraph[i], x, y, 18, COR_TEXTO);
        y += 26;
    }
    y += 16;

    gText("Como jogar", x, y, 24, COR_ACENTO);
    y += 36;
    const char *how[] = {
        "1. A cada rodada voce tem 3 spots para alimentar caranguejos.",
        "2. Alimente primeiro quem tem mais fome — a fome chega ao maximo, ele morre.",
        "3. Eventos do mangue (Mare, Siri, Rei) mudam o ritmo da rodada.",
        "4. Se um caranguejo morre, a partida acaba e seu placar entra no ranking."
    };
    for (int i = 0; i < 4; i++) {
        gText(how[i], x, y, 18, COR_TEXTO);
        y += 26;
    }
    y += 16;

    gText("Equipe", x, y, 24, COR_ACENTO);
    y += 36;
    const char *team[] = {
        "Arthur Sindeaux",
        "Arthur Oliveira",
        "Lucas Pinto",
        "Joao Bastos"
    };
    for (int i = 0; i < 4; i++) {
        gText(team[i], x, y, 18, COR_TEXTO);
        y += 24;
    }

    Rectangle r_back = (Rectangle){ SCREEN_W - 200 - 40, SCREEN_H - 60 - 30, 200, 60 };
    if (Button(r_back, "Voltar", COR_PAINEL, COR_PAINEL_HOVER, COR_TEXTO)) {
        NextScreen(SCREEN_MENU);
    }
}
