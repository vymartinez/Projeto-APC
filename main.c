#include <stdio.h>
#include <ctype.h>
#include <string.h>

int menu, settings, ranking, chooseDificulty, played;
int dificulty = 1;
char modes[3][10] = {"Facil", "Medio", "Dificil"};
char player[21];

typedef struct players {
    char name[21];
    int score;
} players;

void EnterNickname() {
    printf("\nDigite seu nome(ate 20 caracteres): ");
    scanf("%s", player);
    int len = strlen(player);
    for (int i = 0; i < len; i++) {
        if (!isalpha(player[i]) && player[i] != ' ') { 
            printf("\nNome invalido! Digite um nome valido, por favor.\n");
            EnterNickname();
            return;
        }
    }
    menu = 1;
    return;
}

void printMenu() {
    printf("\n");
    printf("        Number Sums - Made by: Victor Yan        \n");
    printf("################################################\n\n");
    printf("[1] - Jogar [Modo %s]\n", modes[dificulty-1]);
    printf("[2] - Configuracoes\n");
    printf("[3] - Ranking\n");
    printf("[4] - Sair\n\n");
    printf("################################################\n\n");
    printf("Escolha uma opcao: ");
    return;
}

void printConfig() {
    printf("\n");
    printf("                  Configuracoes                   \n");
    printf("################################################\n\n");
    printf("[1] - Dificuldade\n");
    printf("[2] - Sugerir novo nivel\n");
    printf("[3] - Voltar\n\n");
    printf("################################################\n\n");
    printf("Escolha uma opcao: ");
    return;
}

void printDificulty() {
    printf("\n");
    printf("                  Dificuldade                     \n");
    printf("################################################\n\n");
    printf("[1] - Facil\n");
    printf("[2] - Medio\n");
    printf("[3] - Dificil\n");
    printf("[4] - Voltar\n\n");
    printf("################################################\n\n");
    printf("Escolha uma opcao: ");
    return;
}

void printRanking() {
    players rankingPlayer;
    FILE *file = fopen("ranking.bin","rb");
    if (file == NULL) {
        printf("\nNenhum jogador cadastrado ainda.\n\n");
        fclose(file);
    } else {
        printf("\n");
        printf("                    Ranking                      \n");
        printf("################################################\n\n");
        fread(&played, sizeof(int), 1, file);
        for (int i = 1; i <= played; i++) {
            fread(&rankingPlayer, sizeof(players), 1, file);
            printf("%d - %s - %d\n", i, rankingPlayer.name, rankingPlayer.score);
        }
        fclose(file);
        printf("################################################\n\n");
    }
    printf("Pressione qualquer tecla para voltar ao menu.\n");
    return;
}

void suggestGame() {
    //TODO: implementar funcionalidade do player poder sugerir uma nova matriz (ULTIMA TAREFA)
}

int Play() {
    //TODO: implementar a gameplay aqui
    return 1;
}

int main() {
    menu = settings = chooseDificulty = ranking = played = 0;
    char menuOption;
    EnterNickname();
    while (menu) {
        printMenu();
        scanf("\n%c", &menuOption);
        switch(menuOption) {
            case '1':
                Play();
                break;
            case '2':
                settings = 1;
                while (settings) {
                    printConfig();
                    scanf("\n%c", &menuOption);
                    switch(menuOption) {
                        case '1':
                            settings = 0;
                            chooseDificulty = 1;
                            while (chooseDificulty) {
                                printDificulty();
                                scanf("\n%c", &menuOption);
                                switch(menuOption) {
                                    case '1':
                                        dificulty = 1;
                                        chooseDificulty = 0;
                                        break;
                                    case '2':
                                        dificulty = 2;
                                        chooseDificulty = 0;
                                        break;
                                    case '3':
                                        dificulty = 3;
                                        chooseDificulty = 0;
                                        break;
                                    case '4':
                                        chooseDificulty = 0;
                                        settings = 1;
                                        break;
                                    default:
                                        printf("\nOpcao invalida! Selecione uma dificuldade, por favor.\n");
                                        break;
                                }
                            }
                            break;
                        case '2':
                            settings = 0;
                            suggestGame();
                            break;
                        case '3':
                            settings = 0;
                            break;
                        default:
                            printf("\nOpcao invalida! Selecione uma opcao de configuracao, por favor.\n");
                            break;
                    }
                }
                break;
            case '3':
                ranking = 1;
                while (ranking) {
                    printRanking();
                    getchar();
                    getchar();
                    ranking = 0;
                }
                break;
            case '4':
                return 0;
                break;
            default:
                printf("\nOpcao invalida! Selecione uma opcao do menu, por favor.\n");
                break;
        }
    }
}