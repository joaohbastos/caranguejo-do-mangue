static int renderEventoRei(GameState *gs) {
    printf("\n[Evento] CARANGUEJO REI aparece no mangue!\n");
    printf("  1. Aceitar (entra na colonia, fome cresce 2x mais rapido;\n");
    printf("              bonus -2 de fome para todos se sobreviver %d rodadas)\n",
           REI_RODADAS_PARA_BONUS);
    printf("  2. Rejeitar (nada acontece)\n");
    printf("Escolha: ");

    int escolha = 0;
    if (!lerInteiro(&escolha)) {
        printf("Entrada invalida. Assumindo Rejeitar.\n");
        return 0;
    }

    if (escolha != 1) {
        printf("O Rei segue seu caminho. Nada acontece.\n");
        return 0;
    }

    int novoId = proximoIdDisponivel(gs->colonia);
    if (!inserirRei(gs, novoId)) {
        printf("Falha ao alocar o Rei.\n");
        return 0;
    }
    printf("Caranguejo Rei id=%d entrou na colonia.\n", novoId);
    return 1;
}

static void renderEventoNenhum(void) {
    printf("\n[Evento] Mangue calmo. Nada de novo nesta rodada.\n");
}

static int renderAplicarEvento(GameState *gs, TipoEvento evento) {
    switch (evento) {
        case EVENTO_MARE:
            renderEventoMare(gs);
            return 0;
        case EVENTO_SIRI:
            renderEventoSiri(gs);
            return 0;
        case EVENTO_REI:
            return renderEventoRei(gs);
        case EVENTO_NENHUM:
        default:
            renderEventoNenhum();
            return 0;
    }
}

static void renderInserirNovosCaranguejos(GameState *gs, int quantos) {
    if (quantos <= 0) {
        return;
    }
    printf("\nChegam %d novo(s) caranguejo(s) ao mangue:\n", quantos);
    for (int i = 0; i < quantos; i++) {
        int idAlocado = 0;
        if (!inserirNovoCaranguejo(gs, &idAlocado)) {
            printf("  Falha ao alocar novo caranguejo.\n");
            return;
        }
        printf("  + Caranguejo id=%d entrou.\n", idAlocado);
    }
}

static void atualizarStatusRei(GameState *gs) {
    Caranguejo *rei = encontrarRei(gs->colonia);
    if (rei != NULL) {
        gs->reiAtivo       = 1;
        gs->reiRodadasVivo = rei->rodadasNaColonia;
    } else {
        gs->reiAtivo       = 0;
        gs->reiRodadasVivo = 0;
    }
}

static int rodadaUnicaTerminal(GameState *gs, ParametrosFase fase, int *reiBonusDado) {
    ordenarPorFome(&gs->colonia);

    printf("\n=== Rodada %d - %s ===\n", gs->rodadaAtual, fase.nome);
    printf("Fome/rodada: +%d | Novos caranguejos: %d | Chance de evento: %d%%\n",
           fase.incrementoFome, fase.novosCaranguejos, fase.chanceEvento);
    printf("Estado da colonia (mais faminto primeiro):\n");
    renderExibirColonia(gs->colonia);

    for (int spot = 1; spot <= SPOTS_POR_RODADA; spot++) {
        gs->spotsRestantes = SPOTS_POR_RODADA - spot + 1;
        int id = 0;
        printf("\nSpot %d/%d - id do caranguejo a alimentar (0 para pular): ",
               spot, SPOTS_POR_RODADA);
        if (!lerInteiro(&id)) {
            printf("Entrada invalida. Spot perdido.\n");
            continue;
        }
        if (id == 0) {
            printf("Spot pulado.\n");
            continue;
        }
        if (!alimentarCaranguejo(gs->colonia, id)) {
            printf("Caranguejo id=%d nao encontrado. Spot perdido.\n", id);
        } else {
            printf("Caranguejo id=%d alimentado.\n", id);
        }
    }
    gs->spotsRestantes = 0;

    if (rand() % 100 < fase.chanceEvento) {
        TipoEvento evento = sortearEvento();
        renderAplicarEvento(gs, evento);
    } else {
        renderAplicarEvento(gs, EVENTO_NENHUM);
    }

    aplicarFomeRodada(gs, fase.incrementoFome);
    printf("\nApos aumentarFome:\n");
    renderExibirColonia(gs->colonia);

    int mortos = verificarMortes(&gs->colonia);
    if (mortos > 0) {
        printf("\nA crab died -- game over\n");
        return 0;
    }

    incrementarRodadasNaColonia(gs);
    atualizarStatusRei(gs);

    Caranguejo *rei = encontrarRei(gs->colonia);
    if (rei != NULL && !*reiBonusDado &&
        rei->rodadasNaColonia >= REI_RODADAS_PARA_BONUS) {
        printf("\n[Caranguejo Rei] Sobreviveu %d rodadas! Bonus para a colonia: -2 de fome em todos.\n",
               REI_RODADAS_PARA_BONUS);
        aplicarBonusRei(gs);
        *reiBonusDado = 1;
    }

    renderInserirNovosCaranguejos(gs, fase.novosCaranguejos);
    return 1;
}

int jogarPartidaTerminal(GameState *gs) {
    if (gs == NULL || gs->colonia == NULL) {
        printf("\nColonia vazia. Nada a jogar.\n");
        return 0;
    }

    int reiBonusDado = 0;
    int faseAnterior = 0;

    for (int n = 1; ; n++) {
        gs->rodadaAtual = n;
        ParametrosFase fase = aplicarFaseAtual(n);
        gs->fase = fase.faseId;

        if (fase.faseId != faseAnterior) {
            if (faseAnterior == 0) {
                printf("\n>>> Inicio da fase: %s <<<\n", fase.nome);
            } else {
                printf("\n>>> Transicao de fase: agora %s! <<<\n", fase.nome);
            }
            faseAnterior = fase.faseId;
        }

        int sobreviveu = rodadaUnicaTerminal(gs, fase, &reiBonusDado);
        if (!sobreviveu) {
            printf("\nFim de jogo na rodada %d.\n", n);
            gs->pontuacao = n - 1;
            return gs->pontuacao;
        }
    }
}
