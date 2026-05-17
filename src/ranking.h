#ifndef RANKING_H
#define RANKING_H

#define ARQUIVO_PLACAR "ranking.dat"
#define TOP_PLACAR     10

typedef struct Registro {
    char nome[50];
    int pontuacao;
    struct Registro *proximo;
} Registro;

/* Insere um novo registro no inicio da lista do ranking.
   Retorna 1 em sucesso, 0 em falha de alocacao. */
int registrarPlacar(Registro **inicio, const char *nome, int pontuacao);

/* Ordena a lista por pontuacao decrescente usando Insertion Sort. */
void ordenarPlacar(Registro **inicio);

/* Salva o ranking em ARQUIVO_PLACAR para persistir entre sessoes.
   Retorna 1 em sucesso, 0 em falha de I/O. */
int salvarPlacar(Registro *inicio);

/* Le ARQUIVO_PLACAR (se existir) e popula a lista do ranking.
   Retorna 1 se carregou, 0 se nao havia arquivo. */
int carregarPlacar(Registro **inicio);

/* Libera todos os nos da lista do ranking. */
void liberarPlacar(Registro **inicio);

#endif
