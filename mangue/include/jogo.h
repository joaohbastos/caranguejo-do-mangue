#ifndef JOGO_H
#define JOGO_H

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ─── Constantes do jogo ─────────────────────────────────────── */
#define FOME_MAXIMA          10
#define RODADAS_MATURIDADE   5
#define SPOTS_INICIAIS       4
#define MAX_PLACAR           10
#define CARANGUEJOS_POR_FASE 3   /* base; cresce por fase */
#define NOME_MAX             24

#define LARGURA   1000
#define ALTURA    650

/* ─── Cores temáticas ─────────────────────────────────────────── */
#define COR_FUNDO       CLITERAL(Color){15,  40,  25,  255}
#define COR_PAINEL      CLITERAL(Color){20,  55,  35,  230}
#define COR_DESTAQUE    CLITERAL(Color){255, 200,  50,  255}
#define COR_PERIGO      CLITERAL(Color){220,  60,  60,  255}
#define COR_OK          CLITERAL(Color){ 60, 200, 100,  255}
#define COR_NEUTRO      CLITERAL(Color){150, 200, 170,  255}
#define COR_TEXTO       CLITERAL(Color){230, 245, 235,  255}
#define COR_SOMBRA      CLITERAL(Color){  0,   0,   0,  120}

/* ─── Estruturas ──────────────────────────────────────────────── */

typedef struct Caranguejo {
    int  id;
    char nome[NOME_MAX];
    int  fome;           /* 0..FOME_MAXIMA */
    int  rodadasNaColonia;
    int  ehRei;          /* flag: Caranguejo-Rei */
    struct Caranguejo *prox;
} Caranguejo;

/* Fila encadeada — ED central */
typedef struct {
    Caranguejo *inicio;
    Caranguejo *fim;
    int         tamanho;
} FilaCaranguejo;

typedef struct {
    char nome[NOME_MAX];
    int  pontos;
    int  fase;
} EntradaPlacar;

typedef enum {
    TELA_MENU,
    TELA_JOGO,
    TELA_EVENTO,
    TELA_FASE_RESULTADO,
    TELA_GAME_OVER,
    TELA_PLACAR
} Tela;

typedef enum {
    EVENTO_NENHUM,
    EVENTO_MARE,
    EVENTO_REI
} TipoEvento;

typedef struct {
    TipoEvento tipo;
    int        ativo;
    int        escolha; /* -1 = não escolhida ainda */
} Evento;

typedef struct {
    /* Fila principal */
    FilaCaranguejo fila;

    /* Estado da rodada */
    int fase;
    int rodada;
    int rodadasPorFase;    /* aumenta por fase */
    int spotsDisponiveis;
    int spotsUsados;

    /* Contadores */
    int caranguejosMaturos;
    int caranguejosMortos;
    int pontuacao;
    int idProximo;

    /* Seleção de alimentação */
    int  selecionados[32];
    int  numSelecionados;

    /* Evento atual */
    Evento evento;

    /* Placar */
    EntradaPlacar placar[MAX_PLACAR];
    int           numPlacar;

    /* Controle de tela */
    Tela tela;
    Tela telaAnterior;

    /* Câmera/animação */
    float tempMensagem;
    char  mensagem[128];

    /* Nome do jogador */
    char nomeJogador[NOME_MAX];
    int  digitandoNome;
    int  cursorNome;

    Texture2D texCaranguejo;
    Texture2D texRei;
    Texture2D texFundo;
    int       texturasCarregadas;
} EstadoJogo;

/* ─── Protótipos das funções de fila (ED) ─────────────────────── */
void       filaInicializar(FilaCaranguejo *f);
void       filaEnfileirar(FilaCaranguejo *f, Caranguejo *c);
Caranguejo *filaDesenfileirar(FilaCaranguejo *f);
Caranguejo *filaBuscarMaisFaminto(FilaCaranguejo *f);
void       filaAumentarFome(FilaCaranguejo *f, int delta);
int        filaRemoverMortos(FilaCaranguejo *f, int *pontos);
int        filaRemoverMaturos(FilaCaranguejo *f, int *pontos);
int        filaContar(FilaCaranguejo *f);
void       filaLiberar(FilaCaranguejo *f);
void       filaOrdenarPorFome(FilaCaranguejo *f);   /* Insertion Sort */

/* ─── Protótipos de ordenação do placar ──────────────────────── */
void ordenarPlacar(EntradaPlacar *placar, int n);    /* Selection Sort */

/* ─── Protótipos de lógica do jogo ───────────────────────────── */
void       jogoInicializar(EstadoJogo *j);
void       jogoNovaRodada(EstadoJogo *j);
void       jogoAlimentarSelecionados(EstadoJogo *j);
void       jogoGerarEvento(EstadoJogo *j);
void       jogoAplicarEventoMare(EstadoJogo *j, int opcao);
void       jogoAplicarEventoRei(EstadoJogo *j, int opcao);
void       jogoVerificarFaseFim(EstadoJogo *j);
void       jogoAvancarFase(EstadoJogo *j);
void       jogoAvancarFase(EstadoJogo *j);
void       jogoSalvarPlacar(EstadoJogo *j);
int        jogoCarregandoPlacar(EstadoJogo *j);
Caranguejo *criarCaranguejo(int id, int ehRei);

/* ─── Protótipos de renderização ─────────────────────────────── */
void renderMenu(EstadoJogo *j);
void renderJogo(EstadoJogo *j);
void renderEvento(EstadoJogo *j);
void renderFaseResultado(EstadoJogo *j);
void renderGameOver(EstadoJogo *j);
void renderPlacar(EstadoJogo *j);
void renderBarraFome(int x, int y, int w, int h, int fome, int max);
void renderCaranguejoCard(EstadoJogo *j, Caranguejo *c, int x, int y, int selecionado, int spots);

#endif /* JOGO_H */
