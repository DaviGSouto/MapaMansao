#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// --- Constantes ---
#define MAX_NOME 50
#define MAX_PISTA 100
#define HASH_SIZE 10 // Tamanho da Tabela Hash (simplificado)

// --- Estruturas de Dados ---

// 1. Estrutura para o Mapa da Mansão (Árvore Binária Simples)
typedef struct Sala {
    char nome[MAX_NOME];
    // A pista será definida via lógica na main, não como campo da struct Sala
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// 2. Estrutura para a Árvore de Pistas Coletadas (BST)
typedef struct PistaNode {
    char pista[MAX_PISTA];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// 3. Estrutura para a Tabela Hash (Associação Pista -> Suspeito)
// Implementação de hash com encadeamento separado (Separate Chaining)
typedef struct HashNode {
    char pista[MAX_PISTA];  // Chave: a pista
    char suspeito[MAX_NOME]; // Valor: o suspeito
    struct HashNode *proximo; // Próximo nó na mesma bucket (encadeamento)
} HashNode;

// Variáveis Globais
PistaNode *raizPistas = NULL; // Raiz da BST de Pistas
HashNode *tabelaHash[HASH_SIZE]; // Tabela Hash de Suspeitos

// --- Protótipos das Funções ---

// Funções de Sala (Mapa)
Sala* criarSala(const char *nome);
void explorarSalas(Sala *atual);
char* obterPistaPorSala(const char *nomeSala);
void liberarMapa(Sala *raiz);

// Funções de Pista (BST)
PistaNode* inserirPista(PistaNode *raiz, const char *pista);
void exibirPistas(PistaNode *raiz);
void liberarPistas(PistaNode *raiz);

// Funções de Tabela Hash
unsigned int hash(const char *chave);
void inserirNaHash(const char *pista, const char *suspeito);
char* encontrarSuspeito(const char *pista);
void inicializarHash();
void liberarHash();

// Funções de Julgamento
void verificarSuspeitoFinal();

// --- Implementação das Funções ---

// ===================================
// Funções do Mapa (Árvore Binária)
// ===================================

/**
 * @brief Cria dinamicamente um cômodo (Sala) com o nome especificado.
 * * @param nome O nome da sala a ser criada.
 * @return Um ponteiro para a Sala recém-criada.
 */
Sala* criarSala(const char *nome) {
    Sala *novaSala = (Sala *)malloc(sizeof(Sala));
    if (novaSala != NULL) {
        strncpy(novaSala->nome, nome, MAX_NOME - 1);
        novaSala->nome[MAX_NOME - 1] = '\0';
        novaSala->esquerda = NULL;
        novaSala->direita = NULL;
    }
    return novaSala;
}

/**
 * @brief Obtém a pista estática associada a uma sala (Simulação de regras do jogo).
 * @param nomeSala O nome da sala atual.
 * @return A string da pista associada ou uma string vazia ("").
 */
char* obterPistaPorSala(const char *nomeSala) {
    // Note: Esta função simula a associação fixa de pistas por regra.
    if (strcmp(nomeSala, "Hall de entrada") == 0) return "Casaco com lama no cabideiro.";
    if (strcmp(nomeSala, "Biblioteca") == 0) return "O livro 'Como Sair Ileso' sumiu da estante.";
    if (strcmp(nomeSala, "Cozinha") == 0) return ""; // Sem pista
    if (strcmp(nomeSala, "Jardim de Inverno") == 0) return "Corda cortada perto da janela.";
    if (strcmp(nomeSala, "Quarto Principal") == 0) return "Um mapa de fuga foi queimado na lareira.";
    if (strcmp(nomeSala, "Porão Secreto") == 0) return "Cabelos ruivos na escada.";
    
    return ""; // Pista padrão
}

/**
 * @brief Navega pela árvore e ativa o sistema de pistas.
 * * Controla a navegação do jogador ('e', 'd', 's'), coleta pistas e as insere na BST.
 * * @param atual O nó (Sala) atual onde o jogador se encontra.
 */
void explorarSalas(Sala *atual) {
    char escolha;
    
    while (atual != NULL) {
        printf("\n============================================\n");
        printf("=> Voce esta no(a): **%s**\n", atual->nome);

        // 1. Coleta e Inserção da Pista
        char *pista_atual = obterPistaPorSala(atual->nome);
        
        if (strlen(pista_atual) > 0) {
            printf("🚨 **PISTA ENCONTRADA!** Conteudo: \"%s\"\n", pista_atual);
            // Insere a pista na BST
            raizPistas = inserirPista(raizPistas, pista_atual);
            printf("Pista adicionada ao seu caderno de notas.\n");
        } else {
            printf("(Nenhuma pista nova neste comodo.)\n");
        }

        // 2. Opções de Navegação
        printf("\nPara onde voce gostaria de ir?\n");
        if (atual->esquerda != NULL) {
            printf("  [e] Esquerda (para %s)\n", atual->esquerda->nome);
        }
        if (atual->direita != NULL) {
            printf("  [d] Direita (para %s)\n", atual->direita->nome);
        }
        printf("  [s] Sair da mansao e acusar o culpado\n");

        printf("Sua escolha (e/d/s): ");
        if (scanf(" %c", &escolha) != 1) {
             break;
        }

        // 3. Lógica de Navegação
        switch (tolower(escolha)) {
            case 'e':
                if (atual->esquerda != NULL) {
                    atual = atual->esquerda;
                } else {
                    printf("Caminho inexistente para a esquerda.\n");
                }
                break;
            case 'd':
                if (atual->direita != NULL) {
                    atual = atual->direita;
                } else {
                    printf("Caminho inexistente para a direita.\n");
                }
                break;
            case 's':
                printf("\nEncerrando a exploracao...\n");
                return; 
            default:
                printf("Escolha invalida.\n");
                break;
        }
    }
}

// ===================================
// Funções da Pista (BST)
// ===================================

/**
 * @brief Insere a pista coletada na Árvore Binária de Busca (BST).
 * @param raiz A raiz da subárvore.
 * @param pista O conteúdo da pista a ser inserida.
 * @return O ponteiro para a nova raiz da subárvore.
 */
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) {
        PistaNode *novoNode = (PistaNode *)malloc(sizeof(PistaNode));
        if (novoNode == NULL) return NULL;
        strncpy(novoNode->pista, pista, MAX_PISTA - 1);
        novoNode->pista[MAX_PISTA - 1] = '\0';
        novoNode->esquerda = novoNode->direita = NULL;
        return novoNode;
    }

    int comparacao = strcmp(pista, raiz->pista);
    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    } 
    // Ignora duplicatas

    return raiz;
}

/**
 * @brief Imprime a árvore de pistas em ordem alfabética (In-Ordem).
 * @param raiz A raiz da BST de Pistas.
 */
void exibirPistas(PistaNode *raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda); 
        printf("- %s\n", raiz->pista); 
        exibirPistas(raiz->direita);
    }
}


// ===================================
// Funções da Tabela Hash
// ===================================

/**
 * @brief Função hash simples para determinar o índice do bucket.
 * * Soma os valores ASCII dos caracteres da chave e aplica módulo HASH_SIZE.
 * @param chave A string (pista) a ser hasheada.
 * @return O índice do bucket na tabela.
 */
unsigned int hash(const char *chave) {
    unsigned int valor = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        valor = valor * 31 + chave[i];
    }
    return valor % HASH_SIZE;
}

/**
 * @brief Inicializa a Tabela Hash, definindo todos os ponteiros como NULL.
 */
void inicializarHash() {
    for (int i = 0; i < HASH_SIZE; i++) {
        tabelaHash[i] = NULL;
    }
}

/**
 * @brief Insere associação pista/suspeito na tabela hash.
 * * Lida com colisões usando encadeamento (Separate Chaining).
 * @param pista A chave (pista).
 * @param suspeito O valor (suspeito).
 */
void inserirNaHash(const char *pista, const char *suspeito) {
    unsigned int indice = hash(pista);
    
    // Cria um novo nó
    HashNode *novoNode = (HashNode *)malloc(sizeof(HashNode));
    if (novoNode == NULL) {
        fprintf(stderr, "Erro: Falha na alocacao de memoria para HashNode.\n");
        return;
    }

    strncpy(novoNode->pista, pista, MAX_PISTA - 1);
    novoNode->pista[MAX_PISTA - 1] = '\0';
    strncpy(novoNode->suspeito, suspeito, MAX_NOME - 1);
    novoNode->suspeito[MAX_NOME - 1] = '\0';
    
    // Adiciona o novo nó no início do encadeamento (Lista Ligada)
    novoNode->proximo = tabelaHash[indice];
    tabelaHash[indice] = novoNode;
}

/**
 * @brief Consulta o suspeito correspondente a uma pista.
 * * Percorre o encadeamento se houver colisão.
 * @param pista A chave (pista) a ser consultada.
 * @return O nome do suspeito ou NULL se a pista não for encontrada.
 */
char* encontrarSuspeito(const char *pista) {
    unsigned int indice = hash(pista);
    HashNode *atual = tabelaHash[indice];

    // Percorre a lista encadeada no índice
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito; // Encontrou!
        }
        atual = atual->proximo;
    }
    return NULL; // Não encontrado
}

// ===================================
// Funções de Julgamento
// ===================================

// Função auxiliar para percorrer a BST de Pistas e contar evidências
void contarEvidenciasRecursivo(PistaNode *raiz, const char *suspeitoAlvo, int *contador) {
    if (raiz != NULL) {
        // 1. Processa o nó atual
        char *suspeitoApontado = encontrarSuspeito(raiz->pista);
        if (suspeitoApontado != NULL && strcmp(suspeitoApontado, suspeitoAlvo) == 0) {
            (*contador)++;
            printf("   [+] Evidencia: \"%s\" (aponta para %s)\n", raiz->pista, suspeitoAlvo);
        }

        // 2. Percorre as subárvores
        contarEvidenciasRecursivo(raiz->esquerda, suspeitoAlvo, contador);
        contarEvidenciasRecursivo(raiz->direita, suspeitoAlvo, contador);
    }
}

/**
 * @brief Conduz à fase de julgamento final.
 * * Lista pistas, solicita a acusação e verifica se há evidências suficientes.
 */
void verificarSuspeitoFinal() {
    char acusado[MAX_NOME];
    
    if (raizPistas == NULL) {
        printf("\nVoce nao coletou nenhuma pista. A investigacao nao pode prosseguir.\n");
        return;
    }

    // 1. Listar Pistas
    printf("\n## 📋 PISTAS COLETADAS ##\n");
    exibirPistas(raizPistas);
    printf("---------------------------\n");

    // 2. Solicitar Acusação
    printf("\nQuem voce acusa? (Digite o nome: Sra. White, Prof. Plum, Rev. Green): ");
    if (scanf(" %[^\n]", acusado) != 1) {
        return;
    }

    // 3. Contagem de Evidências
    printf("\n## ⚖️ JULGAMENTO FINAL: Analisando Evidencias contra %s ##\n", acusado);
    int evidencias = 0;
    
    // Inicia a contagem recursiva na BST de pistas
    contarEvidenciasRecursivo(raizPistas, acusado, &evidencias);
    
    printf("\n=> Total de evidencias contra %s: **%d**\n", acusado, evidencias);

    // 4. Verificação de Desfecho
    if (evidencias >= 2) {
        printf("🎉 **VITORIA!** O caso esta fechado! Ha %d evidencias para sustentar a acusacao de %s. O verdadeiro culpado foi encontrado!\n", evidencias, acusado);
    } else {
        printf("⚠️ **DERROTA!** O caso nao pode ser fechado. Apenas %d evidencias nao sao suficientes para acusar %s. A investigacao continua...\n", evidencias, acusado);
    }
    printf("============================================\n");
}


// ===================================
// Funções de Liberação de Memória
// ===================================

void liberarMapa(Sala *raiz) {
    if (raiz != NULL) {
        liberarMapa(raiz->esquerda);
        liberarMapa(raiz->direita);
        free(raiz);
    }
}

void liberarPistas(PistaNode *raiz) {
    if (raiz != NULL) {
        liberarPistas(raiz->esquerda);
        liberarPistas(raiz->direita);
        free(raiz);
    }
}

void liberarHash() {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode *atual = tabelaHash[i];
        while (atual != NULL) {
            HashNode *temp = atual;
            atual = atual->proximo;
            free(temp);
        }
        tabelaHash[i] = NULL;
    }
}

// ===================================
// FUNÇÃO PRINCIPAL
// ===================================

int main() {
    printf("### DETECTIVE QUEST: NIVEL MESTRE (Mapa, BST e Hash) ###\n");

    // 

    // 1. Inicialização da Tabela Hash
    inicializarHash();

    // 2. Inserção das Associações Pista -> Suspeito (Fixo)
    // As pistas inseridas aqui DEVEM corresponder às pistas obtidas via obterPistaPorSala().
    inserirNaHash("Casaco com lama no cabideiro.", "Rev. Green"); // Evidencia 1 (R. Green)
    inserirNaHash("O livro 'Como Sair Ileso' sumiu da estante.", "Prof. Plum");
    inserirNaHash("Corda cortada perto da janela.", "Sra. White"); // Evidencia 1 (S. White)
    inserirNaHash("Um mapa de fuga foi queimado na lareira.", "Rev. Green"); // Evidencia 2 (R. Green)
    inserirNaHash("Cabelos ruivos na escada.", "Sra. White"); // Evidencia 2 (S. White)
    inserirNaHash("Uma fatia de bolo pela metade no cesto.", "Prof. Plum");
    
    printf("A base de dados de suspeitos foi carregada com sucesso.\n");
    
    // 3. Montagem do Mapa da Mansão (Árvore Binária Simples)
    Sala *hall = criarSala("Hall de entrada"); // Pista: Rev. Green
    Sala *biblioteca = criarSala("Biblioteca"); // Pista: Prof. Plum
    Sala *cozinha = criarSala("Cozinha"); // Sem Pista
    Sala *jardim = criarSala("Jardim de Inverno"); // Pista: Sra. White
    Sala *quartoPrincipal = criarSala("Quarto Principal"); // Pista: Rev. Green
    Sala *salaDeEstar = criarSala("Sala de Estar"); // Pista: Prof. Plum
    Sala *porao = criarSala("Porão Secreto"); // Pista: Sra. White

    // Estrutura da Árvore (Conexões)
    if (hall != NULL) {
        hall->esquerda = biblioteca;
        hall->direita = salaDeEstar;

        if (biblioteca != NULL) {
            biblioteca->esquerda = cozinha; 
            biblioteca->direita = jardim;   
        }

        if (salaDeEstar != NULL) {
            salaDeEstar->esquerda = quartoPrincipal;
            salaDeEstar->direita = porao;
        }
    } else {
        fprintf(stderr, "Erro ao criar o mapa. Encerrando.\n");
        return 1;
    }

    // 4. Início da Exploração
    explorarSalas(hall);

    // 5. Fase de Julgamento Final
    verificarSuspeitoFinal();

    // 6. Liberação da Memória
    liberarMapa(hall);
    liberarPistas(raizPistas);
    liberarHash();
    
    return 0;
}