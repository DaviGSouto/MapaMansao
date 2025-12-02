#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tamanho máximo para nomes e pistas
#define MAX_NOME 50
#define MAX_PISTA 100

// --- Estruturas de Dados ---

// 1. Estrutura para o Mapa da Mansão (Árvore Binária Simples)
typedef struct Sala {
    char nome[MAX_NOME];       // Nome do cômodo (ex: "Hall de entrada")
    char pista[MAX_PISTA];     // Pista associada a este cômodo (pode ser vazia)
    struct Sala *esquerda;     // Próxima sala à esquerda
    struct Sala *direita;      // Próxima sala à direita
} Sala;

// 2. Estrutura para a Árvore de Pistas Coletadas (BST)
typedef struct PistaNode {
    char pista[MAX_PISTA];     // Conteúdo da pista
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// Variável global para a raiz da BST de Pistas (simplificação do nível)
PistaNode *raizPistas = NULL;

// --- Protótipos das Funções ---

Sala* criarSala(const char *nome, const char *pista);
PistaNode* inserirPista(PistaNode *raiz, const char *pista);
void explorarSalasComPistas(Sala *atual);
void exibirPistas(PistaNode *raiz);
void liberarMapa(Sala *raiz);
void liberarPistas(PistaNode *raiz);

// --- Implementação das Funções ---

/**
 * @brief Cria dinamicamente uma nova sala com o nome e a pista especificados.
 * * Utiliza malloc para alocar memória e inicializa os campos.
 * * @param nome O nome da sala.
 * * @param pista O texto da pista (use "" se não houver pista).
 * @return Um ponteiro para a Sala recém-criada, ou NULL em caso de falha.
 */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *novaSala = (Sala *)malloc(sizeof(Sala));
    if (novaSala != NULL) {
        // Inicializa os campos da Sala
        strncpy(novaSala->nome, nome, MAX_NOME - 1);
        novaSala->nome[MAX_NOME - 1] = '\0';
        
        strncpy(novaSala->pista, pista, MAX_PISTA - 1);
        novaSala->pista[MAX_PISTA - 1] = '\0';
        
        novaSala->esquerda = NULL;
        novaSala->direita = NULL;
    }
    return novaSala;
}

// -------------------------------------------------------------------

/**
 * @brief Insere uma nova pista na Árvore Binária de Busca (BST).
 * * Utiliza **recursividade** para encontrar o local correto de inserção,
 * garantindo que a ordem alfabética seja mantida.
 * * @param raiz A raiz da subárvore atual da BST.
 * * @param pista O conteúdo da pista a ser inserida.
 * @return O ponteiro para a nova raiz da subárvore (ou a raiz inalterada).
 */
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    // 1. Caso base: se a raiz for NULL, cria um novo nó
    if (raiz == NULL) {
        PistaNode *novoNode = (PistaNode *)malloc(sizeof(PistaNode));
        if (novoNode == NULL) {
            fprintf(stderr, "Erro: Falha na alocacao de memoria para a pista.\n");
            return NULL;
        }
        strncpy(novoNode->pista, pista, MAX_PISTA - 1);
        novoNode->pista[MAX_PISTA - 1] = '\0';
        novoNode->esquerda = NULL;
        novoNode->direita = NULL;
        return novoNode; // Retorna o novo nó como a nova raiz
    }

    // Compara alfabeticamente a nova pista com a pista do nó atual
    int comparacao = strcmp(pista, raiz->pista);

    if (comparacao < 0) {
        // A nova pista é 'menor' (vem antes no alfabeto): insere à esquerda
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (comparacao > 0) {
        // A nova pista é 'maior' (vem depois no alfabeto): insere à direita
        raiz->direita = inserirPista(raiz->direita, pista);
    } 
    // Se comparacao == 0, a pista já existe, não insere (ou faria tratamento de duplicatas)

    return raiz; // Retorna o ponteiro da raiz inalterado
}

// -------------------------------------------------------------------

/**
 * @brief Permite a navegação interativa e coleta de pistas.
 * * Controla a navegação do jogador, exibe mensagens e chama inserirPista()
 * quando uma pista é encontrada.
 * * @param atual O nó (Sala) atual onde o jogador se encontra.
 */
void explorarSalasComPistas(Sala *atual) {
    char escolha;
    
    while (atual != NULL) {
        printf("\n=> Voce esta no(a): **%s**\n", atual->nome);

        // Verifica se a sala atual possui uma pista
        if (strlen(atual->pista) > 0) {
            printf("----------------------------------------------\n");
            printf("🚨 **PISTA ENCONTRADA!** Conteudo: \"%s\"\n", atual->pista);
            // Insere automaticamente a pista na BST
            raizPistas = inserirPista(raizPistas, atual->pista);
            // Limpa a pista da Sala para que não seja coletada novamente
            atual->pista[0] = '\0'; 
            printf("Pista adicionada ao seu caderno de notas.\n");
            printf("----------------------------------------------\n");
        } else {
            printf("(Nenhuma pista neste comodo, continue a busca.)\n");
        }
        
        // Exibe as opções de navegação
        printf("\nPara onde voce gostaria de ir?\n");
        if (atual->esquerda != NULL) {
            printf("  [e] Esquerda (para %s)\n", atual->esquerda->nome);
        }
        if (atual->direita != NULL) {
            printf("  [d] Direita (para %s)\n", atual->direita->nome);
        }
        printf("  [s] Sair da mansao e revisar as pistas\n");

        printf("Sua escolha (e/d/s): ");
        if (scanf(" %c", &escolha) != 1) {
             // Tratamento simples de erro de leitura
             break;
        }

        // Lógica de Navegação
        switch (escolha) {
            case 'e':
            case 'E':
                if (atual->esquerda != NULL) {
                    atual = atual->esquerda;
                } else {
                    printf("Nao ha caminho para a esquerda a partir desta sala.\n");
                }
                break;
            case 'd':
            case 'D':
                if (atual->direita != NULL) {
                    atual = atual->direita;
                } else {
                    printf("Nao ha caminho para a direita a partir desta sala.\n");
                }
                break;
            case 's':
            case 'S':
                printf("\n--- Revisando seu caderno de notas ---\n");
                return; // Sai da função para revisar as pistas
            default:
                printf("Escolha invalida. Por favor, digite 'e', 'd' ou 's'.\n");
                break;
        }
    }
}

// -------------------------------------------------------------------

/**
 * @brief Exibe todas as pistas coletadas em ordem alfabética.
 * * Utiliza a travessia **In-Ordem** da BST (Esquerda -> Raiz -> Direita)
 * para garantir a ordenação alfabética das pistas.
 * * @param raiz A raiz da subárvore atual da BST de Pistas.
 */
void exibirPistas(PistaNode *raiz) {
    // 1. Caso base: se a raiz for NULL, não há mais nada para imprimir
    if (raiz != NULL) {
        // 2. Recursivamente para a subárvore esquerda
        exibirPistas(raiz->esquerda); 

        // 3. Processa a Raiz (imprime o conteúdo)
        printf("- %s\n", raiz->pista); 

        // 4. Recursivamente para a subárvore direita
        exibirPistas(raiz->direita);
    }
}

// -------------------------------------------------------------------

/**
 * @brief Libera recursivamente a memória alocada para o mapa da mansão.
 * * (Travessia Pós-Ordem: Esquerda -> Direita -> Raiz)
 */
void liberarMapa(Sala *raiz) {
    if (raiz != NULL) {
        liberarMapa(raiz->esquerda);
        liberarMapa(raiz->direita);
        free(raiz);
    }
}

/**
 * @brief Libera recursivamente a memória alocada para a BST de pistas.
 * * (Travessia Pós-Ordem: Esquerda -> Direita -> Raiz)
 */
void liberarPistas(PistaNode *raiz) {
    if (raiz != NULL) {
        liberarPistas(raiz->esquerda);
        liberarPistas(raiz->direita);
        free(raiz);
    }
}

// -------------------------------------------------------------------
// --- FUNÇÃO PRINCIPAL ---
// -------------------------------------------------------------------

/**
 * @brief Função principal: monta o mapa e inicia a exploração.
 * * Cria a estrutura da Árvore Binária de Salas e inicia o loop de exploração.
 */
int main() {
    printf("### DETECTIVE QUEST: MAPA E CADERNO DE PISTAS ###\n");
    printf("Explore a mansao para coletar indicios. Digite 's' para sair e revisar.\n");
    printf("-------------------------------------------------------------------\n");

    // 

    // 1. Montagem do Mapa da Mansão (Árvore Binária Simples)
    Sala *hall = criarSala("Hall de entrada", "O casaco do mordomo esta sujo de lama."); // Raiz
    Sala *biblioteca = criarSala("Biblioteca", "Um livro sobre venenos esta fora do lugar.");
    Sala *salaDeJantar = criarSala("Sala de Jantar", "Uma xicara de cha quebrada sob a mesa."); 
    Sala *cozinha = criarSala("Cozinha", ""); // Sem pista
    Sala *jardim = criarSala("Jardim de Inverno", "As pegadas levam a uma saida lateral.");
    Sala *quartoPrincipal = criarSala("Quarto Principal", "Um bilhete de resgate vazio.");
    Sala *banheiro = criarSala("Banheiro de Servico", ""); // Sem pista

    // Estrutura da Árvore (Conexões)
    if (hall != NULL) {
        // Nível 1
        hall->esquerda = biblioteca;
        hall->direita = salaDeJantar;

        // Nível 2 (Filhos de Biblioteca)
        if (biblioteca != NULL) {
            biblioteca->esquerda = cozinha; 
            biblioteca->direita = jardim;   
        }

        // Nível 2 (Filhos de Sala de Jantar)
        if (salaDeJantar != NULL) {
            salaDeJantar->esquerda = quartoPrincipal;
            salaDeJantar->direita = banheiro;
        }
        
        // Os demais nós não possuem mais filhos (são folhas ou semi-folhas na nossa estrutura atual)
    } else {
        fprintf(stderr, "Erro ao criar o Hall de entrada. Encerrando.\n");
        return 1;
    }

    // 2. Início da Exploração
    explorarSalasComPistas(hall);

    // 3. Exibição das Pistas Coletadas (Travessia In-Ordem da BST)
    printf("\n## 📚 PISTAS COLETADAS (Ordem Alfabética) ##\n");
    if (raizPistas == NULL) {
        printf("Nenhuma pista foi coletada durante a exploracao.\n");
    } else {
        exibirPistas(raizPistas);
    }
    printf("---------------------------------------------------\n");

    // 4. Liberação da Memória
    liberarMapa(hall);
    liberarPistas(raizPistas);
    
    return 0;
}