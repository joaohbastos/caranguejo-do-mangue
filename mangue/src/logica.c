#include "../include/jogo.h"

#define ARQUIVO_PLACAR "placar.dat"

/* ─────────────────────────────────────────────────────────────────
   jogoInicializar
───────────────────────────────────────────────────────────────── */
void jogoInicializar(EstadoJogo *j) {
    srand((unsigned)time(NULL));

    filaLiberar(&j->fila);
    filaInicializar(&j->fila);

    j->fase               = 1;
    j->rodada             = 0;
    j->rodadasPorFase     = 6;
    j->spotsDisponiveis   = SPOTS_INICIAIS;
    j->spotsUsados        = 0;
    j->caranguejosMaturos = 0;
    j->caranguejosMortos  = 0;
    j->pontuacao          = 0;
    j->idProximo          = 0;
    j->numSelecionados    = 0;
    j->evento.tipo        = EVENTO_NENHUM;
    j->evento.ativo       = 0;
    j->evento.escolha     = -1;
    j->tempMensagem       = 0.0f;
    j->mensagem[0]        = '\0';

    jogoCarregandoPlacar(j);

    int chegando = CARANGUEJOS_POR_FASE + j->fase - 1;
    for (int i = 0; i < chegando; i++) {
        Caranguejo *c = criarCaranguejo(j->idProximo++, 0);
        if (c) filaEnfileirar(&j->fila, c);
    }

    filaOrdenarPorFome(&j->fila);
    j->tela = TELA_JOGO;
}

/* ─────────────────────────────────────────────────────────────────
   jogoNovaRodada
───────────────────────────────────────────────────────────────── */
void jogoNovaRodada(EstadoJogo *j) {
    j->rodada++;
    j->spotsUsados     = 0;
    j->numSelecionados = 0;

    /* Incrementa permanência */
    Caranguejo *atual = j->fila.inicio;
    while (atual != NULL) {
        atual->rodadasNaColonia++;
        atual = atual->prox;
    }

    /* Fome sobe mais rápido em fases avançadas */
    int delta = 1 + (j->fase / 3);
    filaAumentarFome(&j->fila, delta);

    /* Remove maturos (saída natural) */
    int maturos = filaRemoverMaturos(&j->fila, &j->pontuacao);
    j->caranguejosMaturos += maturos;

    /* Remove mortos */
    int mortos = filaRemoverMortos(&j->fila, &j->pontuacao);
    j->caranguejosMortos += mortos;

    /* Novos caranguejos chegam */
    int chegando = CARANGUEJOS_POR_FASE + (j->fase - 1);
    for (int i = 0; i < chegando; i++) {
        Caranguejo *c = criarCaranguejo(j->idProximo++, 0);
        if (c) filaEnfileirar(&j->fila, c);
    }

    /* Ordena por fome para exibição */
    filaOrdenarPorFome(&j->fila);

    /* Evento aleatório (chance aumenta por fase) */
    int chanceEvento = 30 + j->fase * 10;
    if (j->evento.tipo == EVENTO_NENHUM && (rand() % 100) < chanceEvento) {
        jogoGerarEvento(j);
    }

    /* Verifica fim de fase */
    if (j->rodada >= j->rodadasPorFase) {
        jogoVerificarFaseFim(j);
    }
}

/* ─────────────────────────────────────────────────────────────────
   jogoAlimentarSelecionados
───────────────────────────────────────────────────────────────── */
void jogoAlimentarSelecionados(EstadoJogo *j) {
    Caranguejo *atual = j->fila.inicio;
    while (atual != NULL) {
        for (int i = 0; i < j->numSelecionados; i++) {
            if (j->selecionados[i] == atual->id) {
                atual->fome -= 4;
                if (atual->fome < 0) atual->fome = 0;
                break;
            }
        }
        atual = atual->prox;
    }
    j->spotsUsados     = j->numSelecionados;
    j->numSelecionados = 0;
}

/* ─────────────────────────────────────────────────────────────────
   jogoGerarEvento
───────────────────────────────────────────────────────────────── */
void jogoGerarEvento(EstadoJogo *j) {
    int tipo = rand() % 2;
    j->evento.tipo    = (tipo == 0) ? EVENTO_MARE : EVENTO_REI;
    j->evento.ativo   = 1;
    j->evento.escolha = -1;
    j->tela           = TELA_EVENTO;
}

/* ─────────────────────────────────────────────────────────────────
   jogoAplicarEventoMare
   Opção 0: protege os 3 mais famintos, perde 1 spot esta rodada
   Opção 1: deixa a maré passar, recebe 2 caranguejos extras
───────────────────────────────────────────────────────────────── */
void jogoAplicarEventoMare(EstadoJogo *j, int opcao) {
    if (opcao == 0) {
        /* Reduz fome dos 3 mais famintos */
        for (int k = 0; k < 3; k++) {
            Caranguejo *faminto = filaBuscarMaisFaminto(&j->fila);
            if (faminto) faminto->fome -= 3;
        }
        j->spotsDisponiveis -= 1;
        if (j->spotsDisponiveis < 1) j->spotsDisponiveis = 1;
        snprintf(j->mensagem, 128, "Protegeu os mais fracos! -1 spot nesta fase.");
    } else {
        /* 2 caranguejos extras chegam */
        for (int k = 0; k < 2; k++) {
            Caranguejo *c = criarCaranguejo(j->idProximo++, 0);
            if (c) filaEnfileirar(&j->fila, c);
        }
        snprintf(j->mensagem, 128, "A mare passou... 2 novos caranguejos chegaram!");
    }
    j->evento.tipo  = EVENTO_NENHUM;
    j->evento.ativo = 0;
    j->tempMensagem = 3.0f;
    filaOrdenarPorFome(&j->fila);
    j->tela = TELA_JOGO;
}

/* ─────────────────────────────────────────────────────────────────
   jogoAplicarEventoRei
   Opção 0: aceita o Rei (alto risco, alto retorno)
   Opção 1: recusa o Rei
───────────────────────────────────────────────────────────────── */
void jogoAplicarEventoRei(EstadoJogo *j, int opcao) {
    if (opcao == 0) {
        Caranguejo *rei = criarCaranguejo(j->idProximo++, 1);
        if (rei) filaEnfileirar(&j->fila, rei);
        snprintf(j->mensagem, 128, "O Rei entrou na colonia! Sobreviva com ele...");
    } else {
        snprintf(j->mensagem, 128, "O Rei foi recusado. Colonia segura por ora.");
    }
    j->evento.tipo  = EVENTO_NENHUM;
    j->evento.ativo = 0;
    j->tempMensagem = 3.0f;
    filaOrdenarPorFome(&j->fila);
    j->tela = TELA_JOGO;
}

/* ─────────────────────────────────────────────────────────────────
   jogoVerificarFaseFim
───────────────────────────────────────────────────────────────── */
void jogoVerificarFaseFim(EstadoJogo *j) {
    int metaMaturos = 2 + j->fase;
    int metaMortos  = 3 + j->fase;
    int passou      = (j->caranguejosMaturos >= metaMaturos &&
                       j->caranguejosMortos  <= metaMortos);

    if (passou) {
        /* Recompensa: spot extra permanente */
        j->spotsDisponiveis++;
        j->pontuacao += 20;
    } else {
        /* Penalidade */
        j->pontuacao -= 10;
    }

    j->tela = TELA_FASE_RESULTADO;
}

/* ─────────────────────────────────────────────────────────────────
   jogoAvancarFase
───────────────────────────────────────────────────────────────── */
void jogoAvancarFase(EstadoJogo *j) {
    int passou = (j->caranguejosMaturos >= 2 + j->fase &&
                  j->caranguejosMortos  <= 3 + j->fase);

    if (!passou) {
        /* Duas falhas consecutivas = game over */
        j->tela = TELA_GAME_OVER;
        jogoSalvarPlacar(j);
        return;
    }

    j->fase++;
    j->rodada             = 0;
    j->rodadasPorFase     = 6 + j->fase;
    j->caranguejosMaturos = 0;
    j->caranguejosMortos  = 0;
    j->evento.tipo        = EVENTO_NENHUM;
    j->evento.ativo       = 0;

    /* Novos caranguejos para a nova fase */
    int chegando = CARANGUEJOS_POR_FASE + j->fase - 1;
    for (int i = 0; i < chegando; i++) {
        Caranguejo *c = criarCaranguejo(j->idProximo++, 0);
        if (c) filaEnfileirar(&j->fila, c);
    }
    filaOrdenarPorFome(&j->fila);
    j->tela = TELA_JOGO;
}

/* ─────────────────────────────────────────────────────────────────
   Placar
───────────────────────────────────────────────────────────────── */
void jogoSalvarPlacar(EstadoJogo *j) {
    if (j->numPlacar < MAX_PLACAR) {
        EntradaPlacar *e = &j->placar[j->numPlacar++];
        strncpy(e->nome, j->nomeJogador, NOME_MAX - 1);
        e->pontos = j->pontuacao;
        e->fase   = j->fase;
    } else {
        /* Substitui o pior se o novo for melhor */
        ordenarPlacar(j->placar, j->numPlacar);
        if (j->pontuacao > j->placar[j->numPlacar - 1].pontos) {
            EntradaPlacar *e = &j->placar[j->numPlacar - 1];
            strncpy(e->nome, j->nomeJogador, NOME_MAX - 1);
            e->pontos = j->pontuacao;
            e->fase   = j->fase;
        }
    }
    ordenarPlacar(j->placar, j->numPlacar);

    FILE *f = fopen(ARQUIVO_PLACAR, "wb");
    if (f) {
        fwrite(&j->numPlacar, sizeof(int), 1, f);
        fwrite(j->placar, sizeof(EntradaPlacar), j->numPlacar, f);
        fclose(f);
    }
}

int jogoCarregandoPlacar(EstadoJogo *j) {
    j->numPlacar = 0;
    FILE *f = fopen(ARQUIVO_PLACAR, "rb");
    if (!f) return 0;
    fread(&j->numPlacar, sizeof(int), 1, f);
    if (j->numPlacar > MAX_PLACAR) j->numPlacar = MAX_PLACAR;
    fread(j->placar, sizeof(EntradaPlacar), j->numPlacar, f);
    fclose(f);
    return 1;
}
