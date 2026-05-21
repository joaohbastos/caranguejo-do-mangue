#include "screen.h"
#include "theme.h"

void NextScreen(Screen s) {
    current_screen = s;
    switch (s) {
        case SCREEN_GAME:     GameEnter();     break;
        case SCREEN_GAMEOVER: GameOverEnter(); break;
        default: break;
    }
}

int Button(Rectangle r, const char *text, Color base, Color hover, Color text_color) {
    Vector2 mouse = GetMousePosition();
    int     over  = CheckCollisionPointRec(mouse, r);
    Color   fill  = over ? hover : base;

    DrawRectangleRounded(r, 0.25f, 8, fill);

    int font_size = 24;
    int tw        = gMeasure(text, font_size);
    int tx        = (int)(r.x + (r.width  - tw)        / 2);
    int ty        = (int)(r.y + (r.height - font_size) / 2);
    gText(text, tx, ty, font_size, text_color);

    return over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void gText(const char *text, int x, int y, int size, Color color) {
    DrawTextEx(g_font, text, (Vector2){ (float)x, (float)y }, (float)size, 1.0f, color);
}

int gMeasure(const char *text, int size) {
    return (int)MeasureTextEx(g_font, text, (float)size, 1.0f).x;
}
