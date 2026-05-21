#include "raylib.h"
#include "screen.h"
#include "theme.h"

void MenuUpdate(void) { }

void MenuDraw(void) {
    const char *title    = "GUARDIOES DO MANGUE";
    const char *subtitle = "Colonia de caranguejos no Capibaribe";

    int title_size = 56;
    int sub_size   = 22;
    int tw         = gMeasure(title, title_size);
    int sw         = gMeasure(subtitle, sub_size);

    gText(title,    (SCREEN_W - tw) / 2, 90,  title_size, COR_PRIMARIA);
    gText(subtitle, (SCREEN_W - sw) / 2, 160, sub_size,   COR_TEXTO);

    int btn_w = 260;
    int btn_h = 60;
    int gap   = 24;
    int total = btn_h * 3 + gap * 2;
    int x     = (SCREEN_W - btn_w) / 2;
    int y0    = (SCREEN_H - total) / 2 + 40;

    Rectangle r_play = (Rectangle){ x, y0,                      btn_w, btn_h };
    Rectangle r_set  = (Rectangle){ x, y0 + (btn_h + gap),      btn_w, btn_h };
    Rectangle r_abt  = (Rectangle){ x, y0 + 2 * (btn_h + gap),  btn_w, btn_h };

    if (Button(r_play, "JOGAR",  COR_PRIMARIA, COR_PRIMARIA_HOVER, COR_TEXTO)) NextScreen(SCREEN_GAME);
    if (Button(r_set,  "EDITAR", COR_PAINEL,   COR_PAINEL_HOVER,   COR_TEXTO)) NextScreen(SCREEN_SETTINGS);
    if (Button(r_abt,  "SOBRE",  COR_ACENTO,   COR_ACENTO_HOVER,   COR_TEXTO)) NextScreen(SCREEN_ABOUT);

    const char *hint = "Clique para selecionar";
    int hw = gMeasure(hint, 16);
    gText(hint, (SCREEN_W - hw) / 2, SCREEN_H - 32, 16, COR_TEXTO);
}
