#ifndef SCREENS_SCREEN_H
#define SCREENS_SCREEN_H

#include "raylib.h"
#include "../game_state.h"
#include "../ranking.h"

#define SCREEN_W 1000
#define SCREEN_H 650

#define PLAYER_NAME_MAX 24

typedef enum {
    SCREEN_MENU,
    SCREEN_GAME,
    SCREEN_ABOUT,
    SCREEN_SETTINGS,
    SCREEN_GAMEOVER
} Screen;

typedef enum {
    MODO_CASUAL,
    MODO_MANGUE
} Modo;

extern Screen    current_screen;
extern char      player_name[PLAYER_NAME_MAX];
extern Modo      modo_jogo;
extern GameState game;
extern Registro *ranking;

/* Recursos globais (carregados em main_gui.c). */
extern Texture2D tex_crab;
extern Texture2D tex_crab_king;
extern Texture2D tex_mangue_bg;
extern Font      g_font;

/* Troca para outra tela. Dispara hooks de entrada (GameEnter/GameOverEnter). */
void NextScreen(Screen s);

/* Botao arredondado com feedback de hover. Retorna 1 no frame do clique. */
int Button(Rectangle r, const char *text, Color base, Color hover, Color text_color);

/* Wrappers de texto que usam a fonte global g_font (fallback para
 * GetFontDefault quando o TTF nao foi carregado). Mantem mesma assinatura
 * de DrawText/MeasureText para facilitar substituicao. */
void gText(const char *text, int x, int y, int size, Color color);
int  gMeasure(const char *text, int size);

/* Per-screen update + draw entry points. */
void MenuUpdate(void);      void MenuDraw(void);
void AboutUpdate(void);     void AboutDraw(void);
void SettingsUpdate(void);  void SettingsDraw(void);
void GameUpdate(void);      void GameDraw(void);     void GameEnter(void);
void GameOverUpdate(void);  void GameOverDraw(void); void GameOverEnter(void);

#endif /* SCREENS_SCREEN_H */
