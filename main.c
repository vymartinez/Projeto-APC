#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct players {
    char name[21];
    int score;
} players;

int menu, playing, settings, ranking, chooseDificulty, played, random, level, row, col, correct, lastTurn;
int easyCols[4], easyRows[4], easyMatrix[4][4], easyColsDelete[7], easyRowsDelete[7],
mediumCols[6], mediumRows[6], mediumMatrix[6][6], mediumColsDelete[21], mediumRowsDelete[21],
hardCols[7], hardRows[7], hardMatrix[7][7], hardColsDelete[32], hardRowsDelete[32], lives;
int easyOffsets[3] = {0, 116, 216}, mediumOffsets[3] = {0, 158, 316}, hardOffsets[3] = {0, 198, 396};
int dificulty = 1;
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
    FILE *file = fopen("ranking.bin","rb");
    if (file == NULL) {
        fclose(file);
        menu = played = 1;
        player.score = 0;
        FILE *file2 = fopen("ranking.bin","wb");
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
        FILE *file2 = fopen("ranking.bin","wb");
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
    FILE *file = fopen("ranking.bin","rb");
    if (file == NULL) {
        printf("\nNenhum jogador cadastrado ainda.\n\n");
        fclose(file);
    } else {
        printf("\n");
        printf("                    Ranking                      \n");
        printf("################################################\n\n");
        fread(&played, sizeof(int), 1, file);
        for (int i = 1; i <= 10 && i <= played; i++) {
            fread(&rankingPlayer, sizeof(players), 1, file);
            printf("%d. %s - PONTOS: %d\n", i, rankingPlayer.name, rankingPlayer.score);
        }
        fclose(file);
        printf("\n################################################\n\n");
    }
    printf("Pressione qualquer tecla para voltar ao menu: ");
    return;
}

void playEasyGame() {
    while (playing) {
        lastTurn = correct;
        printf("\n X | ");
        for (int i = 0; i < 4; i++) {
            if (i == 3) {
                printf("%2d\n", easyCols[i]);
            } else {
                printf("%2d | ", easyCols[i]);
            }
        }
        for (int i = 0; i < 22; i++) {
            printf("-");
        }
        printf("\n");
        for (int i = 0; i < 4; i++) {
            printf("%2d | ", easyRows[i]);
            for (int j = 0; j < 4; j++) {
                if (j == 3) {
                    if (easyMatrix[i][j] == -1) {
                        printf("  \n");
                    } else {
                        printf("%2d\n", easyMatrix[i][j]);
                    }
                } else {
                    if (easyMatrix[i][j] == -1) {
                        printf("     ");
                    } else {
                        printf("%2d   ", easyMatrix[i][j]);
                    }
                }
            }
        }
        printf("\nVoce ainda tem %d vidas!\n", lives);
        printf("Selecione, respectivamente, a linha e a matriz do numero que voce deseja excluir (L C): \n\n");
        scanf("\n%d %d", &row, &col);
        for (int i = 0; i < 7; i++) {
            if (row == easyRowsDelete[i] && col == easyColsDelete[i]) {
                easyMatrix[row-1][col-1] = -1;
                correct++;
                if (correct == 7) {
                    printf("\nVoce ganhou! Parabens! Aperte qualquer tecla para continuar: ");
                    getchar();
                    getchar();
                    playing = 0;
                    return;
                }
                printf("\nVoce acertou, parabens! Aperte qualquer tecla para continuar.\n");
                getchar();
                getchar();
                break;
            }
        }
        if (correct == lastTurn) {
            lives--;
            printf("\nVoce errou. Ainda restam %d vidas! Aperte qualquer tecla para continuar: ", lives);
            getchar();
            getchar();
            printf("\n");
        }
        if (lives == 0) {
            printf("\nVoce perdeu! Aperte qualquer tecla para continuar: ");
            getchar();
            getchar();
            playing = 0;
        }
    }
}

void playMediumGame() {
    while (playing) {
        lastTurn = correct;
        printf("\n X | ");
        for (int i = 0; i < 6; i++) {
            if (i == 5) {
                printf("%2d\n", mediumCols[i]);
            } else {
                printf("%2d | ", mediumCols[i]);
            }
        }
        for (int i = 0; i < 32; i++) {
            printf("-");
        }
        printf("\n");
        for (int i = 0; i < 6; i++) {
            printf("%2d | ", mediumRows[i]);
            for (int j = 0; j < 6; j++) {
                if (j == 5) {
                    if (mediumMatrix[i][j] == -1) {
                        printf("  \n");
                    } else {
                        printf("%2d\n", mediumMatrix[i][j]);
                    }
                } else {
                    if (mediumMatrix[i][j] == -1) {
                        printf("     ");
                    } else {
                        printf("%2d   ", mediumMatrix[i][j]);
                    }
                }
            }
        }
        printf("\nVoce ainda tem %d vidas!\n", lives);
        printf("Selecione, respectivamente, a linha e a matriz do numero que voce deseja excluir (L C): \n\n");
        scanf("\n%d %d", &row, &col);
        for (int i = 0; i < 21; i++) {
            if (row == mediumRowsDelete[i] && col == mediumColsDelete[i]) {
                mediumMatrix[row-1][col-1] = -1;
                correct++;
                if (correct == 21) {
                    printf("\nVoce ganhou! Parabens! Aperte qualquer tecla para continuar: ");
                    getchar();
                    getchar();
                    playing = 0;
                    return;
                }
                printf("\nVoce acertou, parabens! Aperte qualquer tecla para continuar.\n");
                getchar();
                getchar();
                break;
            }
        }
        if (correct == lastTurn) {
            lives--;
            printf("\nVoce errou. Ainda restam %d vidas! Aperte qualquer tecla para continuar: ", lives);
            getchar();
            getchar();
            printf("\n");
        }
        if (lives == 0) {
            printf("\nVoce perdeu! Aperte qualquer tecla para continuar: ");
            getchar();
            getchar();
            playing = 0;
        }
    }
}

void suggestGame() {
    //TODO: implementar funcionalidade do player poder sugerir uma nova matriz (ULTIMA TAREFA)
}

void generateEasyGame() {
    FILE *easy = fopen("easy.txt", "r");
    if (easy == NULL) {
        printf("\nNao foi possivel iniciar a fase. Tente novamente mais tarde!\n");
        fclose(easy);
        return;
    }
    if (playing == 0) {
        srand(time(0));
        random = (rand() % 3);
        fseek(easy, (216), SEEK_SET);
    }
        fscanf(easy, "ID: %d\n", &level);
        fscanf(easy, "COLS: %d %d %d %d\n", &easyCols[0], &easyCols[1], &easyCols[2], &easyCols[3]);
        fscanf(easy, "ROWS: %d %d %d %d\n", &easyRows[0], &easyRows[1], &easyRows[2], &easyRows[3]);
        for (int i = 0; i < 7; i++) {
            fscanf(easy, "%d", &easyColsDelete[i]);
        }
        for (int i = 0; i < 7; i++) {
            fscanf(easy, "%d", &easyRowsDelete[i]);
        }
        for (int i = 0; i < 4; i++) {
            fscanf(easy, "%d %d %d %d\n", &easyMatrix[i][0], &easyMatrix[i][1], &easyMatrix[i][2], &easyMatrix[i][3]);
        }
        fclose(easy);
        playing = 1;
        correct = lastTurn = 0;
        playEasyGame();
}
void generateMediumGame() {
    FILE *medium = fopen("medium.txt", "r");
    if (medium == NULL) {
        printf("\nNao foi possivel iniciar a fase. Tente novamente mais tarde!\n");
        fclose(medium);
        return;
    }
    if (playing == 0) {
        //srand(time(0));
        //random = (rand() % 3);
        //fseek(medium, (mediumOffsets[random]), SEEK_SET);
    }
        fscanf(medium, "ID: %d\n", &level);
        fscanf(medium, "COLS: %d %d %d %d %d %d\n", &mediumCols[0], &mediumCols[1], &mediumCols[2], &mediumCols[3], &mediumCols[4], &mediumCols[5]);
        fscanf(medium, "ROWS: %d %d %d %d %d %d\n", &mediumRows[0], &mediumRows[1], &mediumRows[2], &mediumRows[3], &mediumRows[4], &mediumRows[5]);
        for (int i = 0; i < 21; i++) {
            fscanf(medium, "%d", &mediumColsDelete[i]);
        }
        for (int i = 0; i < 21; i++) {
            fscanf(medium, "%d", &mediumRowsDelete[i]);
        }
        for (int i = 0; i < 6; i++) {
            fscanf(medium, "%d %d %d %d %d %d\n", &mediumMatrix[i][0], &mediumMatrix[i][1], &mediumMatrix[i][2], &mediumMatrix[i][3], &mediumMatrix[i][4], &mediumMatrix[i][5]);
        }
        fclose(medium);
        playing = 1;
        correct = lastTurn = 0;
        playMediumGame();
}

void Play() {
    switch(dificulty) {
        case 1:
            generateEasyGame();
            break;
        case 2:
            generateMediumGame();
            break;
        case 3:
            //FILE *medium = fopen("hard.txt", "r");
            break;
    }
    if (lives != 0) {
        player.score += 100*dificulty;
        FILE *file = fopen("ranking.bin", "rb");
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
        FILE *file2 = fopen("ranking.bin", "wb");
        fwrite(&played, sizeof(int), 1, file2);
        for (int i = 0; i < played; i++) {
            fwrite(&ranking[i], sizeof(players), 1, file2);
        }
        fclose(file2);
    }
}

int main() {
    menu = settings = chooseDificulty = ranking = played = 0;
    char menuOption;
    EnterNickname();
    while (menu) {
        lives = 5;
        printMenu();
        scanf("\n%c", &menuOption);
        printf("\n");
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
                                printf("\n");
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