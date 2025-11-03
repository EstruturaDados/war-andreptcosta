// ============================================================================
//                PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <ctype.h>

// Constantes Globais
#define MAX_TERRITORIES 5
#define NAME_LEN 40
#define COLOR_LEN 20

// IDs de missões
typedef enum {
    MISSAO_DESTRUIR_VERDE = 0,
    MISSAO_CONQUISTAR_3   = 1
} MissaoID;

typedef struct {
    char nome[NAME_LEN];
    char cor[COLOR_LEN];
    int tropas;           
} Territorio;

void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void removerQuebraLinha(char *s) {
    if (!s) return;
    size_t len = strlen(s);
    if (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
    }
}

Territorio *alocarMapa(int qtd) {
    Territorio *ptr = (Territorio *)calloc(qtd, sizeof(Territorio));
    if (!ptr) {
        fprintf(stderr, "Erro: Falha ao alocar memória para %d territórios.\n", qtd);
    }
    return ptr;
}

void liberarMapa(Territorio **mapa) {
    if (mapa && *mapa) {
        free(*mapa);
        *mapa = NULL;
    }
}

void cadastrarTerritoriosManual(Territorio *mapa, int qtd) {
    printf("\n=== Cadastro Manual de Territórios ===\n");
    for (int i = 0; i < qtd; ++i) {
        printf("Território %d/%d\n", i + 1, qtd);
        printf("  Nome: ");
        fgets(mapa[i].nome, NAME_LEN, stdin); removerQuebraLinha(mapa[i].nome);
        if (strlen(mapa[i].nome) == 0) { snprintf(mapa[i].nome, NAME_LEN, "Territorio_%d", i + 1); }

        printf("  Cor do Exército: ");
        fgets(mapa[i].cor, COLOR_LEN, stdin); removerQuebraLinha(mapa[i].cor);
        if (strlen(mapa[i].cor) == 0) { snprintf(mapa[i].cor, COLOR_LEN, "Neutro"); }

        printf("  Número de Tropas: ");
        while (scanf("%d", &mapa[i].tropas) != 1 || mapa[i].tropas < 0) {
            limparBufferEntrada();
            printf("    Valor inválido. Digite novamente: ");
        }
        limparBufferEntrada();
        if (mapa[i].tropas == 0) mapa[i].tropas = 1; // garantir pelo menos 1
        printf("-----------------------------------------\n");
    }
}

void inicializarTerritoriosAutomatico(Territorio *mapa, int qtd) {
    const char *nomesPadrao[MAX_TERRITORIES] = {"Atenas", "Rodes", "Olimpo", "Esparta", "Corinto"};
    const char *coresPadrao[MAX_TERRITORIES] = {"Verde", "Vermelho", "Azul", "Amarelo", "Verde"};
    const int tropasPadrao[MAX_TERRITORIES] = {3, 4, 5, 2, 3};
    for (int i = 0; i < qtd; ++i) {
        snprintf(mapa[i].nome, NAME_LEN, "%s", (i < MAX_TERRITORIES ? nomesPadrao[i] : "Terr")); 
        snprintf(mapa[i].cor, COLOR_LEN, "%s", (i < MAX_TERRITORIES ? coresPadrao[i] : "Neutro"));
        mapa[i].tropas = (i < MAX_TERRITORIES ? tropasPadrao[i] : 1);
    }
}

void exibirMapa(const Territorio *mapa, int qtd) {
    printf("\n================ MAPA ATUAL ================\n");
    printf("%-3s | %-15s | %-10s | %-6s\n", "ID", "Nome", "Cor", "Tropas");
    printf("---------------------------------------------\n");
    for (int i = 0; i < qtd; ++i) {
        printf("%3d | %-15s | %-10s | %6d\n", i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("============================================\n");
}

void exibirMenuPrincipal(void) {
    printf("\n===== MENU PRINCIPAL =====\n");
    printf("1 - Atacar\n");
    printf("2 - Verificar Missão\n");
    printf("0 - Sair\n");
    printf("Escolha: ");
}

void exibirMissao(int missaoId) {
    printf("\n>>> Missão Atual: ");
    switch (missaoId) {
        case MISSAO_DESTRUIR_VERDE:
            printf("Destruir o exército Verde (eliminar todos os territórios com tropas da cor Verde).\n");
            break;
        case MISSAO_CONQUISTAR_3:
            printf("Conquistar 3 territórios com a sua cor.\n");
            break;
        default:
            printf("(Desconhecida)\n");
    }
}

void simularAtaque(Territorio *mapa, int atacanteIdx, int defensorIdx, const char *corJogador) {
    Territorio *atk = &mapa[atacanteIdx];
    Territorio *def = &mapa[defensorIdx];

    if (strcmp(atk->cor, corJogador) != 0) {
        printf("Ataque inválido: você não controla '%s'.\n", atk->nome);
        return;
    }
    if (strcmp(def->cor, corJogador) == 0) {
        printf("Defensor inválido: '%s' já é seu.\n", def->nome);
        return;
    }
    if (atk->tropas < 2) {
        printf("Você precisa de pelo menos 2 tropas para atacar (1 fica).\n");
        return;
    }

    int dadoAtk = (rand() % 6) + 1;
    int dadoDef = (rand() % 6) + 1;
    printf("Dados -> Atacante: %d | Defensor: %d\n", dadoAtk, dadoDef);

    if (dadoAtk >= dadoDef) { // empate favorece atacante
        def->tropas -= 1;
        printf("Defensor '%s' perde 1 tropa. Tropas restantes: %d\n", def->nome, def->tropas);
        if (def->tropas <= 0) {
            printf(">>> Território '%s' CONQUISTADO!\n", def->nome);
            snprintf(def->cor, COLOR_LEN, "%s", corJogador);
            def->tropas = 1;           // ocupa com 1
            atk->tropas -= 1;          // move 1 do atacante
            if (atk->tropas < 1) atk->tropas = 1; // garantir mínimo
        }
    } else { // defensor vence
        atk->tropas -= 1;
        printf("Atacante '%s' perde 1 tropa. Tropas restantes: %d\n", atk->nome, atk->tropas);
    }
}

void faseAtaque(Territorio *mapa, int qtd, const char *corJogador) {
    printf("\n=== FASE DE ATAQUE ===\n");
    exibirMapa(mapa, qtd);
    int a, d;
    printf("Escolha o ID do território ATACANTE (0 para cancelar): ");
    if (scanf("%d", &a) != 1) { limparBufferEntrada(); return; }
    if (a == 0) { limparBufferEntrada(); return; }
    printf("Escolha o ID do território DEFENSOR: ");
    if (scanf("%d", &d) != 1) { limparBufferEntrada(); return; }
    limparBufferEntrada();
    if (a < 1 || a > qtd || d < 1 || d > qtd || a == d) {
        printf("IDs inválidos.\n");
        return;
    }
    simularAtaque(mapa, a - 1, d - 1, corJogador);
}

int sortearMissao(void) {
    return rand() % 2; // 0 ou 1
}

int verificarVitoria(int missaoId, const Territorio *mapa, int qtd, const char *corJogador) {
    switch (missaoId) {
        case MISSAO_DESTRUIR_VERDE: {
            for (int i = 0; i < qtd; ++i) {
                if (strcmp(mapa[i].cor, "Verde") == 0 && mapa[i].tropas > 0) {
                    return 0; // ainda existe verde com tropas
                }
            }
            return 1; // nenhum verde ativo
        }
        case MISSAO_CONQUISTAR_3: {
            int conta = 0;
            for (int i = 0; i < qtd; ++i) {
                if (strcmp(mapa[i].cor, corJogador) == 0) conta++;
            }
            return conta >= 3;
        }
        default:
            return 0;
    }
}

int main(void) {
    setlocale(LC_ALL, "");
    srand((unsigned int)time(NULL));

    printf("================= WAR ESTRUTURADO =================\n");

    // Escolha / definição da cor do jogador
    char corJogador[COLOR_LEN];
    printf("Digite a sua cor de exército (ex: Azul): ");
    fgets(corJogador, COLOR_LEN, stdin); removerQuebraLinha(corJogador);
    if (strlen(corJogador) == 0) snprintf(corJogador, COLOR_LEN, "Azul");

    // Alocação dinâmica
    Territorio *mapa = alocarMapa(MAX_TERRITORIES);
    if (!mapa) return 1;

    // Pergunta para cadastro manual ou automático
    char opc;
    printf("Deseja cadastro MANUAL dos territórios? (s/n): ");
    if (scanf(" %c", &opc) != 1) { opc = 'n'; }
    limparBufferEntrada();
    if (opc == 's' || opc == 'S') {
        cadastrarTerritoriosManual(mapa, MAX_TERRITORIES);
    } else {
        inicializarTerritoriosAutomatico(mapa, MAX_TERRITORIES);
        printf("\nTerritórios inicializados automaticamente.\n");
    }

    // Sorteia missão
    int missao = sortearMissao();

    if (missao == MISSAO_DESTRUIR_VERDE && strcasecmp(corJogador, "Verde") == 0) {
        missao = MISSAO_CONQUISTAR_3;
    }

    int sair = 0;
    while (!sair) {
        exibirMapa(mapa, MAX_TERRITORIES);
        exibirMissao(missao);
        exibirMenuPrincipal();
        int escolha;
        if (scanf("%d", &escolha) != 1) {
            limparBufferEntrada();
            printf("Entrada inválida.\n");
            continue;
        }
        limparBufferEntrada();
        switch (escolha) {
            case 1:
                faseAtaque(mapa, MAX_TERRITORIES, corJogador);
                if (verificarVitoria(missao, mapa, MAX_TERRITORIES, corJogador)) {
                    printf("\n================= VITÓRIA =================\n");
                    exibirMissao(missao);
                    printf("Parabéns! Missão cumprida.\n");
                    sair = 1;
                }
                break;
            case 2:
                if (verificarVitoria(missao, mapa, MAX_TERRITORIES, corJogador)) {
                    printf("\n>>> Missão concluída! Você venceu o jogo.\n");
                    sair = 1;
                } else {
                    printf("Missão ainda não concluída. Continue!\n");
                }
                break;
            case 0:
                sair = 1;
                break;
            default:
                printf("Opção inválida.\n");
        }
    }

    liberarMapa(&mapa);
    printf("\nFim de jogo!\n");
    return 0;
}