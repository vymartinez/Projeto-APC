#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct players {
    char name[25];
    int score;
} players;

typedef struct matrix {
    int number;
    int delete;
} matrix;

int menu, playing, settings, ranking, chooseDificulty, played, random, level, row, col,
correct, lastTurn, lives, easyDeletes, mediumDeletes, hardDeletes, totalEasy, totalMedium, totalHard;
int easyCols[4], easyRows[4], mediumCols[6], mediumRows[6], hardCols[7], hardRows[7];
int *easyOffsets, *mediumOffsets, *hardOffsets;
matrix easyMatrix[4][4], mediumMatrix[6][6], hardMatrix[7][7];
int lastEasyOffset, lastMediumOffset, lastHardOffset;
int dificulty = 1;
char modes[3][10] = {"Facil", "Medio", "Dificil"};
players player;

int compare(players *a, players *b) {
    if (a->score < b->score) {
        return 1;
    }
    return -1;
}

void setupGame() {
    FILE *easy = fopen("easy.txt", "r");
    fscanf(easy, "%d\n", &totalEasy);
    easyOffsets = (int*)calloc(totalEasy, sizeof(int));
    for (int i = 0; i < totalEasy; i++) {
        if (i == totalEasy-1) {
            fscanf(easy, "%d\n", &easyOffsets[i]);
        } else {
            fscanf(easy, "%d ", &easyOffsets[i]);
        }
    }
    fclose(easy);
    FILE *medium = fopen("medium.txt", "r");
    fscanf(medium, "%d", &totalMedium);
    mediumOffsets = (int*)calloc(totalMedium, sizeof(int));
    for (int i = 0; i < totalMedium; i++) {
        if (i == totalMedium-1) {
            fscanf(medium, "%d\n", &mediumOffsets[i]);
        } else {
            fscanf(medium, "%d ", &mediumOffsets[i]);
        }
    }
    fclose(medium);
    FILE *hard = fopen("hard.txt", "r");
    fscanf(hard, "%d", &totalHard);
    hardOffsets = (int*)calloc(totalHard, sizeof(int));
    for (int i = 0; i < totalHard; i++) {
        if (i == totalHard-1) {
            fscanf(hard, "%d\n", &hardOffsets[i]);
        } else {
            fscanf(hard, "%d ", &hardOffsets[i]);
        }
    }
    fclose(hard);
}

void EnterNickname() {
    printf("\nDigite seu nome(ate 20 caracteres): ");
    scanf("%s", player.name);
    int len = strlen(player.name);
    if (len > 20) {
        printf("\nNome invalido! Digite um nome valido, por favor.\n");
        EnterNickname();
        return;
    }
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

void resetRanking() {
    FILE *file = fopen("ranking.bin","wb");
    fwrite(&played, sizeof(int), 1, file);
    fwrite(&player, sizeof(players), 1, file);
    fclose(file);
    printf("\n\nRanking reiniciado com sucesso! Pressione qualquer tecla para continuar: ");
    getchar();
    getchar();
    printf("\n");
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
    printf("[3] - Resetar ranking\n");
    printf("[4] - Voltar\n\n");
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
                    if (easyMatrix[i][j].delete == -1) {
                        printf("  \n");
                    } else {
                        printf("%2d\n", easyMatrix[i][j].number);
                    }
                } else {
                    if (easyMatrix[i][j].delete == -1) {
                        printf("     ");
                    } else {
                        printf("%2d   ", easyMatrix[i][j].number);
                    }
                }
            }
        }
        if (lives == 1) {
            printf("\nVoce ainda tem %d vida!\n", lives);
        } else {
            printf("\nVoce ainda tem %d vidas!\n", lives);
        }
        printf("Selecione, respectivamente, a linha e a matriz do numero que voce deseja excluir (L C): ");
        scanf("\n%d %d", &row, &col);
        if (easyMatrix[row-1][col-1].delete == -1) {
            printf("\n\nOps, vc já selecionou este espaço! Digite qualquer tecla para continuar: ");
            getchar();
            getchar();
        } else {
            if (easyMatrix[row-1][col-1].delete == 0) {
                easyMatrix[row-1][col-1].delete = -1;
                correct++;
                if (correct == easyDeletes) {
                    printf("\nVoce ganhou! Parabens! Aperte qualquer tecla para continuar: ");
                    getchar();
                    getchar();
                    playing = 0;
                    return;
                }
                printf("\nVoce acertou, parabens! Aperte qualquer tecla para continuar.\n");
                getchar();
                getchar();
            }
            if (correct == lastTurn) {
                lives--;
                printf("\n\nVoce errou. Ainda restam %d vidas! Aperte qualquer tecla para continuar: ", lives);
                getchar();
                getchar();
                printf("\n");
            }
            if (lives == 0) {
                printf("\n\nVoce perdeu! Aperte qualquer tecla para continuar: ");
                getchar();
                getchar();
                playing = 0;
            }
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
                    if (mediumMatrix[i][j].delete == -1) {
                        printf("  \n");
                    } else {
                        printf("%2d\n", mediumMatrix[i][j].number);
                    }
                } else {
                    if (mediumMatrix[i][j].delete == -1) {
                        printf("     ");
                    } else {
                        printf("%2d   ", mediumMatrix[i][j].number);
                    }
                }
            }
        }
        printf("\nVoce ainda tem %d vidas!\n", lives);
        printf("Selecione, respectivamente, a linha e a matriz do numero que voce deseja excluir (L C): ");
        scanf("\n%d %d", &row, &col);
        if (mediumMatrix[row-1][col-1].delete == -1) {
            printf("\n\nOps, vc já selecionou este espaço! Digite qualquer tecla para continuar: ");
            getchar();
            getchar();
        } else {
            if (mediumMatrix[row-1][col-1].delete == 0) {
                mediumMatrix[row-1][col-1].delete = -1;
                correct++;
                if (correct == mediumDeletes) {
                    printf("\n\nVoce ganhou! Parabens! Aperte qualquer tecla para continuar: ");
                    getchar();
                    getchar();
                    playing = 0;
                    return;
                }
                printf("\n\nVoce acertou, parabens! Aperte qualquer tecla para continuar.\n");
                getchar();
                getchar();
            }
            if (correct == lastTurn) {
                lives--;
                printf("\n\nVoce errou. Ainda restam %d vidas! Aperte qualquer tecla para continuar: ", lives);
                getchar();
                getchar();
                printf("\n");
            }
            if (lives == 0) {
                printf("\n\nVoce perdeu! Aperte qualquer tecla para continuar: ");
                getchar();
                getchar();
                playing = 0;
            }
        }
    }
}

void playHardGame() {
    while (playing) {
        lastTurn = correct;
        printf("\n X | ");
        for (int i = 0; i < 7; i++) {
            if (i == 6) {
                printf("%2d\n", hardCols[i]);
            } else {
                printf("%2d | ", hardCols[i]);
            }
        }
        for (int i = 0; i < 37; i++) {
            printf("-");
        }
        printf("\n");
        for (int i = 0; i < 7; i++) {
            printf("%2d | ", hardRows[i]);
            for (int j = 0; j < 7; j++) {
                if (j == 6) {
                    if (hardMatrix[i][j].delete == -1) {
                        printf("  \n");
                    } else {
                        printf("%2d\n", hardMatrix[i][j].number);
                    }
                } else {
                    if (hardMatrix[i][j].delete == -1) {
                        printf("     ");
                    } else {
                        printf("%2d   ", hardMatrix[i][j].number);
                    }
                }
            }
        }
        printf("\nVoce ainda tem %d vidas!\n", lives);
        printf("Selecione, respectivamente, a linha e a matriz do numero que voce deseja excluir (L C): ");
        scanf("\n%d %d", &row, &col);
        if (hardMatrix[row-1][col-1].delete == -1) {
            printf("\n\nOps, vc já selecionou este espaço! Digite qualquer tecla para continuar: ");
            getchar();
            getchar();
        } else {
            if (hardMatrix[row-1][col-1].delete == 0) {
                hardMatrix[row-1][col-1].delete = -1;
                correct++;
                if (correct == hardDeletes) {
                    printf("\n\nVoce ganhou! Parabens! Aperte qualquer tecla para continuar: ");
                    getchar();
                    getchar();
                    playing = 0;
                    return;
                }
                printf("\n\nVoce acertou, parabens! Aperte qualquer tecla para continuar.\n");
                getchar();
                getchar();
            }
            if (correct == lastTurn) {
                lives--;
                printf("\n\nVoce errou. Ainda restam %d vidas! Aperte qualquer tecla para continuar: ", lives);
                getchar();
                getchar();
                printf("\n");
            }
            if (lives == 0) {
                printf("\n\nVoce perdeu! Aperte qualquer tecla para continuar: ");
                getchar();
                getchar();
                playing = 0;
            }
        }
    }
}

void suggestGame() {
    //TODO: implementar funcionalidade do player poder sugerir uma nova matriz (ULTIMA TAREFA)
}

void generateEasyGame() {
    easyDeletes = 0;
    FILE *easy = fopen("easy.txt", "r");
    if (easy == NULL) {
        printf("\nNao foi possivel iniciar a fase. Tente novamente mais tarde!\n");
        fclose(easy);
        return;
    }
    if (playing == 0) {
        srand(time(0));
        random = (rand() % 3);
        fseek(easy, easyOffsets[random], SEEK_SET);
    }
        fscanf(easy, "%d %d %d %d\n", &easyCols[0], &easyCols[1], &easyCols[2], &easyCols[3]);
        fscanf(easy, "%d %d %d %d\n", &easyRows[0], &easyRows[1], &easyRows[2], &easyRows[3]);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (j == 3) {
                    fscanf(easy, "%d\n", &easyMatrix[i][j].number);
                } else {
                    fscanf(easy, "%d ", &easyMatrix[i][j].number);
                }
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (j == 3) {
                    fscanf(easy, "%d\n", &easyMatrix[i][j].delete);
                } else {
                    fscanf(easy, "%d ", &easyMatrix[i][j].delete);
                }
                if (easyMatrix[i][j].delete == 0) {
                    easyDeletes++;
                }
            }
        }
        fclose(easy);
        playing = 1;
        correct = lastTurn = 0;
        playEasyGame();
}
void generateMediumGame() {
    mediumDeletes = 0;
    FILE *medium = fopen("medium.txt", "r");
    if (medium == NULL) {
        printf("\nNao foi possivel iniciar a fase. Tente novamente mais tarde!\n");
        fclose(medium);
        return;
    }
    if (playing == 0) {
        srand(time(0));
        random = (rand() % 3);
        fseek(medium, mediumOffsets[random], SEEK_SET);
    }
        fscanf(medium, "%d %d %d %d %d %d\n", &mediumCols[0], &mediumCols[1], &mediumCols[2], &mediumCols[3], &mediumCols[4], &mediumCols[5]);
        fscanf(medium, "%d %d %d %d %d %d\n", &mediumRows[0], &mediumRows[1], &mediumRows[2], &mediumRows[3], &mediumRows[4], &mediumRows[5]);
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                if (j == 3) {
                    fscanf(medium, "%d\n", &mediumMatrix[i][j].number);
                } else {
                    fscanf(medium, "%d ", &mediumMatrix[i][j].number);
                }
            }
        }
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                if (j == 3) {
                    fscanf(medium, "%d\n", &mediumMatrix[i][j].delete);
                } else {
                    fscanf(medium, "%d ", &mediumMatrix[i][j].delete);
                }
                if (mediumMatrix[i][j].delete == 0) {
                    mediumDeletes++;
                }
            }
        }
        fclose(medium);
        playing = 1;
        correct = lastTurn = 0;
        playMediumGame();
}

void generateHardGame() {
    FILE *hard = fopen("hard.txt", "r");
    if (hard == NULL) {
        printf("\nNao foi possivel iniciar a fase. Tente novamente mais tarde!\n");
        fclose(hard);
        return;
    }
    if (playing == 0) {
        srand(time(0));
        random = (rand() % 3);
        fseek(hard, hardOffsets[random], SEEK_SET);
    }
        fscanf(hard, "%d %d %d %d %d %d %d\n", &hardCols[0], &hardCols[1], &hardCols[2], &hardCols[3], &hardCols[4], &hardCols[5], &hardCols[6]);
        fscanf(hard, "%d %d %d %d %d %d %d\n", &hardRows[0], &hardRows[1], &hardRows[2], &hardRows[3], &hardRows[4], &hardRows[5], &hardRows[6]);
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 7; j++) {
                if (j == 3) {
                    fscanf(hard, "%d\n", &hardMatrix[i][j].number);
                } else {
                    fscanf(hard, "%d ", &hardMatrix[i][j].number);
                }
            }
        }
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 7; j++) {
                if (j == 3) {
                    fscanf(hard, "%d\n", &hardMatrix[i][j].delete);
                } else {
                    fscanf(hard, "%d ", &hardMatrix[i][j].delete);
                }
                if (hardMatrix[i][j].delete == 0) {
                    hardDeletes++;
                }
            }
        }
        fclose(hard);
        playing = 1;
        correct = lastTurn = 0;
        playHardGame();
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
            generateHardGame();
            break;
    }
    if (lives != 0) {
        player.score += 100 * dificulty;
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
    setupGame();
    char menuOption;
    EnterNickname();
    while (menu) {
        lives = 5;
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
                            resetRanking();
                        case '4':
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