#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "screen.h"
#include "theme.h"
#include "../game_state.h"
#include "../ranking.h"

/* Pointer to the player's just-inserted record so we can highlight it.
 * registrarPlacar inserts at head, so we capture the head right after the
 * call; sort/persist may move it within the list but the node address holds. */
static Registro *just_inserted = NULL;

/* Hook chamado por NextScreen(SCREEN_GAMEOVER): registra a pontuacao
 * final, ordena o ranking via Insertion Sort (ordenarPlacar) e
 * persiste em disco. Guarda o ponteiro do registro inserido para
 * que GameOverDraw possa destacar a linha do jogador. */
void GameOverEnter(void) {
    just_inserted = NULL;

    const char *name = (player_name[0] != '\0') ? player_name : "Guardiao";
    if (registrarPlacar(&ranking, name, game.pontuacao)) {
        just_inserted = ranking;          /* head insert → new node is here */
        ordenarPlacar(&ranking);          /* Insertion Sort (ranking.c) */
        salvarPlacar(ranking);            /* writes ranking.dat */
    }
}

/* Tela de Game Over — sem logica por frame. */
void GameOverUpdate(void) { }

/* Renderiza uma linha da tabela do ranking. Se highlight=1, desenha
 * um retangulo laranja por tras para destacar a corrida atual do jogador. */
static void drawRankingRow(int pos, Registro *r, int x, int y, int width, int highlight) {
    int row_h = 28;
    if (highlight) {
        Rectangle hl = (Rectangle){ x - 8, y - 4, width + 16, row_h };
        DrawRectangleRounded(hl, 0.2f, 6, COR_PRIMARIA);
    }

    char pos_buf[8];
    snprintf(pos_buf, sizeof(pos_buf), "%d.", pos);
    gText(pos_buf, x,        y, 20, COR_TEXTO);
    gText(r->nome, x + 50,   y, 20, COR_TEXTO);

    char pts_buf[16];
    snprintf(pts_buf, sizeof(pts_buf), "%d", r->pontuacao);
    int pts_w = gMeasure(pts_buf, 20);
    gText(pts_buf, x + width - pts_w, y, 20, COR_TEXTO);
}

/* Desenha cabecalho "GAME OVER", pontuacao final, tabela top 10 do
 * ranking (com destaque para a corrida atual se estiver no top 10)
 * e os botoes "Jogar novamente" / "Menu principal". */
void GameOverDraw(void) {
    const char *title = "GAME OVER";
    int title_size = 56;
    int tw = gMeasure(title, title_size);
    gText(title, (SCREEN_W - tw) / 2, 30, title_size, COR_PRIMARIA);

    char score_buf[64];
    snprintf(score_buf, sizeof(score_buf), "Pontuacao final: %d", game.pontuacao);
    int sw = gMeasure(score_buf, 28);
    gText(score_buf, (SCREEN_W - sw) / 2, 100, 28, COR_TEXTO);

    /* Ranking table */
    int table_w = 520;
    int table_x = (SCREEN_W - table_w) / 2;
    int table_y = 160;

    const char *header = "TOP 10";
    int hw = gMeasure(header, 22);
    gText(header, (SCREEN_W - hw) / 2, table_y, 22, COR_ACENTO);
    table_y += 36;

    DrawLine(table_x, table_y - 4, table_x + table_w, table_y - 4, COR_ACENTO);

    int pos = 1;
    Registro *r = ranking;
    while (r != NULL && pos <= TOP_PLACAR) {
        int highlight = (r == just_inserted);
        drawRankingRow(pos, r, table_x, table_y, table_w, highlight);
        table_y += 28;
        r = r->proximo;
        pos++;
    }

    if (ranking == NULL) {
        const char *empty = "(ranking vazio)";
        int ew = gMeasure(empty, 18);
        gText(empty, (SCREEN_W - ew) / 2, table_y + 10, 18, COR_TEXTO);
    }

    /* Buttons pinned to bottom */
    int btn_w = 220;
    int btn_h = 56;
    int gap   = 24;
    int total_w = btn_w * 2 + gap;
    int bx      = (SCREEN_W - total_w) / 2;
    int by      = SCREEN_H - btn_h - 30;

    Rectangle r_again = (Rectangle){ bx,                  by, btn_w, btn_h };
    Rectangle r_menu  = (Rectangle){ bx + btn_w + gap,    by, btn_w, btn_h };

    if (Button(r_again, "Jogar novamente", COR_PRIMARIA, COR_PRIMARIA_HOVER, COR_TEXTO)) {
        NextScreen(SCREEN_GAME);   /* GameEnter resets state + colony */
    }
    if (Button(r_menu, "Menu principal", COR_PAINEL_HOVER, COR_ACENTO_HOVER, COR_TEXTO)) {
        NextScreen(SCREEN_MENU);
    }
}
