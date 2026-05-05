#include "../include/jogo.h"

/* ── Helpers de desenho ───────────────────────────────────────── */

static void desenharPainelArredondado(int x, int y, int w, int h, Color cor) {
    DrawRectangleRounded((Rectangle){x, y, w, h}, 0.12f, 8, cor);
}

static void desenharTextoCentrado(const char *txt, int y, int fontSize, Color cor) {
    int larg = MeasureText(txt, fontSize);
    DrawText(txt, (LARGURA - larg) / 2, y, fontSize, cor);
}

/* ─────────────────────────────────────────────────────────────────
   renderBarraFome
───────────────────────────────────────────────────────────────── */
void renderBarraFome(int x, int y, int w, int h, int fome, int max) {
    float pct = (float)fome / (float)max;
    Color cor = (pct > 0.7f) ? COR_PERIGO :
                (pct > 0.4f) ? COR_DESTAQUE : COR_OK;

    DrawRectangle(x, y, w, h, Fade(DARKGRAY, 0.5f));
    DrawRectangle(x, y, (int)(w * pct), h, cor);
    DrawRectangleLines(x, y, w, h, Fade(WHITE, 0.3f));
}

/* ─────────────────────────────────────────────────────────────────
   renderCaranguejoCard
───────────────────────────────────────────────────────────────── */
void renderCaranguejoCard(EstadoJogo *j, Caranguejo *c, int x, int y, int selecionado, int podeSelecionar) {
    Color bordaCor = selecionado  ? COR_DESTAQUE :
                     (c->fome >= FOME_MAXIMA - 2) ? COR_PERIGO : COR_NEUTRO;

    Color fundo = selecionado
        ? CLITERAL(Color){40, 80, 50, 240}
        : CLITERAL(Color){25, 55, 38, 220};

    desenharPainelArredondado(x, y, 175, 85, fundo);
    DrawRectangleRoundedLines((Rectangle){x, y, 175, 85}, 0.12f, 8, 2.0f, bordaCor);

    /* Sprite */
    if (j->texturasCarregadas) {
        Texture2D tex = c->ehRei ? j->texRei : j->texCaranguejo;
        Rectangle src  = {0, 0, (float)tex.width, (float)tex.height};
        Rectangle dest = {x + 4, y + 4, 26, 26};
        DrawTexturePro(tex, src, dest, (Vector2){0,0}, 0.0f, WHITE);
    } else {
        DrawText(c->ehRei ? "[R]" : "[C]", x + 8, y + 8, 14, WHITE);
    }

    /* Nome */
    DrawText(c->nome, x + 34, y + 10, 13, COR_TEXTO);

    /* Rodadas */
    char rodTxt[32];
    snprintf(rodTxt, 32, "Rod: %d/%d", c->rodadasNaColonia, RODADAS_MATURIDADE);
    DrawText(rodTxt, x + 8, y + 34, 11, COR_NEUTRO);

    /* Barra de fome */
    DrawText("Fome:", x + 8, y + 52, 11, COR_NEUTRO);
    renderBarraFome(x + 52, y + 52, 112, 12, c->fome, FOME_MAXIMA);

    char fomeTxt[8];
    snprintf(fomeTxt, 8, "%d", c->fome);
    DrawText(fomeTxt, x + 168 - MeasureText(fomeTxt, 11), y + 52, 11, COR_TEXTO);

    /* Hint de seleção */
    if (!selecionado && podeSelecionar)
        DrawText("[clique]", x + 8, y + 68, 10, Fade(WHITE, 0.35f));
    if (selecionado)
        DrawText("[selecionado]", x + 8, y + 68, 10, COR_DESTAQUE);
}

/* ─────────────────────────────────────────────────────────────────
   renderMenu
───────────────────────────────────────────────────────────────── */
void renderMenu(EstadoJogo *j) {
    /* Fundo gradiente simulado */
    DrawRectangleGradientV(0, 0, LARGURA, ALTURA,
        CLITERAL(Color){10, 35, 20, 255},
        CLITERAL(Color){5, 20, 12, 255});

    /* Título */
    desenharTextoCentrado("COLONIA DO MANGUE", 80, 52, COR_DESTAQUE);
    desenharTextoCentrado("Manguezal do Rio Capibaribe", 140, 22, COR_NEUTRO);
    desenharTextoCentrado("🦀  Cuide da sua colonia de caranguejos  🦀", 175, 18, Fade(WHITE, 0.6f));

    /* Caixa de nome */
    int bx = LARGURA / 2 - 200, by = 250;
    desenharPainelArredondado(bx - 10, by - 10, 420, 50, COR_PAINEL);
    DrawText("Nome do jogador:", bx, by, 18, COR_NEUTRO);

    char nomeMostrado[NOME_MAX + 2];
    snprintf(nomeMostrado, NOME_MAX + 2, "%s%s",
             j->nomeJogador,
             (j->digitandoNome && ((int)(GetTime() * 2) % 2 == 0)) ? "|" : "");
    DrawText(nomeMostrado, bx + 180, by, 18, COR_DESTAQUE);

    /* Botões */
    int btnW = 220, btnH = 50, btnX = LARGURA / 2 - btnW / 2;

    /* Jogar */
    Rectangle btnJogar = {btnX, 330, btnW, btnH};
    bool hoverJogar = CheckCollisionPointRec(GetMousePosition(), btnJogar);
    DrawRectangleRounded(btnJogar, 0.3f, 8,
        hoverJogar ? COR_OK : CLITERAL(Color){30, 100, 55, 255});
    desenharTextoCentrado("JOGAR", 348, 22, WHITE);

    /* Placar */
    Rectangle btnPlacar = {btnX, 400, btnW, btnH};
    bool hoverPlacar = CheckCollisionPointRec(GetMousePosition(), btnPlacar);
    DrawRectangleRounded(btnPlacar, 0.3f, 8,
        hoverPlacar ? COR_NEUTRO : CLITERAL(Color){30, 70, 55, 255});
    desenharTextoCentrado("PLACAR", 418, 22, WHITE);

    /* Sair */
    Rectangle btnSair = {btnX, 470, btnW, btnH};
    bool hoverSair = CheckCollisionPointRec(GetMousePosition(), btnSair);
    DrawRectangleRounded(btnSair, 0.3f, 8,
        hoverSair ? COR_PERIGO : CLITERAL(Color){80, 30, 30, 255});
    desenharTextoCentrado("SAIR", 488, 22, WHITE);

    /* Dica */
    desenharTextoCentrado("Clique em JOGAR para comecar", 550, 15, Fade(WHITE, 0.35f));

    /* Input de nome */
    if (j->digitandoNome) {
        int key = GetCharPressed();
        while (key > 0) {
            int len = strlen(j->nomeJogador);
            if (key >= 32 && len < NOME_MAX - 1) {
                j->nomeJogador[len]     = (char)key;
                j->nomeJogador[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(j->nomeJogador) > 0)
            j->nomeJogador[strlen(j->nomeJogador) - 1] = '\0';
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mp = GetMousePosition();
        if (CheckCollisionPointRec(mp, (Rectangle){bx - 10, by - 10, 420, 50})) {
            j->digitandoNome = 1;
        } else if (hoverJogar) {
            if (strlen(j->nomeJogador) == 0)
                strncpy(j->nomeJogador, "Jogador", NOME_MAX - 1);
            jogoInicializar(j);
        } else if (hoverPlacar) {
            j->tela = TELA_PLACAR;
        } else if (hoverSair) {
            CloseWindow();
        } else {
            j->digitandoNome = 0;
        }
    }
}

/* ─────────────────────────────────────────────────────────────────
   renderJogo — tela principal
───────────────────────────────────────────────────────────────── */
void renderJogo(EstadoJogo *j) {
    DrawRectangleGradientV(0, 0, LARGURA, ALTURA,
        CLITERAL(Color){10, 35, 20, 255},
        CLITERAL(Color){5, 20, 12, 255});

    /* ── Header ── */
    desenharPainelArredondado(0, 0, LARGURA, 55, COR_PAINEL);
    char header[128];
    snprintf(header, 128, "Fase %d  |  Rodada %d/%d  |  Spots: %d/%d  |  Pontos: %d",
             j->fase, j->rodada, j->rodadasPorFase,
             j->spotsDisponiveis - j->numSelecionados, j->spotsDisponiveis,
             j->pontuacao);
    DrawText(header, 12, 17, 18, COR_DESTAQUE);

    /* Contadores */
    char contadores[64];
    snprintf(contadores, 64, "🟢 Maturos: %d   💀 Mortos: %d",
             j->caranguejosMaturos, j->caranguejosMortos);
    DrawText(contadores, LARGURA - MeasureText(contadores, 16) - 12, 18, 16, COR_NEUTRO);

    /* ── Painel de caranguejos ── */
    desenharPainelArredondado(10, 65, LARGURA - 20, 460, COR_PAINEL);
    DrawText("Colonia — clique para selecionar quem alimentar:", 22, 75, 16, COR_NEUTRO);

    int cx = 20, cy = 100, col = 0;
    Caranguejo *atual = j->fila.inicio;
    int podeSelecionar = (j->numSelecionados < j->spotsDisponiveis);

    while (atual != NULL) {
        /* Verifica se está selecionado */
        int sel = 0;
        for (int i = 0; i < j->numSelecionados; i++)
            if (j->selecionados[i] == atual->id) { sel = 1; break; }

        Rectangle card = {cx, cy, 175, 85};
        renderCaranguejoCard(j, atual, cx, cy, sel, podeSelecionar || sel);

        /* Clique no card */
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            CheckCollisionPointRec(GetMousePosition(), card)) {
            if (sel) {
                /* Desseleciona */
                for (int i = 0; i < j->numSelecionados; i++) {
                    if (j->selecionados[i] == atual->id) {
                        j->selecionados[i] = j->selecionados[--j->numSelecionados];
                        break;
                    }
                }
            } else if (podeSelecionar) {
                j->selecionados[j->numSelecionados++] = atual->id;
            }
        }

        cx += 185;
        col++;
        if (col >= 5) { col = 0; cx = 20; cy += 95; }

        atual = atual->prox;
        if (cy > 500) break; /* overflow: rola para fora */
    }

    /* ── Rodapé ── */
    desenharPainelArredondado(10, 535, LARGURA - 20, 105, COR_PAINEL);

    /* Botão Confirmar Alimentação */
    Rectangle btnAlim = {20, 548, 240, 45};
    bool hoverAlim = CheckCollisionPointRec(GetMousePosition(), btnAlim);
    DrawRectangleRounded(btnAlim, 0.3f, 8,
        hoverAlim ? COR_OK : CLITERAL(Color){30, 100, 55, 255});
    char lblAlim[48];
    snprintf(lblAlim, 48, "Alimentar (%d sel.)", j->numSelecionados);
    DrawText(lblAlim, 30, 562, 18, WHITE);

    /* Botão Próxima Rodada */
    Rectangle btnRod = {275, 548, 220, 45};
    bool hoverRod = CheckCollisionPointRec(GetMousePosition(), btnRod);
    DrawRectangleRounded(btnRod, 0.3f, 8,
        hoverRod ? COR_DESTAQUE : CLITERAL(Color){80, 60, 20, 255});
    DrawText("Proxima Rodada", 285, 562, 18, hoverRod ? BLACK : WHITE);

    /* Botão Menu */
    Rectangle btnMenu = {510, 548, 160, 45};
    bool hoverMenu = CheckCollisionPointRec(GetMousePosition(), btnMenu);
    DrawRectangleRounded(btnMenu, 0.3f, 8,
        hoverMenu ? COR_PERIGO : CLITERAL(Color){70, 25, 25, 255});
    DrawText("Menu", 560, 562, 18, WHITE);

    /* Legenda */
    DrawText("Verde = alimentado | Amarelo = atencao | Vermelho = critico",
             20, 602, 13, Fade(WHITE, 0.4f));

    /* Mensagem temporária */
    if (j->tempMensagem > 0.0f) {
        j->tempMensagem -= GetFrameTime();
        float alpha = (j->tempMensagem > 1.0f) ? 1.0f : j->tempMensagem;
        int tw = MeasureText(j->mensagem, 20);
        DrawRectangle(LARGURA/2 - tw/2 - 10, 490, tw + 20, 34,
                      Fade(BLACK, 0.7f * alpha));
        DrawText(j->mensagem, LARGURA/2 - tw/2, 496, 20,
                 Fade(COR_DESTAQUE, alpha));
    }

    /* Interações */
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mp = GetMousePosition();
        if (CheckCollisionPointRec(mp, btnAlim) && j->numSelecionados > 0) {
            jogoAlimentarSelecionados(j);
            snprintf(j->mensagem, 128, "%d caranguejo(s) alimentado(s)!", j->spotsUsados);
            j->tempMensagem = 2.0f;
        } else if (CheckCollisionPointRec(mp, btnRod)) {
            jogoNovaRodada(j);
        } else if (CheckCollisionPointRec(mp, btnMenu)) {
            filaLiberar(&j->fila);
            filaInicializar(&j->fila);
            j->tela = TELA_MENU;
        }
    }
}

/* ─────────────────────────────────────────────────────────────────
   renderEvento
───────────────────────────────────────────────────────────────── */
void renderEvento(EstadoJogo *j) {
    DrawRectangle(0, 0, LARGURA, ALTURA, Fade(BLACK, 0.75f));

    int px = LARGURA/2 - 320, py = ALTURA/2 - 200;
    desenharPainelArredondado(px, py, 640, 400, CLITERAL(Color){18, 48, 30, 250});
    DrawRectangleRoundedLines((Rectangle){px, py, 640, 400}, 0.08f, 8, 2.0f, COR_DESTAQUE);

    if (j->evento.tipo == EVENTO_MARE) {
        desenharTextoCentrado("🌊  MARE ENCHENDO RAPIDO!", py + 20, 28, COR_DESTAQUE);
        desenharTextoCentrado("A mare esta subindo depressa.", py + 65, 18, COR_TEXTO);
        desenharTextoCentrado("Voce precisa tomar uma decisao!", py + 90, 16, Fade(WHITE,0.6f));

        /* Opção A */
        Rectangle btnA = {px + 30, py + 140, 270, 100};
        bool hA = CheckCollisionPointRec(GetMousePosition(), btnA);
        DrawRectangleRounded(btnA, 0.12f, 8, hA ? COR_OK : CLITERAL(Color){25,80,45,255});
        DrawText("PROTEGER OS FRACOS", px + 45, py + 155, 15, WHITE);
        DrawText("Salva os 3 mais famintos", px + 45, py + 178, 13, COR_NEUTRO);
        DrawText("Perde 1 spot nesta fase", px + 45, py + 196, 13, COR_PERIGO);

        /* Opção B */
        Rectangle btnB = {px + 340, py + 140, 270, 100};
        bool hB = CheckCollisionPointRec(GetMousePosition(), btnB);
        DrawRectangleRounded(btnB, 0.12f, 8, hB ? COR_PERIGO : CLITERAL(Color){80,35,25,255});
        DrawText("DEIXAR A MARE PASSAR", px + 355, py + 155, 15, WHITE);
        DrawText("Mantem todos os spots", px + 355, py + 178, 13, COR_NEUTRO);
        DrawText("+2 caranguejos extras", px + 355, py + 196, 13, COR_PERIGO);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mp = GetMousePosition();
            if (CheckCollisionPointRec(mp, btnA)) jogoAplicarEventoMare(j, 0);
            else if (CheckCollisionPointRec(mp, btnB)) jogoAplicarEventoMare(j, 1);
        }

    } else if (j->evento.tipo == EVENTO_REI) {
        desenharTextoCentrado("👑  O CARANGUEJO-REI!", py + 20, 28, COR_DESTAQUE);
        desenharTextoCentrado("Um caranguejo dominante quer entrar na colonia.", py + 65, 18, COR_TEXTO);
        desenharTextoCentrado("Alto risco. Alta recompensa.", py + 90, 16, Fade(WHITE,0.6f));

        Rectangle btnA = {px + 30, py + 140, 270, 100};
        bool hA = CheckCollisionPointRec(GetMousePosition(), btnA);
        DrawRectangleRounded(btnA, 0.12f, 8, hA ? COR_DESTAQUE : CLITERAL(Color){80,60,15,255});
        DrawText("ACEITAR O REI", px + 45, py + 155, 15, WHITE);
        DrawText("Chega com fome alta", px + 45, py + 178, 13, COR_PERIGO);
        DrawText("Se madurecer: +1 spot perm.", px + 45, py + 196, 13, COR_OK);

        Rectangle btnB = {px + 340, py + 140, 270, 100};
        bool hB = CheckCollisionPointRec(GetMousePosition(), btnB);
        DrawRectangleRounded(btnB, 0.12f, 8, hB ? COR_NEUTRO : CLITERAL(Color){30,60,45,255});
        DrawText("RECUSAR O REI", px + 355, py + 155, 15, WHITE);
        DrawText("Sem risco adicional", px + 355, py + 178, 13, COR_OK);
        DrawText("Sem recompensa extra", px + 355, py + 196, 13, COR_NEUTRO);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mp = GetMousePosition();
            if (CheckCollisionPointRec(mp, btnA)) jogoAplicarEventoRei(j, 0);
            else if (CheckCollisionPointRec(mp, btnB)) jogoAplicarEventoRei(j, 1);
        }
    }

    desenharTextoCentrado("Escolha com sabedoria — nao existe opcao completamente segura.",
                          py + 360, 13, Fade(WHITE, 0.35f));
}

/* ─────────────────────────────────────────────────────────────────
   renderFaseResultado
───────────────────────────────────────────────────────────────── */
void renderFaseResultado(EstadoJogo *j) {
    DrawRectangle(0, 0, LARGURA, ALTURA, Fade(BLACK, 0.8f));

    int metaMaturos = 2 + j->fase;
    int metaMortos  = 3 + j->fase;
    int passou = (j->caranguejosMaturos >= metaMaturos &&
                  j->caranguejosMortos  <= metaMortos);

    int px = LARGURA/2 - 280, py = ALTURA/2 - 190;
    Color corBorda = passou ? COR_OK : COR_PERIGO;
    desenharPainelArredondado(px, py, 560, 380, CLITERAL(Color){15, 40, 25, 252});
    DrawRectangleRoundedLines((Rectangle){px, py, 560, 380}, 0.08f, 8, 2.0f, corBorda);

    if (passou) {
        desenharTextoCentrado("🎉  FASE CONCLUIDA!", py + 20, 30, COR_OK);
        desenharTextoCentrado("Meta atingida! +1 spot permanente  +20 pts", py + 65, 18, COR_DESTAQUE);
    } else {
        desenharTextoCentrado("❌  META NAO CUMPRIDA", py + 20, 30, COR_PERIGO);
        desenharTextoCentrado("Penalidade: -10 pontos", py + 65, 18, COR_PERIGO);
    }

    char buf[64];
    snprintf(buf, 64, "Fase %d encerrada", j->fase);
    desenharTextoCentrado(buf, py + 108, 20, COR_NEUTRO);

    /* Stats */
    snprintf(buf, 64, "Caranguejos maturos:  %d / %d (meta)", j->caranguejosMaturos, metaMaturos);
    DrawText(buf, px + 40, py + 150, 18, j->caranguejosMaturos >= metaMaturos ? COR_OK : COR_PERIGO);
    snprintf(buf, 64, "Caranguejos mortos:   %d / %d (limite)", j->caranguejosMortos, metaMortos);
    DrawText(buf, px + 40, py + 180, 18, j->caranguejosMortos <= metaMortos ? COR_OK : COR_PERIGO);
    snprintf(buf, 64, "Pontuacao atual:  %d", j->pontuacao);
    DrawText(buf, px + 40, py + 210, 18, COR_DESTAQUE);
    snprintf(buf, 64, "Spots disponiveis:  %d", j->spotsDisponiveis);
    DrawText(buf, px + 40, py + 240, 18, COR_NEUTRO);

    /* Botão continuar */
    Rectangle btnCont = {LARGURA/2 - 120, py + 300, 240, 50};
    bool hCont = CheckCollisionPointRec(GetMousePosition(), btnCont);
    DrawRectangleRounded(btnCont, 0.3f, 8, hCont ? COR_OK : CLITERAL(Color){30,100,55,255});
    desenharTextoCentrado(passou ? "PROXIMA FASE" : "TENTAR NOVAMENTE", py + 316, 20, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(GetMousePosition(), btnCont)) {
        jogoAvancarFase(j);
    }
}

/* ─────────────────────────────────────────────────────────────────
   renderGameOver
───────────────────────────────────────────────────────────────── */
void renderGameOver(EstadoJogo *j) {
    DrawRectangleGradientV(0, 0, LARGURA, ALTURA,
        CLITERAL(Color){35, 8, 8, 255},
        CLITERAL(Color){10, 5, 5, 255});

    desenharTextoCentrado("💀  COLONIA EXTINTA", 100, 44, COR_PERIGO);
    desenharTextoCentrado("Os caranguejos do Capibaribe nao sobreviveram.", 165, 20, COR_NEUTRO);

    char buf[64];
    snprintf(buf, 64, "Fase alcancada: %d", j->fase);
    desenharTextoCentrado(buf, 230, 24, COR_DESTAQUE);
    snprintf(buf, 64, "Pontuacao final: %d", j->pontuacao);
    desenharTextoCentrado(buf, 265, 28, COR_DESTAQUE);
    snprintf(buf, 64, "Maturos: %d  |  Mortos: %d", j->caranguejosMaturos, j->caranguejosMortos);
    desenharTextoCentrado(buf, 305, 18, COR_NEUTRO);

    /* Botões */
    Rectangle btnJogar = {LARGURA/2 - 220, 380, 200, 50};
    Rectangle btnMenu  = {LARGURA/2 + 20,  380, 200, 50};
    Rectangle btnPlac  = {LARGURA/2 - 100, 450, 200, 50};

    bool hJ = CheckCollisionPointRec(GetMousePosition(), btnJogar);
    bool hM = CheckCollisionPointRec(GetMousePosition(), btnMenu);
    bool hP = CheckCollisionPointRec(GetMousePosition(), btnPlac);

    DrawRectangleRounded(btnJogar, 0.3f, 8, hJ ? COR_OK : CLITERAL(Color){30,100,55,255});
    DrawText("JOGAR NOVAMENTE", LARGURA/2 - 210, 395, 18, WHITE);

    DrawRectangleRounded(btnMenu, 0.3f, 8, hM ? COR_NEUTRO : CLITERAL(Color){30,70,55,255});
    DrawText("MENU PRINCIPAL", LARGURA/2 + 30, 395, 18, WHITE);

    DrawRectangleRounded(btnPlac, 0.3f, 8, hP ? COR_DESTAQUE : CLITERAL(Color){60,50,15,255});
    DrawText("VER PLACAR", LARGURA/2 - 90, 465, 18, hP ? BLACK : WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mp = GetMousePosition();
        if (CheckCollisionPointRec(mp, btnJogar)) jogoInicializar(j);
        else if (CheckCollisionPointRec(mp, btnMenu)) j->tela = TELA_MENU;
        else if (CheckCollisionPointRec(mp, btnPlac)) j->tela = TELA_PLACAR;
    }
}

/* ─────────────────────────────────────────────────────────────────
   renderPlacar
───────────────────────────────────────────────────────────────── */
void renderPlacar(EstadoJogo *j) {
    DrawRectangleGradientV(0, 0, LARGURA, ALTURA,
        CLITERAL(Color){10, 35, 20, 255},
        CLITERAL(Color){5, 20, 12, 255});

    desenharTextoCentrado("🏆  PLACAR DOS MELHORES MANEJADORES", 40, 28, COR_DESTAQUE);
    desenharTextoCentrado("Manguezal do Rio Capibaribe", 80, 16, COR_NEUTRO);

    /* Cabeçalho da tabela */
    int ty = 130;
    desenharPainelArredondado(60, ty - 8, LARGURA - 120, 36, CLITERAL(Color){30,80,50,200});
    DrawText("#",   80, ty, 18, COR_DESTAQUE);
    DrawText("Nome",           140, ty, 18, COR_DESTAQUE);
    DrawText("Pontos",         460, ty, 18, COR_DESTAQUE);
    DrawText("Fase",           620, ty, 18, COR_DESTAQUE);

    if (j->numPlacar == 0) {
        desenharTextoCentrado("Nenhuma partida registrada ainda.", 240, 20, COR_NEUTRO);
    } else {
        for (int i = 0; i < j->numPlacar; i++) {
            int ry = ty + 50 + i * 44;
            Color fundo = (i % 2 == 0)
                ? CLITERAL(Color){20, 55, 35, 180}
                : CLITERAL(Color){15, 45, 28, 180};
            desenharPainelArredondado(60, ry - 6, LARGURA - 120, 38, fundo);

            Color corPos = (i == 0) ? COR_DESTAQUE :
                           (i == 1) ? CLITERAL(Color){180,180,180,255} :
                           (i == 2) ? CLITERAL(Color){180,100,40,255} : COR_TEXTO;
            char pos[4];
            snprintf(pos, 4, "%d", i + 1);
            DrawText(pos,                      80, ry, 18, corPos);
            DrawText(j->placar[i].nome,       140, ry, 18, COR_TEXTO);
            char pts[16], fas[8];
            snprintf(pts, 16, "%d", j->placar[i].pontos);
            snprintf(fas, 8,  "%d", j->placar[i].fase);
            DrawText(pts, 460, ry, 18, COR_DESTAQUE);
            DrawText(fas, 620, ry, 18, COR_NEUTRO);
        }
    }

    /* Botão voltar */
    Rectangle btnVoltar = {LARGURA/2 - 110, ALTURA - 75, 220, 48};
    bool hV = CheckCollisionPointRec(GetMousePosition(), btnVoltar);
    DrawRectangleRounded(btnVoltar, 0.3f, 8, hV ? COR_NEUTRO : CLITERAL(Color){30,70,55,255});
    desenharTextoCentrado("VOLTAR AO MENU", ALTURA - 60, 20, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(GetMousePosition(), btnVoltar)) {
        j->tela = TELA_MENU;
    }
}
