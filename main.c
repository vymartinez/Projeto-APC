#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct players {
    char name[21];
    int score;
} players;

int menu, settings, ranking, chooseDificulty, played, dificulty;
dificulty = 1;
char modes[3][10] = {"Facil", "Medio", "Dificil"};
players player;

int compare(players *a, players *b) {
    if (a->score < b->score) {
        return 1;
    }
    return -1;
}

void EnterNickname() {
    printf("\nDigite seu nome(ate 20 caracteres): ");
    scanf("%s", player.name);
    int len = strlen(player.name);
    for (int i = 0; i < len; i++) {
        if (!isalpha(player.name[i]) && player.name[i] != ' ') { 
            printf("\nNome invalido! Digite um nome valido, por favor.\n");
            EnterNickname();
            return;
        }
    }
    FILE *file = fopen("rank.bin","rb");
    if (file == NULL) {
        fclose(file);
        menu = played = 1;
        player.score = 0;
        FILE *file2 = fopen("rank.bin","wb");
        fwrite(&played, sizeof(int), 1, file2);
        fwrite(&player, sizeof(players), 1, file2);
        fclose(file2);
        return;
    } else {
        int continuation = 0;
        char answer;
        fread(&played, sizeof(int), 1, file);
        players allPlayers[played];
        for (int i = 0; i < played; i++) {
            fread(&allPlayers[i], sizeof(players), 1, file);
            if (strcmp(allPlayers[i].name, player.name) == 0) {
                fclose(file);
                continuation = 1;
                while (continuation) {
                    printf("\no usuario '%s' ja existe. Deseja continuar? (S/N): ", player.name);
                    scanf("\n%c", &answer);
                    if (answer == 'S') {
                        continuation = 0;
                        menu = 1;
                        player.score = allPlayers[i].score;
                        return;
                    } else if (answer == 'N') {
                        EnterNickname();
                        return;
                    }
                }
            }
        }
        menu = 1;
        player.score = 0;
        played++;
        fclose(file);
        FILE *file2 = fopen("rank.bin","wb");
        fwrite(&played, sizeof(int), 1, file2);
        for (int i = 0; i < played-1; i++) {
            fwrite(&allPlayers[i], sizeof(players), 1, file2);
        }
        fwrite(&player, sizeof(players), 1, file2);
        fclose(file2);
        return;
    }
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
    FILE *file = fopen("rank.bin","rb");
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
            printf("%d. %s - PONTOS: %d\n", i, rankingPlayer.name, rankingPlayer.score);
        }
        fclose(file);
        printf("\n################################################\n\n");
    }
    printf("Pressione qualquer tecla para voltar ao menu: ");
    return;
}

void suggestGame() {
    //TODO: implementar funcionalidade do player poder sugerir uma nova matriz (ULTIMA TAREFA)
}

int Play() {
    //TODO: implementar a gameplay aqui
    switch(dificulty) {
        case 1:
            FILE *easy = fopen("easy.txt", "r");
            break;
        case 2:
            FILE *medium = fopen("medium.txt", "r");
            break;
        case 3:
            FILE *medium = fopen("hard.txt", "r");
            break;
    }
    player.score += 100 * dificulty;
    FILE *file = fopen("rank.bin", "rb");
    fread(&played, sizeof(int), 1, file);
    players ranking[played];
    for (int i = 0; i < played; i++) {
        fread(&ranking[i], sizeof(players), 1, file);
        if (strcmp(ranking[i].name, player.name) == 0) {
            ranking[i].score = player.score;
        }
    }
    fclose(file);
    qsort(ranking, played, sizeof(players), compare);
    FILE *file2 = fopen("rank.bin", "wb");
    fwrite(&played, sizeof(int), 1, file2);
    for (int i = 0; i < played; i++) {
        fwrite(&ranking[i], sizeof(players), 1, file2);
    }
    fclose(file2);
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