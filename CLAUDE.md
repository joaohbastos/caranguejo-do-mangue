<!--
## Stage 0 Audit (2026-05-16)

Pre-Raylib migration audit. Codebase compiles clean and the terminal game runs
end-to-end. No logic was changed in this stage.

### 1. Files in `src/`

| File | One-line description |
|------|----------------------|
| `colonia.h` | `Caranguejo` node typedef, `MAX_FOME`, and the public API for the colony linked list. |
| `colonia.c` | Linked-list operations for the colony: create / insert / remove, hunger update, death sweep, insertion-sort by hunger, and table display. |
| `jogo.h` | `ParametrosFase` struct, `SPOTS_POR_RODADA`, and the round/match API (`alimentarCaranguejo`, `aplicarFaseAtual`, `jogarPartida`). |
| `jogo.c` | Round loop: sort colony, prompt for 3 feeding spots, roll/apply an event, age hunger, kill the dead, age survivors, spawn new crabs, and detect game over. |
| `eventos.h` | `TipoEvento` enum, `REI_RODADAS_PARA_BONUS`, and the event API (`sortearEvento`, `aplicarEscolha`, `gerenciarRei`). |
| `eventos.c` | Event implementations: `MARE` (hunger +1 all), `SIRI` (interactive hunt/ignore), `REI` (interactive accept/reject + insert king crab). |
| `ranking.h` | `Registro` node typedef, file/top constants, and the ranking-list API. |
| `ranking.c` | Ranking linked list: head insert, insertion-sort by score, table display, plus `placar.txt` save/load and free. |
| `main.c` | Menu loop, RNG seed, initial colony seeding, name capture via `fgets`, and ranking lifecycle (load/register/sort/display/save). |

### 2. Colony linked-list functions

`Caranguejo` is the central node type; the table covers every function that
walks or mutates the colony list.

| File | Line | Signature | I/O inside? | Return style |
|------|------|-----------|-------------|--------------|
| `src/colonia.c` | 6 | `Caranguejo *criarCaranguejo(int id, int ehRei)` | no | returns pointer (new node) |
| `src/colonia.c` | 19 | `void inserirCaranguejo(Caranguejo **inicio, Caranguejo *novo)` | no | mutates via `**inicio` |
| `src/colonia.c` | 37 | `int removerCaranguejo(Caranguejo **inicio, int id)` | no | mutates via `**inicio`; returns flag |
| `src/colonia.c` | 61 | `void exibirColonia(Caranguejo *inicio)` | **yes** (`printf`) | read-only traversal |
| `src/colonia.c` | 83 | `void aumentarFome(Caranguejo *inicio)` | no | mutates nodes in place |
| `src/colonia.c` | 94 | `int verificarMortes(Caranguejo **inicio)` | no | mutates via `**inicio`; returns death count |
| `src/colonia.c` | 126 | `void ordenarPorFome(Caranguejo **inicio)` | no | mutates via `**inicio` (new head pointer) |
| `src/jogo.c` | 23 | `int alimentarCaranguejo(Caranguejo *inicio, int id)` | no | mutates target node; returns flag |
| `src/jogo.c` | 62 | `static int proximoIdDisponivel(Caranguejo *inicio)` | no | pure read; returns int |
| `src/jogo.c` | 72 | `static Caranguejo *encontrarRei(Caranguejo *inicio)` | no | returns pointer (or NULL) |
| `src/jogo.c` | 83 | `static void aplicarFomeRodada(Caranguejo *inicio, int delta)` | no | mutates nodes in place |
| `src/jogo.c` | 93 | `static void aplicarBonusRei(Caranguejo *inicio)` | **yes** (`printf`) | mutates nodes in place |
| `src/jogo.c` | 104 | `static void incrementarRodadasNaColonia(Caranguejo *inicio)` | no | mutates nodes in place |
| `src/jogo.c` | 110 | `static void inserirNovosCaranguejos(Caranguejo **inicio, int *proximoId, int quantos)` | **yes** (`printf`) | mutates via `**inicio` |
| `src/jogo.c` | 127 | `static int rodadaUnica(Caranguejo **inicio, int numeroRodada, ParametrosFase fase, int *proximoId, int *reiBonusDado)` | **yes** (`printf` / `scanf` via `lerInteiro`) | mutates via `**inicio`; returns survival flag |
| `src/jogo.c` | 189 | `int jogarPartida(Caranguejo **inicio)` | **yes** (`printf`) | mutates via `**inicio`; returns rounds survived |
| `src/eventos.c` | 9 | `static int contarColonia(Caranguejo *inicio)` | no | pure read; returns count |
| `src/eventos.c` | 17 | `static Caranguejo *caranguejoNoIndice(Caranguejo *inicio, int indice)` | no | returns pointer (or NULL) |
| `src/eventos.c` | 26 | `static void somarFome(Caranguejo *alvo, int delta)` | no | mutates single node |
| `src/eventos.c` | 49 | `static void aplicarMare(Caranguejo *inicio)` | **yes** (`printf`) | mutates nodes in place |
| `src/eventos.c` | 57 | `static void aplicarSiri(Caranguejo *inicio)` | **yes** (`printf` + `scanf`) | mutates nodes in place |
| `src/eventos.c` | 98 | `int gerenciarRei(Caranguejo **inicio, int proximoId)` | **yes** (`printf` + `scanf`) | mutates via `**inicio`; returns accept flag |
| `src/eventos.c` | 129 | `int aplicarEscolha(Caranguejo **inicio, TipoEvento evento, int proximoId)` | indirect (delegates to handlers above) | dispatches; returns flag from `gerenciarRei` |
| `src/main.c` | 102 | `static void iniciarPartida(Registro **ranking)` | **yes** (`printf`, indirect `scanf`) | mutates ranking list; allocates colony |
| `src/main.c` | 135 | `static void liberarColonia(Caranguejo **inicio)` | no | mutates via `**inicio` |

Pattern summary: ownership-changing operations take `Caranguejo **inicio`
(insert/remove/sort/death sweep). Read-only and per-node mutation operations
take a plain `Caranguejo *`. I/O is concentrated in the `jogo.c` /
`eventos.c` interactive layer; the data-structure layer in `colonia.c` only
prints inside `exibirColonia`.

### 3. Insertion Sort presence

**(a) Colony by hunger** — `ordenarPorFome` in `src/colonia.c:126-154`. Hunger
sorted descending (hungriest first) by removing each node from the input list
and splicing it into the correct position of a growing `ordenada` list:

```c
void ordenarPorFome(Caranguejo **inicio) {
    if (inicio == NULL || *inicio == NULL || (*inicio)->proximo == NULL) {
        return;
    }

    Caranguejo *ordenada = NULL;
    Caranguejo *atual    = *inicio;

    while (atual != NULL) {
        Caranguejo *proximo = atual->proximo;

        if (ordenada == NULL || ordenada->nivelFome < atual->nivelFome) {
            atual->proximo = ordenada;
            ordenada       = atual;
        } else {
            Caranguejo *busca = ordenada;
            while (busca->proximo != NULL &&
                   busca->proximo->nivelFome >= atual->nivelFome) {
                busca = busca->proximo;
            }
            atual->proximo = busca->proximo;
            busca->proximo = atual;
        }

        atual = proximo;
    }

    *inicio = ordenada;
}
```

**(b) Ranking by score** — `ordenarPlacar` in `src/ranking.c:23-49`. Same
insertion-sort shape, score descending:

```c
void ordenarPlacar(Registro **inicio) {
    if (inicio == NULL || *inicio == NULL || (*inicio)->proximo == NULL) {
        return;
    }

    Registro *ordenada = NULL;
    Registro *atual    = *inicio;

    while (atual != NULL) {
        Registro *proxima = atual->proximo;

        if (ordenada == NULL || ordenada->pontuacao < atual->pontuacao) {
            atual->proximo = ordenada;
            ordenada = atual;
        } else {
            Registro *p = ordenada;
            while (p->proximo != NULL && p->proximo->pontuacao >= atual->pontuacao) {
                p = p->proximo;
            }
            atual->proximo = p->proximo;
            p->proximo = atual;
        }
        atual = proxima;
    }

    *inicio = ordenada;
}
```

Both are textbook in-place linked-list insertion sort. Requirement satisfied.

### 4. Build (`make clean && make`)

Toolchain: `gcc -Wall -Wextra -std=c99 -Isrc`.

```
rm -f src/main.o src/colonia.o src/jogo.o src/eventos.o src/ranking.o caranguejo
gcc -Wall -Wextra -std=c99 -Isrc -c src/main.c -o src/main.o
gcc -Wall -Wextra -std=c99 -Isrc -c src/colonia.c -o src/colonia.o
gcc -Wall -Wextra -std=c99 -Isrc -c src/jogo.c -o src/jogo.o
gcc -Wall -Wextra -std=c99 -Isrc -c src/eventos.c -o src/eventos.o
gcc -Wall -Wextra -std=c99 -Isrc -c src/ranking.c -o src/ranking.o
gcc -Wall -Wextra -std=c99 -Isrc src/*.o -o caranguejo
```

Zero warnings, zero errors. Nothing to fix in this stage.

### 5. End-to-end smoke test

Ran `./caranguejo` with scripted stdin, drove the menu into a fresh match, fed
crabs through phase-1 rounds, hit an event in round 3, and triggered a death.

**Works:**
- Main menu (`Jogar` / `Configuracoes` / `Sobre` / `Sair`) renders and routes.
- Initial colony seeded with 4 crabs (ids 1–4) and ascending hunger.
- Each round: sort by hunger, render colony table, prompt 3 spots, apply
  feeding, optionally roll/apply event, age hunger, sweep deaths, spawn new
  crab.
- Phase 1 ("Mare Baixa") parameters applied (+1 hunger, +1 new crab,
  33% event chance).
- Event triggered (`SIRI INVASOR`) with interactive prompt and randomised
  victim selection.
- Death detection: when crab 4 reached `5/5` after the SIRI hit, `verificarMortes`
  caught it, printed `A crab died -- game over`, and ended the match on round 3.
- Final score path: `Partida encerrada. Sua pontuacao: 2 rodada(s) sobrevivida(s).`
- `registrarPlacar` + `ordenarPlacar` + `exibirPlacar` produced a TOP 10 board.
- `salvarPlacar` wrote the new line into `placar.txt`.
- Return to menu and clean exit on option 4.

**Notes / known sharp edges (not fixed in this stage):**
- `[Configuracoes]` and `[Sobre]` are stubs ("Em construcao."). Expected per
  the spec.
- The game-over message `"A crab died -- game over"` is in English while
  everything else in the UI is Portuguese. Cosmetic; left as-is.
- Player name capture works via interactive `fgets`. When stdin is piped and
  event prompts shift consumption, a leftover spot value can land in the name
  buffer (observed in the scripted run, where the saved name became `"0"`).
  This is a scripting artifact, not a bug in real interactive play.
- `placar.txt` already existed (`0 1`) and was preserved across the test run.

### Open items for the Raylib migration

- I/O is interleaved with linked-list logic in `jogo.c`/`eventos.c` (every
  prompt is a `scanf`+`getchar` pair). Separating "compute next state" from
  "render / read input" will be the main refactor before plugging in Raylib.
- `colonia.c` is already pure data-structure code except for `exibirColonia`,
  which should become a renderer in the new layer.
- The match loop in `jogarPartida` is an unbounded `for` driven by blocking
  reads — it will need to be turned into a per-frame state machine.

-->

 Contexto do Projeto para Claude Code
Identidade do Projeto
CampoValorNome do jogoGuardiões do MangueDisciplinaAlgoritmos e Estruturas de Dados — AED 2026.1MóduloMOD 02 — Atividade PráticaTema"Na Vibe do Recife" — Manguezais e Rio CapibaribeLinguagemCInterfaceTerminal (texto) — sem interface gráficaIA no jogoNão
Datas importantes
EntregaDataDescriçãoEntrega 0107/05/2026Especificação (já entregue)Entrega 0228/05/2026 até 07:59Código fonte (.zip) + README + vídeoApresentação 0128/05/2026Apresentação para a turma (5 min)Apresentação 02A definirApresentação individual para a professora
Conceito do Jogo
Jogo de gestão de colônia de caranguejos nos manguezais do Rio Capibaribe, Recife.
O jogador é guardião de uma colônia e deve decidir quais caranguejos alimentar a cada rodada.
Novos caranguejos chegam constantemente, a fome se acumula e os spots de alimentação são limitados.
O jogo termina quando qualquer caranguejo morre de fome.
Inspiração de gameplay: Diner Dash com tema de gestão de colônia de caranguejos.
Objetivo do Jogo
Manter todos os caranguejos vivos pelo maior número de rodadas possível.
Pontuação acumulada por:

Rodadas sobrevividas
Streak de boas decisões consecutivas
Gerenciamento eficiente do Caranguejo Rei
Resolução de eventos do mangue com menor custo à colônia

O jogo encerra quando qualquer caranguejo morre de fome. A pontuação é registrada no ranking.
Estrutura de Dados
Lista Encadeada Simples — Colônia (estrutura CENTRAL do jogo)
Cada nó representa um caranguejo vivo.
ctypedef struct Caranguejo {
    int id;               // identificador único
    int nivelFome;        // 0 = saciado; MAX_FOME = morte
    int rodadasNaColonia; // contador de permanência
    int ehRei;            // 1 = Caranguejo Rei
    struct Caranguejo *proximo;
} Caranguejo;
Justificativa: colônia dinâmica (inserções e remoções frequentes em qualquer posição),
jogador acessa qualquer caranguejo por id, sem desperdício de memória como em vetor.
Lista Encadeada Simples — Ranking
Armazena recordes (nome do jogador + pontuação final).
Reordenada após cada partida com Insertion Sort.
Algoritmo de Ordenação
Insertion Sort — usado em dois contextos:

Durante o jogo: ordena a colônia por nível de fome (mais faminto primeiro) ao fim de cada rodada.
Ao fim da partida: ordena o ranking por pontuação (maior primeiro).

Justificativa: lista quase-ordenada entre rodadas (O(n) no melhor caso),
colônia pequena (< 20 caranguejos), implementação direta em listas encadeadas.
Funções — Tabela Completa
FunçãoDescriçãoTipocriarCaranguejo()Aloca e inicializa novo nó com atributos do caranguejoEDinserirCaranguejo()Insere novo nó na lista da colôniaEDremoverCaranguejo()Remove nó do caranguejo morto, libera memóriaEDalimentarCaranguejo()Percorre lista e reduz fome do nó escolhidoEDaumentarFome()Percorre lista e incrementa fome de todos os nósEDordenarPorFome()Reordena lista por nível de fome (Insertion Sort)EDexibirColonia()Percorre lista e exibe estado atual de cada nóEDverificarMortes()Percorre lista e remove nós com fome máximaEDregistrarPlacar()Insere pontuação final na lista de recordesEDordenarPlacar()Ordena ranking por pontuação (Insertion Sort)EDgerenciarRei()Controla ciclo de vida do Caranguejo Rei e bônusEDsortearEvento()Sorteia evento aleatório do mangue a cada rodadaLógicaaplicarEscolha()Aplica consequência da decisão do jogadorLógicalimparMangue()Reduz nível de poluição da colôniaLógicacacarSiri()Resolve evento do Siri InvasorLógicaaplicarFaseAtual()Ajusta parâmetros conforme fase da partidaLógica
Total de funções envolvendo ED: 11 (requisito mínimo: 6)
Loop de uma Rodada
1. Exibir colônia ordenada por fome (ordenarPorFome + exibirColonia)
2. Exibir painel: número da rodada, fase, poluição, spots disponíveis, status do Rei
3. Apresentar ações disponíveis ao jogador
4. Jogador escolhe ações dentro do limite de spots
5. Aplicar efeitos das ações (aplicarEscolha)
6. Sortear e aplicar evento da rodada (sortearEvento)
7. Aumentar fome de todos (aumentarFome)
8. Verificar e remover mortos (verificarMortes)
9. Inserir novos caranguejos (inserirCaranguejo)
10. Próxima rodada
Ações Disponíveis ao Jogador (3 spots por rodada — sugestão inicial)

Alimentar Caranguejo: reduz fome de um caranguejo escolhido. Ação core, sempre disponível.
Limpar o Mangue: reduz nível de poluição. Poluição alta causa fome extra em todos na próxima rodada.
Caçar Siri Invasor: aparece como evento. Se ignorado, aumenta fome de alguns caranguejos.

Fases do Jogo (anti-monotonia)
FaseRodadasDescriçãoMaré Baixa1–5Introdução calma, fome cresce devagarMaré Cheia6–10Pressão aumenta, mais caranguejos, eventos frequentesTempestade no Capibaribe11+Caos total, evento toda rodada, sobrevivência pura
Implementação: if no número da rodada ajusta taxaFome, caranguejosPorRodada e chanceEvento.
Caranguejo Rei (mecânica especial)

Aparece como evento periódico. Jogador decide aceitar ou rejeitar.
Se aceito: entra na colônia com fome que cresce mais rápido OU gera mais poluição passivamente.
Se sobreviver X rodadas (sugestão: 5): bônus de alimentação coletiva grátis e/ou pontuação extra.
Mecânica de risco calculado — aceitar pode salvar ou destruir a colônia.

Eventos do Mangue
Comuns

Maré: traz lixo (aumenta poluição) OU aumenta fome de todos.
Siri Invasor: gastar 1 ação para expulsar, ou ignorar e ter fome extra em alguns caranguejos.
Aparição do Caranguejo Rei: mecânica especial descrita acima.

Raros (a cada ~10 rodadas)

Tempestade no Capibaribe: fome alta em todos + impacto visual marcante.
Festival de Caranguejada no Recife: sacrificar 1 caranguejo para ganhar pontos extras, ou recusar e perder 2 aleatórios.

Parâmetros a Definir via Prototipagem
ParâmetroSugestão inicialMAX_FOME (morte)5Caranguejos iniciais4Spots por rodada3Rodadas para bônus do Rei5Segundos no Modo Mangue (timer)15sTaxa de fome por faseA definir no playtest
Sistema de Pontuação
Fontes de pontos (valores exatos definidos no playtest):

Cada rodada sobrevivida
Streak de boas decisões (não morrer ninguém + poluição não estourar)
Gerenciamento bem-sucedido do Caranguejo Rei
Resolução de eventos raros sem perdas

Fluxo do Menu Principal
Menu Principal
├── Jogar       → pede nome do jogador → inicia partida
├── Editar      → alterar nome padrão, resetar ranking, modo timer
└── Sobre       → contexto cultural do mangue, instruções, créditos
Requisitos Não Funcionais (da professora)

Código fonte devidamente indentado
Identificadores de variáveis significativos (evitar x1, a1, y)
Código comentado e documentado
Linguagem C
Menu interativo obrigatório
ED deve fazer parte da lógica central do jogo (não apenas armazenar dados)

Checklist de Requisitos

 Linguagem C
 Menu interativo
 Estrutura de dados como parte central da lógica
 Algoritmo de ordenação (Insertion Sort)
 Mínimo 6 funções envolvendo a ED (temos 11)
 Especificação das funções principais
 Interface gráfica — NÃO implementada (sem bônus de +0,5 pt)
 IA na dinâmica — NÃO utilizada

Estrutura de Arquivos Sugerida
caranguejo-do-mangue/
├── CLAUDE.md           ← este arquivo
├── README.md           ← como compilar e executar (obrigatório na Entrega 02)
├── src/
│   ├── main.c
│   ├── colonia.c       ← funções da lista da colônia
│   ├── colonia.h
│   ├── ranking.c       ← funções da lista de ranking
│   ├── ranking.h
│   ├── eventos.c       ← sortearEvento, aplicarEscolha, etc.
│   ├── eventos.h
│   ├── jogo.c          ← loop principal do jogo, fases
│   └── jogo.h
└── Makefile
Notas para o Desenvolvedor

Prioridade máxima: simplicidade de implementação
A equipe tem 4 integrantes
Cada integrante será avaliado individualmente na Apresentação 02 — todos precisam entender o código
Trabalhos com lógica copiada são anulados em todas as cópias
Entregas fora do prazo não são aceitas