#ifndef RANKING_H
#define RANKING_H

#define ARQUIVO_PLACAR "placar.txt"
#define TOP_PLACAR     10

typedef struct Registro {
    char nome[50];
    int pontuacao;
    struct Registro *proximo;
} Registro;

/* Insere um novo registro no inicio da lista do ranking. */
void registrarPlacar(Registro **inicio, const char *nome, int pontuacao);

/* Ordena a lista por pontuacao decrescente usando Insertion Sort. */
void ordenarPlacar(Registro **inicio);

/* Exibe os TOP_PLACAR melhores registros do ranking. */
void exibirPlacar(Registro *inicio);

/* Salva o ranking em ARQUIVO_PLACAR para persistir entre sessoes. */
void salvarPlacar(Registro *inicio);

/* Le ARQUIVO_PLACAR (se existir) e popula a lista do ranking. */
void carregarPlacar(Registro **inicio);

/* Libera todos os nos da lista do ranking. */
void liberarPlacar(Registro **inicio);

#endif
