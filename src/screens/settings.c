#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "screen.h"
#include "theme.h"
#include "../ranking.h"

static int confirm_open = 0;

void SettingsUpdate(void) {
    int ch = GetCharPressed();
    while (ch > 0) {
        size_t len = strlen(player_name);
        if (ch >= 32 && ch < 127 && len < PLAYER_NAME_MAX - 1) {
            player_name[len]     = (char)ch;
            player_name[len + 1] = '\0';
        }
        ch = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        size_t len = strlen(player_name);
        if (len > 0) player_name[len - 1] = '\0';
    }
}

/* Dialogo modal de confirmacao do "Resetar ranking".
 * Confirmar -> liberarPlacar + remove(ranking.dat). */
static void DrawConfirmDialog(void) {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0, 0, 0, 160});

    Rectangle box = (Rectangle){ SCREEN_W / 2 - 240, SCREEN_H / 2 - 110, 480, 220 };
    DrawRectangleRounded(box, 0.1f, 8, COR_PAINEL);

    const char *msg = "Apagar todo o ranking?";
    int mw = gMeasure(msg, 24);
    gText(msg, (int)(box.x + (box.width - mw) / 2), (int)(box.y + 30), 24, COR_TEXTO);

    const char *sub = "Esta acao nao pode ser desfeita.";
    int sw = gMeasure(sub, 16);
    gText(sub, (int)(box.x + (box.width - sw) / 2), (int)(box.y + 70), 16, COR_TEXTO);

    Rectangle r_cancel = (Rectangle){ box.x + 40,        box.y + 130, 180, 50 };
    Rectangle r_ok     = (Rectangle){ box.x + 260,       box.y + 130, 180, 50 };

    if (Button(r_cancel, "Cancelar", COR_PAINEL_HOVER, COR_ACENTO_HOVER, COR_TEXTO)) {
        confirm_open = 0;
    }
    if (Button(r_ok, "Confirmar", COR_PERIGO, COR_PERIGO_HOVER, COR_TEXTO)) {
        liberarPlacar(&ranking);
        remove(ARQUIVO_PLACAR);
        confirm_open = 0;
    }
}

/* Desenha campo de nome (com cursor piscante), toggle de modo
 * (Casual / Mangue timer), botao destrutivo "Resetar ranking" e
 * "Voltar". O modal de confirmacao se sobrepoe quando ativo. */
void SettingsDraw(void) {
    const char *title = "EDITAR";
    int title_size = 40;
    int tw = gMeasure(title, title_size);
    gText(title, (SCREEN_W - tw) / 2, 40, title_size, COR_PRIMARIA);

    int x = 120;
    int y = 130;

    /* Player name field */
    gText("Nome do jogador", x, y, 22, COR_ACENTO);
    y += 34;
    Rectangle field = (Rectangle){ x, y, SCREEN_W - 2 * x, 50 };
    DrawRectangleRounded(field, 0.2f, 8, COR_PAINEL);
    DrawRectangleRoundedLines(field, 0.2f, 8, COR_ACENTO);
    gText(player_name, (int)field.x + 14, (int)field.y + 14, 24, COR_TEXTO);

    /* Blinking cursor at the end of the buffer */
    int caret_x = (int)field.x + 14 + gMeasure(player_name, 24) + 2;
    if (((int)(GetTime() * 2)) % 2 == 0) {
        DrawRectangle(caret_x, (int)field.y + 12, 2, 28, COR_TEXTO);
    }
    y += 70;

    /* Mode toggle */
    gText("Modo de jogo", x, y, 22, COR_ACENTO);
    y += 34;
    const char *mode_label = (modo_jogo == MODO_CASUAL)
        ? "Modo: Casual"
        : "Modo: Mangue (timer)";
    Rectangle r_mode = (Rectangle){ x, y, 360, 50 };
    if (Button(r_mode, mode_label, COR_PAINEL, COR_PAINEL_HOVER, COR_TEXTO)) {
        modo_jogo = (modo_jogo == MODO_CASUAL) ? MODO_MANGUE : MODO_CASUAL;
    }
    gText("(clique para alternar)", (int)r_mode.x + (int)r_mode.width + 16,
             (int)r_mode.y + 16, 16, COR_TEXTO);
    y += 80;

    /* Fullscreen toggle */
    gText("Tela cheia", x, y, 22, COR_ACENTO);
    y += 34;
    const char *fs_label = IsWindowFullscreen() ? "Tela Cheia: ON" : "Tela Cheia: OFF";
    Rectangle r_fs = (Rectangle){ x, y, 280, 50 };
    if (Button(r_fs, fs_label, COR_PAINEL, COR_PAINEL_HOVER, COR_TEXTO)) {
        ToggleFullscreen();
    }
    gText("(ou F11)", (int)r_fs.x + (int)r_fs.width + 16,
             (int)r_fs.y + 16, 16, COR_TEXTO);
    y += 80;

    /* Reset ranking */
    Rectangle r_reset = (Rectangle){ x, y, 280, 50 };
    if (Button(r_reset, "Resetar ranking", COR_PERIGO, COR_PERIGO_HOVER, COR_TEXTO)) {
        confirm_open = 1;
    }

    /* Back */
    Rectangle r_back = (Rectangle){ SCREEN_W - 200 - 40, SCREEN_H - 60 - 30, 200, 60 };
    if (Button(r_back, "Voltar", COR_PAINEL, COR_PAINEL_HOVER, COR_TEXTO)) {
        NextScreen(SCREEN_MENU);
    }

    if (confirm_open) DrawConfirmDialog();
}
