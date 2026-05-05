# 🦀 Colônia do Mangue
**Manguezal do Rio Capibaribe — Na Vibe do Recife**

## Como executar

### Pré-requisitos
- GCC (ou MinGW no Windows)
- [Raylib 5.x](https://www.raylib.com/index.html) instalado

### Linux
```bash
sudo apt install libraylib-dev   # Ubuntu/Debian
make
./colonia_do_mangue
```

### macOS
```bash
brew install raylib
make LDFLAGS="-lraylib -lm -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL"
./colonia_do_mangue
```

### Windows (MinGW)
```bash
mingw32-make LDFLAGS="-lraylib -lopengl32 -lgdi32 -lwinmm"
colonia_do_mangue.exe
```

---

## Como jogar

1. Digite seu nome e clique em **JOGAR**
2. Cada rodada, caranguejos chegam à fila da colônia
3. **Clique nos cards** para selecionar quais alimentar (limitado pelos seus spots)
4. Clique **Alimentar** → depois **Próxima Rodada**
5. Tome decisões nos **eventos do mangue** (Maré / Caranguejo-Rei)
6. Cumpra as metas de fase para ganhar recompensas

### Mecânicas principais
| Elemento | Detalhe |
|---|---|
| **Fome** | Sobe todo turno. Se chegar a 10 → morte (−3 pts) |
| **Maturidade** | Após 8 rodadas na colônia → saída natural (+5 pts) |
| **Caranguejo-Rei** | Chega com fome 5. Se madurar → +1 spot permanente |
| **Meta de fase** | Maturos ≥ (2+fase) e Mortos ≤ (3+fase) |
| **Recompensa** | +1 spot permanente +20 pts |
| **Penalidade** | −10 pts; 2 falhas seguidas = game over |

---

## Estrutura do código

```
mangue/
├── include/
│   └── jogo.h          ← structs, constantes, protótipos
├── src/
│   ├── main.c          ← loop principal (Raylib)
│   ├── fila.c          ← ED: fila encadeada + ordenação (Insertion/Selection Sort)
│   ├── logica.c        ← regras do jogo, fases, eventos, placar
│   └── render.c        ← todas as telas desenhadas com Raylib
└── Makefile
```

### Estrutura de dados
**Fila encadeada** — ED central do jogo. Funções implementadas:
1. `filaInicializar` — prepara fila vazia
2. `filaEnfileirar` — caranguejo chega à colônia (FIFO)
3. `filaDesenfileirar` — remove do início
4. `filaAumentarFome` — percorre e incrementa fome (fim de rodada)
5. `filaRemoverMortos` — percorre e remove fome == MAX
6. `filaRemoverMaturos` — percorre e remove rodadas == MAX
7. `filaBuscarMaisFaminto` — percorre e retorna o mais crítico
8. `filaContar` — retorna tamanho
9. `filaOrdenarPorFome` — **Insertion Sort** decrescente por fome
10. `filaLiberar` — libera memória

### Algoritmos de ordenação
- **Insertion Sort** (`filaOrdenarPorFome`) — ordena a fila por fome antes de exibir
- **Selection Sort** (`ordenarPlacar`) — ordena o placar por pontuação decrescente
