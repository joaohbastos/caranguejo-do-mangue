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