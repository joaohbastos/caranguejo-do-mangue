# 🦀 Guardiões do Mangue

Jogo de gerenciamento estratégico de colônia de caranguejos, inspirado nos manguezais do Rio Capibaribe — desenvolvido em linguagem C para a disciplina de Algoritmos e Estruturas de Dados (AED 2026.1 — MOD 02), dentro da temática **"Na Vibe do Recife"**.

---

## 📖 Sobre o Jogo

No coração do Recife, o Rio Capibaribe abraça um dos manguezais mais importantes do Nordeste. O jogador assume o papel de **guardião de uma colônia de caranguejos**, decidindo quais serão alimentados a cada rodada com spots limitados.

A fome aumenta a cada turno, eventos inesperados surgem ao longo do caminho — como a Maré Cheia e o Siri Invasor — e o temido **Caranguejo Rei** pode aparecer a qualquer momento. Cabe ao jogador manter todos os caranguejos vivos pelo maior número de rodadas possível. O mangue não perdoa descuido.

---

## 🎮 Como Jogar

### Compilação

```bash
# Clonar o repositório
git clone https://github.com/joaohbastos/caranguejo-do-mangue.git
cd caranguejo-do-mangue

# Compilar (versão com interface gráfica)
make gui
```

> **Pré-requisitos:** GCC, Make e Raylib instalados.  
> Em sistemas Debian/Ubuntu: `sudo apt install build-essential libraylib-dev`

### Execução

```bash
./caranguejo_gui
```

### Dinâmica de uma partida

1. Informe seu nome para o ranking
2. A cada rodada, visualize a colônia **ordenada por nível de fome** (mais faminto primeiro)
3. Escolha quais caranguejos alimentar dentro do limite de **spots disponíveis**
4. Resolva o **evento do mangue** da rodada (Maré, Siri Invasor, Caranguejo Rei...)
5. A fome de todos os caranguejos aumenta ao fim da rodada
6. O jogo termina quando qualquer caranguejo morre de fome
7. Sua pontuação é registrada no **ranking de recordes**

### Fases da partida

| Fase | Rodadas | Descrição |
|---|---|---|
| 🌊 Maré Baixa | 1 – 5 | Introdução calma; fome cresce devagar |
| 🌊🌊 Maré Cheia | 6 – 10 | Pressão aumenta; mais eventos e caranguejos novos |
| ⛈️ Tempestade no Capibaribe | 11+ | Caos total; evento toda rodada; sobrevivência pura |


---

## 🧱 Estruturas de Dados e Algoritmos

### Lista Encadeada Simples — Colônia

Estrutura central do jogo. Cada nó representa um caranguejo vivo:

```c
typedef struct Caranguejo {
    int id;
    int nivelFome;          // 0 = saciado; MAX_FOME = morte
    int rodadasNaColonia;
    int ehRei;              // 1 = Caranguejo Rei
    struct Caranguejo *proximo;
} Caranguejo;
```

### Lista Encadeada Simples — Ranking

Armazena os recordes (nome do jogador + pontuação). Reordenada após cada partida.

### Insertion Sort

Utilizado em dois contextos:
- **Durante o jogo:** ordena a colônia por nível de fome ao fim de cada rodada (mais faminto primeiro)
- **Ao final da partida:** ordena o ranking por pontuação (maior primeiro)

---

## ⚙️ Comandos do Makefile

```bash
make gui      # Compila a versão com interface gráfica
make terminal # Compila a versão terminal
make clean    # Remove os arquivos compilados
```

---

## 👥 Equipe

| Nome |
|---|
| Arthur Sindeaux |
| Arthur Oliveira |
| Lucas Pinto |
| João Bastos |

Desenvolvido para a disciplina de **Algoritmos e Estruturas de Dados — AED 2026.1**  
Universidade — MOD 02 | Temática: *Na Vibe do Recife* 🦀
