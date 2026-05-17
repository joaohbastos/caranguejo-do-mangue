# Guardiões do Mangue

Jogo de gerenciamento estratégico inspirado no ecossistema do Rio Capibaribe e
na cultura recifense, desenvolvido em **linguagem C** com **Raylib** para a
disciplina de Algoritmos e Estruturas de Dados — UFPE 2026.1.

## Sobre o Jogo

No coração do Recife, o Rio Capibaribe abraça um dos manguezais mais
importantes do Nordeste. O jogador assume o papel de guardião de uma colônia
de caranguejos-uçá, decidindo quais serão alimentados a cada rodada com spots
limitados. A fome aumenta a cada turno, eventos inesperados do mangue surgem
ao longo do caminho e o Caranguejo Rei pode aparecer a qualquer momento — cabe
ao jogador manter todos vivos pelo maior número de rodadas possível.

A colônia é uma **lista encadeada simples** reordenada a cada rodada por
**Insertion Sort** (mais faminto primeiro). O ranking de recordes é uma
segunda lista encadeada, também ordenada por Insertion Sort, persistida em
disco entre sessões.

## Como Compilar e Executar

### Pré-requisitos

#### Ubuntu / WSL2

```bash
sudo apt update
sudo apt install build-essential libraylib-dev
```

Se a distribuição não tiver `libraylib-dev`, compile a partir do fonte:

```bash
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
```

#### Windows (MinGW-w64)

1. Instale MSYS2 (https://www.msys2.org/)
2. No terminal MinGW64:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-raylib make
```

#### macOS

```bash
brew install raylib
```

### Build

Existem dois targets independentes:

```bash
# Versao GUI (Raylib) — o entregavel principal
make gui
./caranguejo_gui          # Linux/macOS
./caranguejo_gui.exe      # Windows
```

```bash
# Versao terminal (texto puro) — referencia de logica
make terminal
./caranguejo
```

Para limpar artefatos:

```bash
make clean
```

> Os flags de link em `Makefile` estão configurados para MinGW (Windows).
> Para Linux, troque `GUI_LDFLAGS` para
> `-lraylib -lm -lpthread -ldl -lrt -lX11`.
> Para macOS, use
> `-lraylib -lm -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL`.

## Controles

| Onde         | Ação                                                    |
|--------------|---------------------------------------------------------|
| Menu         | Mouse — clique em JOGAR / EDITAR / SOBRE                |
| Editar       | Teclado preenche o nome; BACKSPACE apaga                |
| Jogo         | "Alimentar" → clicar em um caranguejo gasta 1 spot      |
| Jogo         | "Limpar Mangue" → −3 poluição, gasta 1 spot             |
| Jogo         | "Caçar Siri" → só ativo durante ameaça, gasta 1 spot    |
| Jogo         | "PASSAR RODADA" → encerra a rodada (confirm se spots>0) |
| Em qualquer  | `ESC` cancela modo alimentar e fecha confirmações       |

## Estrutura do Projeto

```
caranguejo-do-mangue/
├── Makefile                       # build terminal + gui
├── README.md
├── ranking.dat                    # gerado em runtime (top 10)
├── assets/
│   ├── sprites/                   # crab.png, crab_king.png, mangue_bg.png
│   └── fonts/                     # PressStart2P-Regular.ttf (opcional)
└── src/
    ├── main.c                     # entry point do terminal
    ├── main_gui.c                 # entry point do GUI (Raylib)
    ├── colonia.{c,h}              # lista da colonia + ordenarPorFome (Insertion Sort)
    ├── ranking.{c,h}              # lista do ranking  + ordenarPlacar  (Insertion Sort)
    ├── jogo.{c,h}                 # logica de rodada e fases
    ├── eventos.{c,h}              # Mare / Siri / Caranguejo Rei
    ├── game_state.{c,h}           # struct GameState
    ├── render_terminal.{c,h}      # renderer texto
    └── screens/                   # camada de UI Raylib
        ├── screen.{c,h}           # state machine + helpers de UI
        ├── theme.h                # paleta de cores
        ├── menu.c
        ├── about.c
        ├── settings.c
        ├── game.c                 # tela de jogo (Raylib)
        └── gameover.c             # ranking + replay
```

## Estrutura de Dados — onde estão

Para o requisito mínimo (lista encadeada + Insertion Sort em posição central):

- **Lista encadeada da colônia** — `src/colonia.h` (struct `Caranguejo`),
  implementação em `src/colonia.c`.
- **Insertion Sort por fome** — `src/colonia.c:103` `ordenarPorFome()`.
- **Lista encadeada do ranking** — `src/ranking.h` (struct `Registro`),
  implementação em `src/ranking.c`.
- **Insertion Sort por pontuação** — `src/ranking.c:23` `ordenarPlacar()`.

Onze funções totais operam sobre essas listas — ver tabela em `CLAUDE.md`.

## Screenshot

> _Adicionar screenshot do gameplay aqui antes da entrega final._

## Equipe

- Arthur Sindeaux
- Arthur Oliveira
- Lucas Pinto
- João Bastos

Desenvolvido para a disciplina de Algoritmos e Estruturas de Dados —
UFPE 2026.1, MOD 02 — Atividade Prática.
