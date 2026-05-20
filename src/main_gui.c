#include <stdlib.h>
#include <time.h>

#include "raylib.h"
#include "screens/screen.h"
#include "screens/theme.h"
#include "game_state.h"
#include "ranking.h"

Screen    current_screen                = SCREEN_MENU;
char      player_name[PLAYER_NAME_MAX]  = "Jogador";
Modo      modo_jogo                     = MODO_CASUAL;
GameState game;
Registro *ranking = NULL;

Texture2D tex_crab      = {0};
Texture2D tex_crab_king = {0};
Texture2D tex_mangue_bg = {0};
Font      g_font        = {0};

static void carregarRecursos(void) {
    tex_crab      = LoadTexture("assets/sprites/crab.png");
    tex_crab_king = LoadTexture("assets/sprites/crab_king.png");
    tex_mangue_bg = LoadTexture("assets/sprites/mangue_bg.png");

    g_font = LoadFontEx("assets/fonts/PressStart2P-Regular.ttf", 32, NULL, 0);
    if (g_font.texture.id == 0) {
        g_font = GetFontDefault();
    }
}

static void descarregarRecursos(void) {
    if (tex_crab.id      > 0) UnloadTexture(tex_crab);
    if (tex_crab_king.id > 0) UnloadTexture(tex_crab_king);
    if (tex_mangue_bg.id > 0) UnloadTexture(tex_mangue_bg);
    if (g_font.texture.id != 0 &&
        g_font.texture.id != GetFontDefault().texture.id) {
        UnloadFont(g_font);
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_W, SCREEN_H, "Guardioes do Mangue");
    MaximizeWindow();
    SetTargetFPS(60);
    SetExitKey(KEY_ESCAPE);

    RenderTexture2D canvas = LoadRenderTexture(SCREEN_W, SCREEN_H);
    SetTextureFilter(canvas.texture, TEXTURE_FILTER_BILINEAR);

    carregarRecursos();
    gameStateInicializar(&game);
    carregarPlacar(&ranking);  

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        float sw = (float)GetScreenWidth();
        float sh = (float)GetScreenHeight();
        Rectangle dst = { 0, 0, sw, sh };

        SetMouseOffset(0, 0);
        SetMouseScale((float)SCREEN_W / sw, (float)SCREEN_H / sh);

        switch (current_screen) {
            case SCREEN_MENU:     MenuUpdate();     break;
            case SCREEN_GAME:     GameUpdate();     break;
            case SCREEN_ABOUT:    AboutUpdate();    break;
            case SCREEN_SETTINGS: SettingsUpdate(); break;
            case SCREEN_GAMEOVER: GameOverUpdate(); break;
        }

        BeginTextureMode(canvas);
        ClearBackground(COR_FUNDO);

        switch (current_screen) {
            case SCREEN_MENU:     MenuDraw();     break;
            case SCREEN_GAME:     GameDraw();     break;
            case SCREEN_ABOUT:    AboutDraw();    break;
            case SCREEN_SETTINGS: SettingsDraw(); break;
            case SCREEN_GAMEOVER: GameOverDraw(); break;
        }

        EndTextureMode();

        Rectangle src = { 0, 0, (float)SCREEN_W, -(float)SCREEN_H };
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(canvas.texture, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
        EndDrawing();
    }

    gameStateLiberar(&game);
    liberarPlacar(&ranking);
    descarregarRecursos();
    UnloadRenderTexture(canvas);
    CloseWindow();
    return 0;
}
