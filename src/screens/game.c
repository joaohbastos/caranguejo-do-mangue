#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "screen.h"
#include "theme.h"
#include "../game_state.h"
#include "../colonia.h"
#include "../jogo.h"
#include "../eventos.h"

#define MANGUE_W   ((int)(SCREEN_W * 0.7f))
#define PANEL_X    MANGUE_W
#define PANEL_W    (SCREEN_W - MANGUE_W)
#define PANEL_PAD  20

#define CRAB_RADIUS  34
#define CRAB_CELL_W  150
#define CRAB_CELL_H  150
#define CRAB_BAR_W   80
#define CRAB_BAR_H   10
#define CRAB_MARGIN  20

#define LIMPEZA_POR_ACAO 3
#define KING_BONUS_PONTOS 50
#define PHASE_CARD_SECONDS 2.0f

typedef enum {
    GS_PLAYING,        
    GS_CONFIRM_PASS,  
    GS_EVENT_MODAL,   
    GS_KING_OFFER,   
    GS_KING_BONUS,     
    GS_PHASE_CARD,    
    GS_DYING         
} GameSubState;

#define DYING_SECONDS 0.6f

static GameSubState sub_state        = GS_PLAYING;
static TipoEvento   pending_event    = EVENTO_NENHUM;
static float        phase_card_time  = 0.0f;
static float        dying_timer      = 0.0f;
static int          feeding_mode     = 0;
static int          king_bonus_given = 0;  

static void triggerEndOfRound(void);
static void resolveAfterEvent(void);
static void finishRound(void);

#define MAX_VISUALS 64

typedef struct {
    int   id;
    float fome_exibida;   
} CrabVisual;

typedef struct {
    int   cx, cy;
    int   ehRei;
    float alpha;         
} Ghost;

static CrabVisual visuals[MAX_VISUALS];
static int        n_visuals = 0;
static Ghost      ghosts[MAX_VISUALS];
static int        n_ghosts  = 0;

static float fomeExibida(int id, int fome_real) {
    for (int i = 0; i < n_visuals; i++) {
        if (visuals[i].id == id) return visuals[i].fome_exibida;
    }
    if (n_visuals < MAX_VISUALS) {
        visuals[n_visuals].id           = id;
        visuals[n_visuals].fome_exibida = (float)fome_real;
        n_visuals++;
    }
    return (float)fome_real;
}

static void tickVisuals(void) {
    float dt = GetFrameTime();
    float fator = dt * 5.0f;  
    if (fator > 1.0f) fator = 1.0f;

    for (Caranguejo *c = game.colonia; c != NULL; c = c->proximo) {
        for (int i = 0; i < n_visuals; i++) {
            if (visuals[i].id == c->id) {
                visuals[i].fome_exibida +=
                    ((float)c->nivelFome - visuals[i].fome_exibida) * fator;
                break;
            }
        }
        fomeExibida(c->id, c->nivelFome);
    }
}

static void addGhost(int cx, int cy, int ehRei) {
    if (n_ghosts >= MAX_VISUALS) return;
    ghosts[n_ghosts++] = (Ghost){ cx, cy, ehRei, 1.0f };
}

static void tickGhosts(void) {
    float dt = GetFrameTime();
    int write = 0;
    for (int i = 0; i < n_ghosts; i++) {
        ghosts[i].alpha -= dt / 0.5f; 
        if (ghosts[i].alpha > 0.0f) {
            ghosts[write++] = ghosts[i];
        }
    }
    n_ghosts = write;
}

static void resetVisuals(void) {
    n_visuals = 0;
    n_ghosts  = 0;
}

static const char *nomeFase(int faseId) {
    switch (faseId) {
        case 1:  return "Mare Baixa";
        case 2:  return "Mare Cheia";
        case 3:  return "Tempestade no Capibaribe";
        default: return "—";
    }
}

static Color corFome(int fome, int max) {
    float frac = (float)fome / (float)max;
    if (frac < 0.4f) return (Color){ 80, 200,  80, 255 };
    if (frac < 0.7f) return (Color){240, 200,  40, 255 };
    return                  (Color){220,  60,  60, 255 };
}

static int gridCols(void) {
    int usable = MANGUE_W - 2 * CRAB_MARGIN;
    int cols   = usable / CRAB_CELL_W;
    return cols < 1 ? 1 : cols;
}

static void crabPosition(int slot, int *cx, int *cy) {
    int cols = gridCols();
    int col  = slot % cols;
    int row  = slot / cols;
    *cx = CRAB_MARGIN + col * CRAB_CELL_W + CRAB_CELL_W / 2;
    *cy = CRAB_MARGIN + row * CRAB_CELL_H + CRAB_CELL_H / 2 + 20;
}

static const char *eventTitle(TipoEvento e) {
    switch (e) {
        case EVENTO_MARE: return "Mare Cheia!";
        case EVENTO_SIRI: return "Siri Invasor!";
        case EVENTO_REI:  return "Caranguejo Rei";
        default:          return "Evento";
    }
}

static const char *eventLine1(TipoEvento e) {
    switch (e) {
        case EVENTO_MARE: return "A mare invade o mangue e revira tudo.";
        case EVENTO_SIRI: return "Um siri invasor aparece nas pocas de mare.";
        case EVENTO_REI:  return "Um Caranguejo Rei se aproxima da colonia.";
        default:          return "";
    }
}

static const char *eventLine2(TipoEvento e) {
    switch (e) {
        case EVENTO_MARE: return "+1 de fome em todos os caranguejos.";
        case EVENTO_SIRI: return "Cace-o nesta rodada ou ele atacara na proxima.";
        case EVENTO_REI:  return "Decida se ele entra na colonia.";
        default:          return "";
    }
}

static void drawCrown(int cx, int top_y) {
    Color gold = (Color){ 245, 200, 50, 255 };
    int base_w = 26;
    int base_h = 5;
    int base_x = cx - base_w / 2;
    int base_y = top_y - base_h;
    DrawRectangle(base_x, base_y, base_w, base_h, gold);
    DrawTriangle((Vector2){ base_x,            base_y },
                 (Vector2){ base_x + 6,        base_y },
                 (Vector2){ base_x + 3,        base_y - 8 }, gold);
    DrawTriangle((Vector2){ cx - 3,            base_y },
                 (Vector2){ cx + 3,            base_y },
                 (Vector2){ cx,                base_y - 10 }, gold);
    DrawTriangle((Vector2){ base_x + base_w - 6, base_y },
                 (Vector2){ base_x + base_w,     base_y },
                 (Vector2){ base_x + base_w - 3, base_y - 8 }, gold);
}

static void drawCrab(Caranguejo *c, int cx, int cy) {
    Vector2 m   = GetMousePosition();
    int hovered = (sub_state == GS_PLAYING) && feeding_mode && CheckCollisionPointCircle(m, (Vector2){cx, cy}, CRAB_RADIUS);

    float fome_show = fomeExibida(c->id, c->nivelFome);
    int bar_x = cx - CRAB_BAR_W / 2;
    int bar_y = cy - CRAB_RADIUS - 18;
    DrawRectangle(bar_x, bar_y, CRAB_BAR_W, CRAB_BAR_H, (Color){50, 50, 50, 200});
    int fill_w = (int)(CRAB_BAR_W * (fome_show / (float)MAX_FOME));
    DrawRectangle(bar_x, bar_y, fill_w, CRAB_BAR_H,
                  corFome((int)(fome_show + 0.5f), MAX_FOME));
    DrawRectangleLines(bar_x, bar_y, CRAB_BAR_W, CRAB_BAR_H, COR_TEXTO);

    if (hovered) DrawCircle(cx, cy, CRAB_RADIUS + 6, COR_PRIMARIA);

    Texture2D tex = c->ehRei ? tex_crab_king : tex_crab;
    if (tex.id > 0) {
        DrawTexture(tex, cx - tex.width / 2, cy - tex.height / 2, WHITE);
    } else {
        Color body = c->ehRei ? COR_PRIMARIA : COR_ACENTO;
        DrawCircle(cx, cy, CRAB_RADIUS, body);
        DrawCircleLines(cx, cy, CRAB_RADIUS, COR_TEXTO);
        if (c->ehRei) drawCrown(cx, bar_y - 2);
    }

    char id_buf[12];
    snprintf(id_buf, sizeof(id_buf), "#%d", c->id);
    int tw = gMeasure(id_buf, 20);
    gText(id_buf, cx - tw / 2, cy + CRAB_RADIUS + 4, 20, COR_TEXTO);
}

static void drawGhosts(void) {
    for (int i = 0; i < n_ghosts; i++) {
        unsigned char a = (unsigned char)(ghosts[i].alpha * 255.0f);
        Texture2D tex = ghosts[i].ehRei ? tex_crab_king : tex_crab;
        if (tex.id > 0) {
            DrawTexture(tex,
                        ghosts[i].cx - tex.width / 2,
                        ghosts[i].cy - tex.height / 2,
                        (Color){255, 255, 255, a});
        } else {
            Color body = ghosts[i].ehRei ? COR_PRIMARIA : COR_ACENTO;
            body.a = a;
            DrawCircle(ghosts[i].cx, ghosts[i].cy, CRAB_RADIUS, body);
        }
    }
}

static void drawMangroveArea(void) {
    if (tex_mangue_bg.id > 0) {
        DrawTexturePro(tex_mangue_bg,
                       (Rectangle){ 0, 0, (float)tex_mangue_bg.width,
                                          (float)tex_mangue_bg.height },
                       (Rectangle){ 0, 0, (float)MANGUE_W, (float)SCREEN_H },
                       (Vector2){ 0, 0 }, 0.0f, WHITE);
    } else {
        DrawRectangle(0, 0, MANGUE_W, SCREEN_H, (Color){10, 44, 44, 255});
    }

    int slot = 0;
    for (Caranguejo *c = game.colonia; c != NULL; c = c->proximo) {
        int cx, cy;
        crabPosition(slot, &cx, &cy);
        drawCrab(c, cx, cy);
        slot++;
    }

    drawGhosts();

    if (sub_state == GS_PLAYING && feeding_mode) {
        const char *hint = "Clique em um caranguejo para alimentar (ESC cancela)";
        int tw = gMeasure(hint, 18);
        gText(hint, (MANGUE_W - tw) / 2, SCREEN_H - 36, 18, COR_PRIMARIA);
    }
}

static int actionButton(Rectangle r, const char *text, int enabled,
                        Color base, Color hover) {
    if (!enabled) {
        DrawRectangleRounded(r, 0.25f, 8, (Color){60, 75, 75, 255});
        int tw = gMeasure(text, 20);
        gText(text,
                 (int)(r.x + (r.width  - tw) / 2),
                 (int)(r.y + (r.height - 20) / 2),
                 20, (Color){160, 170, 170, 255});
        return 0;
    }
    int clicked = Button(r, text, base, hover, COR_TEXTO);
    return clicked && (sub_state == GS_PLAYING);
}

static Rectangle dialogBox(int w, int h) {
    return (Rectangle){ SCREEN_W / 2 - w / 2, SCREEN_H / 2 - h / 2, w, h };
}

static void dimBackground(void) {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0, 0, 0, 160});
}

static void drawConfirmPassDialog(void) {
    dimBackground();
    Rectangle box = dialogBox(480, 220);
    DrawRectangleRounded(box, 0.1f, 8, COR_PAINEL);

    char msg[80];
    snprintf(msg, sizeof(msg), "Pular rodada com %d spot(s) sobrando?", game.spotsRestantes);
    int mw = gMeasure(msg, 22);
    gText(msg, (int)(box.x + (box.width - mw) / 2), (int)(box.y + 40), 22, COR_TEXTO);

    Rectangle r_cancel = (Rectangle){ box.x + 40,  box.y + 130, 180, 50 };
    Rectangle r_ok     = (Rectangle){ box.x + 260, box.y + 130, 180, 50 };

    if (Button(r_cancel, "Cancelar", COR_PAINEL_HOVER, COR_ACENTO_HOVER, COR_TEXTO)) {
        sub_state = GS_PLAYING;
    }
    if (Button(r_ok, "Confirmar", COR_PRIMARIA, COR_PRIMARIA_HOVER, COR_TEXTO)) {
        sub_state = GS_PLAYING;
        triggerEndOfRound();
    }
}

static void drawEventModal(void) {
    dimBackground();
    Rectangle box = dialogBox(560, 260);
    DrawRectangleRounded(box, 0.08f, 8, COR_PAINEL);

    const char *t = eventTitle(pending_event);
    int tw = gMeasure(t, 32);
    gText(t, (int)(box.x + (box.width - tw) / 2), (int)(box.y + 30), 32, COR_PRIMARIA);

    const char *l1 = eventLine1(pending_event);
    const char *l2 = eventLine2(pending_event);
    int l1w = gMeasure(l1, 18);
    int l2w = gMeasure(l2, 18);
    gText(l1, (int)(box.x + (box.width - l1w) / 2), (int)(box.y + 90),  18, COR_TEXTO);
    gText(l2, (int)(box.x + (box.width - l2w) / 2), (int)(box.y + 120), 18, COR_TEXTO);

    Rectangle r_ok = (Rectangle){ box.x + box.width / 2 - 90, box.y + box.height - 70, 180, 50 };
    if (Button(r_ok, "Entendi", COR_PRIMARIA, COR_PRIMARIA_HOVER, COR_TEXTO)) {
        sub_state = GS_PLAYING;
        resolveAfterEvent();
    }
}

static void drawKingOfferModal(void) {
    dimBackground();
    Rectangle box = dialogBox(580, 320);
    DrawRectangleRounded(box, 0.08f, 8, COR_PAINEL);

    const char *t = "Caranguejo Rei";
    int tw = gMeasure(t, 32);
    gText(t, (int)(box.x + (box.width - tw) / 2), (int)(box.y + 30), 32, COR_PRIMARIA);

    const char *l1 = "Um Caranguejo Rei se aproxima da colonia.";
    const char *l2 = "Aceitar: ele entra mas come o dobro de fome.";
    const char *l3 = "Sobreviver 5 rodadas: bonus -2 fome e +50 pontos.";
    int l1w = gMeasure(l1, 18);
    int l2w = gMeasure(l2, 18);
    int l3w = gMeasure(l3, 18);
    gText(l1, (int)(box.x + (box.width - l1w) / 2), (int)(box.y + 90),  18, COR_TEXTO);
    gText(l2, (int)(box.x + (box.width - l2w) / 2), (int)(box.y + 120), 18, COR_TEXTO);
    gText(l3, (int)(box.x + (box.width - l3w) / 2), (int)(box.y + 150), 18, COR_ACENTO);

    Rectangle r_reject = (Rectangle){ box.x + 60,             box.y + box.height - 70, 200, 50 };
    Rectangle r_accept = (Rectangle){ box.x + box.width - 260, box.y + box.height - 70, 200, 50 };

    if (Button(r_reject, "Rejeitar", COR_PAINEL_HOVER, COR_ACENTO_HOVER, COR_TEXTO)) {
        pending_event = EVENTO_NENHUM;
        sub_state = GS_PLAYING;
        resolveAfterEvent();
    }
    if (Button(r_accept, "Aceitar", COR_PRIMARIA, COR_PRIMARIA_HOVER, COR_TEXTO)) {
        int next_id = proximoIdDisponivel(game.colonia);
        inserirRei(&game, next_id);
        game.reiAtivo       = 1;
        game.reiRodadasVivo = 0;
        king_bonus_given    = 0;
        ordenarPorFome(&game.colonia);
        pending_event = EVENTO_NENHUM;
        sub_state = GS_PLAYING;
        resolveAfterEvent();
    }
}

static void drawKingBonusModal(void) {
    dimBackground();
    Rectangle box = dialogBox(560, 260);
    DrawRectangleRounded(box, 0.08f, 8, COR_PAINEL);

    const char *t = "Bonus do Rei!";
    int tw = gMeasure(t, 32);
    gText(t, (int)(box.x + (box.width - tw) / 2), (int)(box.y + 30), 32, COR_ACENTO);

    const char *l1 = "Seu Caranguejo Rei sobreviveu 5 rodadas.";
    char l2[80];
    snprintf(l2, sizeof(l2), "-2 de fome em todos +%d pontos.", KING_BONUS_PONTOS);
    int l1w = gMeasure(l1, 18);
    int l2w = gMeasure(l2, 18);
    gText(l1, (int)(box.x + (box.width - l1w) / 2), (int)(box.y + 90),  18, COR_TEXTO);
    gText(l2, (int)(box.x + (box.width - l2w) / 2), (int)(box.y + 120), 18, COR_TEXTO);

    Rectangle r_ok = (Rectangle){ box.x + box.width / 2 - 90, box.y + box.height - 70, 180, 50 };
    if (Button(r_ok, "Entendi", COR_PRIMARIA, COR_PRIMARIA_HOVER, COR_TEXTO)) {
        sub_state = GS_PLAYING;
        finishRound();
    }
}

static void drawPhaseCard(void) {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){8, 32, 32, 240});

    const char *name = nomeFase(game.fase);
    int s = 56;
    int w = gMeasure(name, s);
    gText(name, (SCREEN_W - w) / 2, SCREEN_H / 2 - 60, s, COR_PRIMARIA);

    char buf[32];
    snprintf(buf, sizeof(buf), "Rodada %d", game.rodadaAtual);
    int s2 = 24;
    int w2 = gMeasure(buf, s2);
    gText(buf, (SCREEN_W - w2) / 2, SCREEN_H / 2 + 20, s2, COR_TEXTO);
}

/* ─── round flow ─────────────────────────────────────────────────── */

static void triggerEndOfRound(void) {
    if (game.siriAtivo) {
        aplicarSiriIgnorado(&game);
        game.siriAtivo = 0;
        ordenarPorFome(&game.colonia);
    }

    ParametrosFase fase = aplicarFaseAtual(game.rodadaAtual);
    aplicarFomeRodada(&game, fase.incrementoFome);
    ordenarPorFome(&game.colonia);

    int poluicao_anterior = game.poluicao;
    if ((rand() % 3) == 0) {
        game.poluicao++;
        if (game.poluicao > POLUICAO_MAX) game.poluicao = POLUICAO_MAX;
    }
    if (poluicao_anterior < POLUICAO_MAX && game.poluicao >= POLUICAO_MAX) {
        dying_timer = DYING_SECONDS;
        sub_state   = GS_DYING;
        return;
    }

    if ((rand() % 100) < fase.chanceEvento) {
        pending_event = sortearEvento();
        sub_state = (pending_event == EVENTO_REI) ? GS_KING_OFFER : GS_EVENT_MODAL;
        return;
    }
    pending_event = EVENTO_NENHUM;
    resolveAfterEvent();
}

static void resolveAfterEvent(void) {
    switch (pending_event) {
        case EVENTO_MARE:
            aplicarMare(&game);
            ordenarPorFome(&game.colonia);
            break;
        case EVENTO_SIRI:
            game.siriAtivo = 1;
            break;
        default:
            break;
    }
    pending_event = EVENTO_NENHUM;

    if (game.reiAtivo && !king_bonus_given &&
        game.reiRodadasVivo >= REI_RODADAS_PARA_BONUS) {
        aplicarBonusRei(&game);
        ordenarPorFome(&game.colonia);
        game.pontuacao  += KING_BONUS_PONTOS;
        king_bonus_given = 1;
        sub_state = GS_KING_BONUS;
        return;
    }
    finishRound();
}

static void finishRound(void) {
    int slot = 0;
    for (Caranguejo *c = game.colonia; c != NULL; c = c->proximo) {
        if (c->nivelFome >= MAX_FOME) {
            int cx, cy;
            crabPosition(slot, &cx, &cy);
            addGhost(cx, cy, c->ehRei);
        }
        slot++;
    }

    int mortos = verificarMortes(&game.colonia);
    if (mortos > 0) {
        dying_timer = DYING_SECONDS;
        sub_state   = GS_DYING;
        return;
    }

    ParametrosFase fase = aplicarFaseAtual(game.rodadaAtual);
    for (int i = 0; i < fase.novosCaranguejos; i++) {
        inserirNovoCaranguejo(&game, NULL);
    }
    incrementarRodadasNaColonia(&game);

    Caranguejo *rei = encontrarRei(game.colonia);
    if (rei != NULL) {
        game.reiAtivo       = 1;
        game.reiRodadasVivo = rei->rodadasNaColonia;
    } else {
        game.reiAtivo       = 0;
        game.reiRodadasVivo = 0;
    }

    int prev_fase = game.fase;
    game.rodadaAtual++;
    ParametrosFase new_fase = aplicarFaseAtual(game.rodadaAtual);
    game.fase = new_fase.faseId;
    game.pontuacao++;
    game.spotsRestantes = SPOTS_POR_RODADA;
    feeding_mode = 0;

    ordenarPorFome(&game.colonia);

    if (game.fase != prev_fase) {
        phase_card_time = PHASE_CARD_SECONDS;
        sub_state = GS_PHASE_CARD;
    } else {
        sub_state = GS_PLAYING;
    }
}

static void clickFeedTarget(void) {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
    Vector2 m = GetMousePosition();
    if (m.x >= MANGUE_W) return;

    int slot = 0;
    for (Caranguejo *c = game.colonia; c != NULL; c = c->proximo) {
        int cx, cy;
        crabPosition(slot, &cx, &cy);
        if (CheckCollisionPointCircle(m, (Vector2){cx, cy}, CRAB_RADIUS)) {
            alimentarCaranguejo(game.colonia, c->id);
            game.spotsRestantes--;
            feeding_mode = 0;
            return;
        }
        slot++;
    }
    feeding_mode = 0;
}

static void drawInfoPanel(void) {
    DrawRectangle(PANEL_X, 0, PANEL_W, SCREEN_H, COR_PAINEL);

    int x = PANEL_X + PANEL_PAD;
    int y = PANEL_PAD;

    const char *title = "INFO";
    int tw = gMeasure(title, 28);
    gText(title, PANEL_X + (PANEL_W - tw) / 2, y, 28, COR_PRIMARIA);
    y += 50;

    char buf[64];
    int line_h = 28;

    snprintf(buf, sizeof(buf), "Rodada %d", game.rodadaAtual);
    gText(buf, x, y, 20, COR_TEXTO); y += line_h;

    snprintf(buf, sizeof(buf), "Fase: %s", nomeFase(game.fase));
    gText(buf, x, y, 18, COR_TEXTO); y += line_h;

    snprintf(buf, sizeof(buf), "Poluicao: %d / %d", game.poluicao, POLUICAO_MAX);
    gText(buf, x, y, 20, COR_TEXTO); y += line_h;

    snprintf(buf, sizeof(buf), "Spots: %d", game.spotsRestantes);
    gText(buf, x, y, 20, COR_TEXTO); y += line_h;

    snprintf(buf, sizeof(buf), "Pontuacao: %d", game.pontuacao);
    gText(buf, x, y, 20, COR_TEXTO); y += line_h;

    snprintf(buf, sizeof(buf), "Streak: %d", game.streak);
    gText(buf, x, y, 20, COR_TEXTO); y += line_h + 6;

    if (game.siriAtivo) {
        gText("! Siri pendente", x, y, 20, COR_PERIGO);
        y += line_h;
    }
    if (game.reiAtivo) {
        snprintf(buf, sizeof(buf), "Rei vivo: %d rodadas", game.reiRodadasVivo);
        gText(buf, x, y, 20, COR_ACENTO);
        y += line_h;
        int faltam = REI_RODADAS_PARA_BONUS - game.reiRodadasVivo;
        if (faltam > 0) {
            snprintf(buf, sizeof(buf), "Bonus em %d rodada(s)", faltam);
            gText(buf, x, y, 16, COR_ACENTO);
            y += line_h;
        }
    }

    int btn_w = PANEL_W - 2 * PANEL_PAD;
    int btn_h = 46;
    int gap   = 10;
    int total = btn_h * 4 + gap * 3;
    int by    = SCREEN_H - PANEL_PAD - total;

    int spots_left = game.spotsRestantes > 0;

    Rectangle r_feed  = (Rectangle){ x, by,                    btn_w, btn_h };
    Rectangle r_clean = (Rectangle){ x, by + (btn_h + gap),    btn_w, btn_h };
    Rectangle r_siri  = (Rectangle){ x, by + 2*(btn_h + gap),  btn_w, btn_h };
    Rectangle r_pass  = (Rectangle){ x, by + 3*(btn_h + gap),  btn_w, btn_h };

    if (actionButton(r_feed, feeding_mode ? "Cancelar alimentar" : "Alimentar",
                     spots_left, COR_PRIMARIA, COR_PRIMARIA_HOVER)) {
        feeding_mode = !feeding_mode;
    }
    if (actionButton(r_clean, "Limpar Mangue", spots_left, COR_ACENTO, COR_ACENTO_HOVER)) {
        game.poluicao -= LIMPEZA_POR_ACAO;
        if (game.poluicao < 0) game.poluicao = 0;
        game.spotsRestantes--;
        feeding_mode = 0;
    }
    if (actionButton(r_siri, "Cacar Siri", spots_left && game.siriAtivo,
                     COR_PERIGO, COR_PERIGO_HOVER)) {
        game.siriAtivo = 0;
        game.spotsRestantes--;
        feeding_mode = 0;
    }

    Color pass_base  = spots_left ? COR_PAINEL_HOVER : COR_PRIMARIA;
    Color pass_hover = spots_left ? COR_ACENTO_HOVER : COR_PRIMARIA_HOVER;
    int pass_clicked = Button(r_pass, "PASSAR RODADA", pass_base, pass_hover, COR_TEXTO);
    if (pass_clicked && sub_state == GS_PLAYING) {
        if (spots_left) sub_state = GS_CONFIRM_PASS;
        else            triggerEndOfRound();
    }
}

void GameEnter(void) {
    gameStateLiberar(&game);
    gameStateInicializar(&game);

    for (int i = 1; i <= 4; i++) {
        Caranguejo *c = criarCaranguejo(i, 0, 2);
        if (c != NULL) inserirCaranguejo(&game.colonia, c);
    }

    game.rodadaAtual    = 1;
    game.fase           = aplicarFaseAtual(1).faseId;
    game.poluicao       = 0;
    game.spotsRestantes = SPOTS_POR_RODADA;
    game.pontuacao      = 0;
    game.streak         = 0;
    game.reiAtivo       = 0;
    game.reiRodadasVivo = 0;
    game.siriAtivo      = 0;

    feeding_mode     = 0;
    pending_event    = EVENTO_NENHUM;
    phase_card_time  = 0.0f;
    dying_timer      = 0.0f;
    king_bonus_given = 0;
    sub_state        = GS_PLAYING;

    resetVisuals();
}

void GameUpdate(void) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        feeding_mode = 0;
        if (sub_state == GS_CONFIRM_PASS) sub_state = GS_PLAYING;
    }

    tickVisuals();
    tickGhosts();

    if (sub_state == GS_DYING) {
        dying_timer -= GetFrameTime();
        if (dying_timer <= 0.0f) NextScreen(SCREEN_GAMEOVER);
        return;
    }

    if (sub_state == GS_PHASE_CARD) {
        phase_card_time -= GetFrameTime();
        if (phase_card_time <= 0.0f) sub_state = GS_PLAYING;
        return;
    }

    if (sub_state == GS_PLAYING && feeding_mode) {
        clickFeedTarget();
    }
}

void GameDraw(void) {
    drawMangroveArea();
    drawInfoPanel();

    switch (sub_state) {
        case GS_CONFIRM_PASS: drawConfirmPassDialog(); break;
        case GS_EVENT_MODAL:  drawEventModal();        break;
        case GS_KING_OFFER:   drawKingOfferModal();    break;
        case GS_KING_BONUS:   drawKingBonusModal();    break;
        case GS_PHASE_CARD:   drawPhaseCard();         break;
        default: break;
    }
}
