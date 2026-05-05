#include "../include/jogo.h"

/* ── Nomes temáticos para os caranguejos ── */
static const char *NOMES[] = {
    "Zé do Mangue", "Caetano", "Seu Crustáceo", "Dona Garra",
    "Maracatú",     "Frevinho", "Pitombeira",   "Zumbi",
    "Oliveira",     "Choque Peso", "Nêgo d'Água",  "Catarina",
    "Sindô",        "Beno Biba", "Mangue Bit",   "Capibinha"
};
#define NUM_NOMES 16

/* ────────────────────────────────────────────────────────────────
   FUNÇÃO 1 — filaInicializar
   Prepara a fila vazia antes do jogo começar.
───────────────────────────────────────────────────────────────── */
void filaInicializar(FilaCaranguejo *f) {
    f->inicio  = NULL;
    f->fim     = NULL;
    f->tamanho = 0;
}

/* ────────────────────────────────────────────────────────────────
   FUNÇÃO 2 — filaEnfileirar
   Adiciona um caranguejo no fim da fila (chegou à colônia).
───────────────────────────────────────────────────────────────── */
void filaEnfileirar(FilaCaranguejo *f, Caranguejo *c) {
    c->prox = NULL;
    if (f->fim == NULL) {
        f->inicio = c;
        f->fim    = c;
    } else {
        f->fim->prox = c;
        f->fim       = c;
    }
    f->tamanho++;
}

/* ────────────────────────────────────────────────────────────────
   FUNÇÃO 3 — filaDesenfileirar
   Remove e retorna o caranguejo do início da fila.
───────────────────────────────────────────────────────────────── */
Caranguejo *filaDesenfileirar(FilaCaranguejo *f) {
    if (f->inicio == NULL) return NULL;
    Caranguejo *removido = f->inicio;
    f->inicio = f->inicio->prox;
    if (f->inicio == NULL) f->fim = NULL;
    removido->prox = NULL;
    f->tamanho--;
    return removido;
}

/* ────────────────────────────────────────────────────────────────
   FUNÇÃO 4 — filaAumentarFome
   Percorre toda a fila incrementando a fome de cada caranguejo.
   Usado ao final de cada rodada.
───────────────────────────────────────────────────────────────── */
void filaAumentarFome(FilaCaranguejo *f, int delta) {
    Caranguejo *atual = f->inicio;
    while (atual != NULL) {
        atual->fome += delta;
        if (atual->fome > FOME_MAXIMA) atual->fome = FOME_MAXIMA;
        atual = atual->prox;
    }
}

/* ────────────────────────────────────────────────────────────────
   FUNÇÃO 5 — filaRemoverMortos
   Percorre a fila removendo caranguejos com fome == FOME_MAXIMA.
   Retorna quantos morreram e desconta pontos.
───────────────────────────────────────────────────────────────── */
int filaRemoverMortos(FilaCaranguejo *f, int *pontos) {
    int mortos = 0;
    Caranguejo *atual  = f->inicio;
    Caranguejo *ant    = NULL;

    while (atual != NULL) {
        if (atual->fome >= FOME_MAXIMA) {
            Caranguejo *morto = atual;
            if (ant == NULL) {
                f->inicio = atual->prox;
                if (f->inicio == NULL) f->fim = NULL;
            } else {
                ant->prox = atual->prox;
                if (ant->prox == NULL) f->fim = ant;
            }
            atual = atual->prox;
            *pontos -= 3;
            mortos++;
            free(morto);
            f->tamanho--;
        } else {
            ant   = atual;
            atual = atual->prox;
        }
    }
    return mortos;
}

/* ────────────────────────────────────────────────────────────────
   FUNÇÃO 6 — filaRemoverMaturos
   Percorre a fila removendo caranguejos que atingiram
   RODADAS_MATURIDADE. Cada um soma pontos ao placar.
───────────────────────────────────────────────────────────────── */
int filaRemoverMaturos(FilaCaranguejo *f, int *pontos) {
    int maturos = 0;
    Caranguejo *atual = f->inicio;
    Caranguejo *ant   = NULL;

    while (atual != NULL) {
        if (atual->rodadasNaColonia >= RODADAS_MATURIDADE) {
            Caranguejo *maduro = atual;
            if (ant == NULL) {
                f->inicio = atual->prox;
                if (f->inicio == NULL) f->fim = NULL;
            } else {
                ant->prox = atual->prox;
                if (ant->prox == NULL) f->fim = ant;
            }
            atual = atual->prox;
            *pontos += maduro->ehRei ? 10 : 5;
            maturos++;
            free(maduro);
            f->tamanho--;
        } else {
            ant   = atual;
            atual = atual->prox;
        }
    }
    return maturos;
}

/* ────────────────────────────────────────────────────────────────
   FUNÇÃO 7 — filaBuscarMaisFaminto
   Percorre a fila e retorna ponteiro para o caranguejo
   com maior nível de fome (útil para eventos e decisões de IA).
───────────────────────────────────────────────────────────────── */
Caranguejo *filaBuscarMaisFaminto(FilaCaranguejo *f) {
    if (f->inicio == NULL) return NULL;
    Caranguejo *maisFaminto = f->inicio;
    Caranguejo *atual       = f->inicio->prox;
    while (atual != NULL) {
        if (atual->fome > maisFaminto->fome)
            maisFaminto = atual;
        atual = atual->prox;
    }
    return maisFaminto;
}

/* ────────────────────────────────────────────────────────────────
   FUNÇÃO 8 — filaContar
   Retorna o número de caranguejos na fila.
───────────────────────────────────────────────────────────────── */
int filaContar(FilaCaranguejo *f) {
    return f->tamanho;
}

/* ────────────────────────────────────────────────────────────────
   FUNÇÃO 9 — filaOrdenarPorFome  (INSERTION SORT)
   Reconstrói a fila ordenada por fome decrescente.
   O jogador visualiza os mais críticos primeiro.
───────────────────────────────────────────────────────────────── */
void filaOrdenarPorFome(FilaCaranguejo *f) {
    if (f->tamanho <= 1) return;

    /* Coleta ponteiros em array auxiliar */
    int n = f->tamanho;
    Caranguejo **arr = (Caranguejo **)malloc(n * sizeof(Caranguejo *));
    if (!arr) return;

    Caranguejo *atual = f->inicio;
    for (int i = 0; i < n; i++) {
        arr[i] = atual;
        atual  = atual->prox;
    }

    /* Insertion Sort — fome decrescente */
    for (int i = 1; i < n; i++) {
        Caranguejo *chave = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j]->fome < chave->fome) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = chave;
    }

    /* Reconstrói a fila com nova ordem */
    f->inicio = arr[0];
    for (int i = 0; i < n - 1; i++)
        arr[i]->prox = arr[i + 1];
    arr[n - 1]->prox = NULL;
    f->fim = arr[n - 1];

    free(arr);
}

/* ────────────────────────────────────────────────────────────────
   FUNÇÃO 10 — filaLiberar
   Libera toda a memória da fila ao encerrar o jogo.
───────────────────────────────────────────────────────────────── */
void filaLiberar(FilaCaranguejo *f) {
    Caranguejo *atual = f->inicio;
    while (atual != NULL) {
        Caranguejo *prox = atual->prox;
        free(atual);
        atual = prox;
    }
    filaInicializar(f);
}

/* ────────────────────────────────────────────────────────────────
   SELECTION SORT — ordena o placar por pontuação decrescente
───────────────────────────────────────────────────────────────── */
void ordenarPlacar(EntradaPlacar *placar, int n) {
    for (int i = 0; i < n - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (placar[j].pontos > placar[maxIdx].pontos)
                maxIdx = j;
        }
        if (maxIdx != i) {
            EntradaPlacar tmp = placar[i];
            placar[i]        = placar[maxIdx];
            placar[maxIdx]   = tmp;
        }
    }
}

/* ────────────────────────────────────────────────────────────────
   Cria um novo caranguejo alocado na heap
───────────────────────────────────────────────────────────────── */
Caranguejo *criarCaranguejo(int id, int ehRei) {
    Caranguejo *c = (Caranguejo *)malloc(sizeof(Caranguejo));
    if (!c) return NULL;
    c->id               = id;
    c->fome             = ehRei ? 5 : (rand() % 3);   /* rei chega faminto */
    c->rodadasNaColonia = 0;
    c->ehRei            = ehRei;
    c->prox             = NULL;
    strncpy(c->nome, ehRei ? "Caranguejo-Rei 👑" : NOMES[id % NUM_NOMES], NOME_MAX - 1);
    return c;
}
