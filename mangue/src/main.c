#include "../include/jogo.h"

int main(void) {
    InitWindow(LARGURA, ALTURA, "Colonia do Mangue — Manguezal do Rio Capibaribe");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    EstadoJogo jogo = {0};
    filaInicializar(&jogo.fila);
    jogoCarregandoPlacar(&jogo);
    jogo.tela           = TELA_MENU;
    jogo.digitandoNome  = 1;
    jogo.nomeJogador[0] = '\0';

    jogo.texFundo           = LoadTexture("assets/mangue_bg.png");
    jogo.texCaranguejo      = LoadTexture("assets/crab.png");
    jogo.texRei             = LoadTexture("assets/crab_king.png");
    jogo.texturasCarregadas = (jogo.texCaranguejo.id > 0 && jogo.texRei.id > 0);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(COR_FUNDO);

        switch (jogo.tela) {
            case TELA_MENU:           renderMenu(&jogo);          break;
            case TELA_JOGO:           renderJogo(&jogo);          break;
            case TELA_EVENTO:         renderEvento(&jogo);        break;
            case TELA_FASE_RESULTADO: renderFaseResultado(&jogo); break;
            case TELA_GAME_OVER:      renderGameOver(&jogo);      break;
            case TELA_PLACAR:         renderPlacar(&jogo);        break;
        }

        EndDrawing();
    }

    UnloadTexture(jogo.texFundo);
    UnloadTexture(jogo.texCaranguejo);
    UnloadTexture(jogo.texRei);
    filaLiberar(&jogo.fila);
    CloseWindow();
    return 0;
}