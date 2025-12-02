#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição da estrutura para um nó da árvore, que representa uma Sala da mansão.
// Um nó da árvore binária tem um valor (o nome da sala) e ponteiros para os filhos esquerdo e direito.
typedef struct Sala {
    char nome[50];       // Nome da sala (ex: "Hall de entrada")
    struct Sala *esquerda; // Ponteiro para a próxima sala à esquerda
    struct Sala *direita;  // Ponteiro para a próxima sala à direita
} Sala;

// --- Protótipos das Funções ---

Sala* criarSala(const char *nome);
void explorarSalas(Sala *atual);

// --- Implementação das Funções ---

/**
 * @brief Cria, de forma dinâmica, uma nova sala com o nome especificado.
 * * Utiliza malloc para alocar memória para a nova sala e inicializa seus campos.
 * Os ponteiros 'esquerda' e 'direita' são definidos inicialmente como NULL.
 * * @param nome O nome da sala a ser criada.
 * @return Um ponteiro para a Sala recém-criada, ou NULL em caso de falha.
 */
Sala* criarSala(const char *nome) {
    Sala *novaSala = (Sala *)malloc(sizeof(Sala));
    if (novaSala != NULL) {
        // Copia o nome para a estrutura e inicializa os ponteiros dos filhos
        strncpy(novaSala->nome, nome, sizeof(novaSala->nome) - 1);
        novaSala->nome[sizeof(novaSala->nome) - 1] = '\0'; // Garantir terminação
        novaSala->esquerda = NULL;
        novaSala->direita = NULL;
    }
    return novaSala;
}

/**
 * @brief Permite a navegação do jogador pela árvore (mapa da mansão).
 * * Exibe a sala atual e solicita ao jogador que escolha entre o caminho
 * da esquerda ('e'), da direita ('d'), ou sair ('s'). A exploração continua
 * até o jogador escolher sair ou alcançar um nó-folha (sala sem caminhos).
 * * @param atual O nó (Sala) atual onde o jogador se encontra.
 */
void explorarSalas(Sala *atual) {
    char escolha;

    // A exploração continua enquanto houver uma sala para explorar
    while (atual != NULL) {
        printf("\n=> Voce esta no(a): **%s**\n", atual->nome);

        // Verifica se é um nó-folha (sala sem caminhos adicionais)
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("\n--- FIM DA LINHA ---\n");
            printf("Voce encontrou um comodo sem mais caminhos. A exploracao terminou.\n");
            break; // Sai do loop
        }

        printf("Para onde voce gostaria de ir?\n");
        
        // Exibe as opções de navegação com base nos filhos existentes
        if (atual->esquerda != NULL) {
            printf("  [e] Esquerda (para %s)\n", atual->esquerda->nome);
        }
        if (atual->direita != NULL) {
            printf("  [d] Direita (para %s)\n", atual->direita->nome);
        }
        printf("  [s] Sair da mansao\n");

        printf("Sua escolha (e/d/s): ");
        if (scanf(" %c", &escolha) != 1) {
             // Tratamento simples de erro de leitura
             break;
        }

        // --- Lógica de Navegação ---
        switch (escolha) {
            case 'e':
            case 'E':
                if (atual->esquerda != NULL) {
                    atual = atual->esquerda; // Move para a sala da esquerda
                } else {
                    printf("Nao ha caminho para a esquerda a partir desta sala.\n");
                }
                break;
            case 'd':
            case 'D':
                if (atual->direita != NULL) {
                    atual = atual->direita; // Move para a sala da direita
                } else {
                    printf("Nao ha caminho para a direita a partir desta sala.\n");
                }
                break;
            case 's':
            case 'S':
                printf("\nSaindo da mansao... Ate a proxima!\n");
                return; // Sai da função
            default:
                printf("Escolha invalida. Por favor, digite 'e', 'd' ou 's'.\n");
                break;
        }
    }
}

/**
 * @brief Função principal: monta o mapa inicial e dá início à exploração.
 * * Cria manualmente a estrutura da árvore binária da mansão usando a função
 * criarSala() e inicia a exploração a partir do nó raiz.
 * * @return 0 se o programa for executado com sucesso.
 */
int main() {
    // 

    printf("### BEM-VINDO(A) AO DETECTIVE QUEST: MAPA DA MANSAO ###\n");
    printf("A mansao foi mapeada com sucesso. Inicie sua exploracao a partir do Hall de entrada.\n");
    printf("-------------------------------------------------------------------\n");

    // 1. Criação das Salas (Nós)
    Sala *hall = criarSala("Hall de entrada"); // Raiz
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *salaDeEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha"); // Nó-folha
    Sala *jardim = criarSala("Jardim de Inverno"); // Nó-folha
    Sala *quartoPrincipal = criarSala("Quarto Principal");
    Sala *banheiro = criarSala("Banheiro"); // Nó-folha

    // 2. Montagem da Estrutura (Ligações da Árvore Binária)
    if (hall != NULL) {
        // Nível 1
        hall->esquerda = biblioteca;
        hall->direita = salaDeEstar;

        // Nível 2 (Filhos de Biblioteca)
        if (biblioteca != NULL) {
            biblioteca->esquerda = cozinha; // Nó-folha
            biblioteca->direita = jardim;   // Nó-folha
        }

        // Nível 2 (Filhos de Sala de Estar)
        if (salaDeEstar != NULL) {
            salaDeEstar->esquerda = quartoPrincipal;
            salaDeEstar->direita = NULL; // Não há caminho para a direita
        }
        
        // Nível 3 (Filhos de Quarto Principal)
        if (quartoPrincipal != NULL) {
            quartoPrincipal->esquerda = banheiro; // Nó-folha
            quartoPrincipal->direita = NULL; // Não há caminho para a direita
        }
    } else {
        printf("Erro ao criar o Hall de entrada. Encerrando.\n");
        return 1;
    }

    // 3. Início da Exploração
    explorarSalas(hall);
    
    return 0;
}