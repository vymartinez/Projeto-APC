#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct players {
    char name[25];
    int score;
} players;

typedef struct matrix {
    int number;
    int delete;
} matrix;

//LOOPS
int menu, playing, settings, suggesting, ranking, chooseDificulty, played, addingGame, block = 0;
//VARIABLES
int correct, lastTurn, lives, random, level, row, col, lastOffset;
int colsSum[7], rowsSum[7];
int deletes, totals[3];
int *easyOffsets, *mediumOffsets, *hardOffsets;
int solved[3] = {0, 0, 0};
int traces[3] = {22, 32, 37};
int available[3] = {1, 1, 1};
matrix mainMatrix[7][7];
int dificulty = 1;
char modes[3][10] = {"Facil", "Medio", "Dificil"};
players player;

int compare(players *a, players *b) {
    if (a->score < b->score) {
        return 1;
    }
    return -1;
}

void clear(){
    #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        system("clear");
    #endif

    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #endif
}

void setupGame() {
    FILE *offsets = fopen("offsets.txt", "r");
    fscanf(offsets, "%d\n", &totals[0]);
    easyOffsets = (int*)calloc(totals[0], sizeof(int));
    for (int i = 0; i < totals[0]; i++) {
        if (i == totals[0]-1) {
            fscanf(offsets, "%d\n", &easyOffsets[i]);
        } else {
            fscanf(offsets, "%d ", &easyOffsets[i]);
        }
    }
    fscanf(offsets, "%d\n", &totals[1]);
    mediumOffsets = (int*)calloc(totals[1], sizeof(int));
    for (int i = 0; i < totals[1]; i++) {
        if (i == totals[1]-1) {
            fscanf(offsets, "%d\n", &mediumOffsets[i]);
        } else {
            fscanf(offsets, "%d ", &mediumOffsets[i]);
        }
    }
    fscanf(offsets, "%d\n", &totals[2]);
    hardOffsets = (int*)calloc(totals[2], sizeof(int));
    for (int i = 0; i < totals[2]; i++) {
        if (i == totals[2]-1) {
            fscanf(offsets, "%d\n", &hardOffsets[i]);
        } else {
            fscanf(offsets, "%d ", &hardOffsets[i]);
        }
    }
    fclose(offsets);
}

void verifyEndgame() {
    if (solved[0] == totals[0] && solved[1] == totals[1] && solved[2] == totals[2]) {
        printf("\n\nParabens! Voce completou todos os niveis! Voce ZEROU o jogo!!! Aperte qualquer tecla para encerrar o jogo: ");
        getchar();
        getchar();
        block = 1;
    } else {
        for (int i = 0; i < 3; i++) {
            if (available[i] == 1) {
                dificulty = i+1;
                break;
            }
        }
    }
    return;
}

void EnterNickname() {
    printf("\nSeja bem-vindo(a) ao meu Number Sums! Vamos comecar!!\n");
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
    player.score = 0;
    played = 1;
    FILE *file = fopen("ranking.bin","wb");
    fwrite(&played, sizeof(int), 1, file);
    fwrite(&player, sizeof(players), 1, file);
    fclose(file);
    printf("\n\nRanking reiniciado com sucesso! Pressione qualquer tecla para continuar: ");
    getchar();
    getchar();
    printf("\n");
    setupGame();
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

void printSuggestLevel() {
    printf("\n");
    printf("                  Sugerir novo nivel                \n");
    printf("################################################\n\n");
    printf("[1] - Facil (4x4)\n");
    printf("[2] - Medio (6x6)\n");
    printf("[3] - Dificil (7x7)\n");
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

void verifyChoice(int line, int col, int targetLineSum, int lineSum, int targetColSum, int colSum) {
    if (targetLineSum == lineSum) {
        printf("\nMuito bem! Voce fechou a linha %d! Aperte qualquer tecla para continuar: ", line);
        getchar();
        rowsSum[line-1] = -1;
    }
    if (targetColSum == colSum) {
        printf("\nMuito bem! Voce fechou a coluna %d! Aperte qualquer tecla para continuar: ", col);
        getchar();
        colsSum[col-1] = -1;
    }
    return;
}

void playGame(int size) {
    while (playing) {
        lastTurn = correct;
        printf("\n X | ");
        for (int i = 0; i < size; i++) {
            if (i == size-1) {
                if (colsSum[i] == -1) {
                    printf("   | \n");
                } else {
                    printf("%2d\n", colsSum[i]);
                }
            } else {
                if (colsSum[i] == -1) {
                    printf("   | ");
                } else {
                    printf("%2d | ", colsSum[i]);
                }
            }
        }
        for (int i = 0; i < traces[dificulty-1]; i++) {
            printf("-");
        }
        printf("\n");
        for (int i = 0; i < size; i++) {
            if (rowsSum[i] == -1) {
                printf("   | ");
            } else {
                printf("%2d | ", rowsSum[i]);
            }
            for (int j = 0; j < size; j++) {
                if (j == size-1) {
                    if (mainMatrix[i][j].delete == -1) {
                        printf("  \n");
                    } else {
                        printf("%2d\n", mainMatrix[i][j].number);
                    }
                } else {
                    if (mainMatrix[i][j].delete == -1) {
                        printf("     ");
                    } else {
                        printf("%2d   ", mainMatrix[i][j].number);
                    }
                }
            }
        }
        printf("\nVoce ainda tem %d vidas!\n", lives);
        printf("Selecione, respectivamente, a linha e a matriz do numero que voce deseja excluir (L C): ");
        scanf("\n%d %d", &row, &col);
        switch (mainMatrix[row-1][col-1].delete) {
            case -1:
                printf("\n\nOps, vc ja selecionou este espaco! Digite qualquer tecla para continuar: ");
                getchar();
                getchar();
                break;
            case 0:
                mainMatrix[row-1][col-1].delete = -1;
                correct++;
                if (correct == deletes) {
                    printf("\n\nVoce ganhou! Parabens! Aperte qualquer tecla para continuar: ");
                    getchar();
                    getchar();
                    playing = 0;
                    return;
                }
                printf("\n\nVoce acertou, parabens! Aperte qualquer tecla para continuar.\n");
                getchar();
                getchar();
                int lineSum, colSum;
                lineSum = colSum = 0;
                for (int i = 0; i < size; i++) {
                    if (mainMatrix[i][col-1].delete != -1) {
                        colSum += mainMatrix[i][col-1].number;
                    }
                }
                for (int i = 0; i < size; i++) {
                    if (mainMatrix[row-1][i].delete != -1) {
                        lineSum += mainMatrix[row-1][i].number;
                    }
                }
                verifyChoice(row, col, rowsSum[row-1], lineSum, colsSum[col-1], colSum);
                break;
            case 1:
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
                break;
            default:
                printf("\n\nSelecione uma opcao valida! Aperte qualquer tecla para tentar novamente:  ");
                getchar();
                getchar();
                printf("\n");
                break;
        }
        clear();
    }
}

void verifySuggestion(int choice, int size) {
    int sum;
    char answer;
    switch (choice) {
        case 1:
            for (int i = 0; i < size; i++) {
                sum = 0;
                for (int j = 0; j < size; j++) {
                    if (mainMatrix[i][j].delete == 1) {
                        sum += mainMatrix[i][j].number;
                    }
                }
                if (sum != rowsSum[i]) {
                    printf("\nA soma dos numeros da linha %d nao corresponde a resposta (%d). Tente novamente.\n", i+1, rowsSum[i]);
                    printf("\nVoce deseja alterar o numero %d? (S/N): ", rowsSum[i]);
                    scanf("\n%c", &answer);
                    printf("\n");
                    switch (answer) {
                        case 'S':
                            printf("\nDigite um novo valor para esse numero: ");
                            scanf("\n%d", &rowsSum[i]);
                            verifySuggestion(choice, size);
                            return;
                            break;
                        case 'N':
                            break;
                        default:
                            printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                            break;
                    }
                    for (int k = 0; k < size; k++) {
                        printf("\nVoce deseja alterar o numero %d? (S/N): ", mainMatrix[i][k].number);
                        scanf("\n%c", &answer);
                        printf("\n");
                        switch (answer) {
                            case 'S':
                                printf("\nVoce deseja alterar o valor ou se deve, ou nao, ser deletado? (V/D): ");
                                scanf("\n%c", &answer);
                                printf("\n");
                                switch (answer) {
                                    case 'V':
                                        printf("\nDigite um novo valor para esse numero: ");
                                        scanf("\n%d", &mainMatrix[i][k].number);
                                        verifySuggestion(choice, size);
                                        return;
                                        break;
                                    case 'D':
                                        printf("\nEste numero deve ser excluido? (S/N): ");
                                        scanf("\n%c", &answer);
                                        printf("\n");
                                        switch (answer) {
                                            case 'S':
                                                mainMatrix[i][k].delete = 0;
                                                verifySuggestion(choice, size);
                                                break;
                                            case 'N':
                                                mainMatrix[i][k].delete = 1;
                                                 verifySuggestion(choice, size);
                                                break;
                                            default:
                                                printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                                                break;
                                        }
                                    default:
                                        printf("\nOpcao invalida! Digite V para alterar o valor ou D para deletar o numero.\n");
                                        break;
                                }
                                break;
                            case 'N':
                                break;
                            default:
                                printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                                break;
                        }
                    }
                }
            }
            for (int i = 0; i < size; i++) {
                sum = 0;
                for (int j = 0; j < size; j++) {
                    if (mainMatrix[j][i].delete == 1) {
                        sum += mainMatrix[j][i].number;
                    }
                }
                if (sum != colsSum[i]) {
                    printf("\nA soma dos numeros da linha %d nao corresponde a resposta (%d). Tente novamente.\n", i+1, colsSum[i]);
                    printf("\nVoce deseja alterar o numero %d? (S/N): ", colsSum[i]);
                    scanf("\n%c", &answer);
                    printf("\n");
                    switch (answer) {
                        case 'S':
                            printf("\nDigite um novo valor para esse numero: ");
                            scanf("\n%d", &colsSum[i]);
                            verifySuggestion(choice, size);
                            return;
                            break;
                        case 'N':
                            break;
                        default:
                            printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                            break;
                    }
                    for (int k = 0; k < size; k++) {
                        printf("\nVoce deseja alterar o numero %d? (S/N): ", mainMatrix[k][i].number);
                        scanf("\n%c", &answer);
                        printf("\n");
                        switch (answer) {
                            case 'S':
                                printf("\nVoce deseja alterar o valor ou se deve, ou nao, ser deletado? (V/D): ");
                                scanf("\n%c", &answer);
                                printf("\n");
                                switch (answer) {
                                    case 'V':
                                        printf("\nDigite um novo valor para esse numero: ");
                                        scanf("\n%d", &mainMatrix[k][i].number);
                                        verifySuggestion(choice, size);
                                        return;
                                        break;
                                    case 'D':
                                        printf("\nEste numero deve ser excluido? (S/N): ");
                                        scanf("\n%c", &answer);
                                        printf("\n");
                                        switch (answer) {
                                            case 'S':
                                                mainMatrix[k][i].delete = 0;
                                                verifySuggestion(choice, size);
                                                break;
                                            case 'N':
                                                mainMatrix[k][i].delete = 1;
                                                verifySuggestion(choice, size);
                                                break;
                                            default:
                                                printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                                                break;
                                        }
                                    default:
                                        printf("\nOpcao invalida! Digite V para alterar o valor ou D para deletar o numero.\n");
                                        break;
                                }
                                break;
                            case 'N':
                                break;
                            default:
                                printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                                break;
                        }
                    }
                }
            }
            addingGame = 1;
            return;
    }
}

void suggestGame(int choice, int size) {
    memset(mainMatrix, -1, sizeof(mainMatrix));
    memset(colsSum, -1, sizeof(colsSum));
    memset(rowsSum, -1, sizeof(rowsSum));
    addingGame = 0;
    char answer;
    for (int i = 0; i < size; i++) {
        while (suggesting) {    
            printf("\nDigite a resposta da coluna %d: ", i+1);
            scanf("\n%d", &colsSum[i]);
            break;
        }
        clear();
    }
    for (int i = 0; i < size; i++) {
        while (suggesting) {
            printf("\nDigite a resposta da linha %d: ", i+1);
            scanf("\n%d", &rowsSum[i]);
            break;
        }
        clear();
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            while (suggesting) {
                printf("\n X | ");
                for (int i = 0; i < size; i++) {
                    if (i == size-1) {
                        if (colsSum[i] == -1) {
                            printf("   | \n");
                        } else {
                            printf("%2d\n", colsSum[i]);
                        }
                    } else {
                        if (colsSum[i] == -1) {
                            printf("   | ");
                        } else {
                            printf("%2d | ", colsSum[i]);
                        }
                    }
                }
                for (int i = 0; i < traces[dificulty-1]; i++) {
                    printf("-");
                }
                printf("\n");
                for (int i = 0; i < size; i++) {
                    if (rowsSum[i] == -1) {
                        printf("   | ");
                    } else {
                        printf("%2d | ", rowsSum[i]);
                    }
                    for (int j = 0; j < size; j++) {
                        if (j == size-1) {
                            if (mainMatrix[i][j].delete == -1) {
                                printf("  \n");
                            } else {
                                printf("%2d\n", mainMatrix[i][j].number);
                            }
                        } else {
                            if (mainMatrix[i][j].delete == -1) {
                                printf("     ");
                            } else {
                                printf("%2d   ", mainMatrix[i][j].number);
                            }
                        }
                    }
                }
                printf("\nDigite o numero da linha %d e coluna %d: ", i+1, j+1);
                scanf("\n%d", &mainMatrix[i][j].number);
                printf("\n");
                if (mainMatrix[i][j].number < 0 || mainMatrix[i][j].number > 9) {
                    printf("\nNumero invalido! Digite um numero entre 0 e 9, por favor.\n");
                } else {
                    printf("\nEste numero deve ser excluido? (S/N): ");
                    scanf("\n%c", &answer);
                    printf("\n");
                    switch (answer) {
                        case 'S':
                            mainMatrix[i][j].delete = 0;
                            break;
                        case 'N':
                            mainMatrix[i][j].delete = 1;
                            break;
                        default:
                            printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                            break;
                    }
                    break;
                }
                clear();
            }
            }
        }
    verifySuggestion(choice, size);
    while (addingGame) {
        FILE *file;
        switch (choice) {
            case 1:
                file = fopen("easy.txt", "r");
                fseek(file, 0, SEEK_END);
                lastOffset = ftell(file);
                fclose(file);
                file = fopen("easy.txt", "a");
                break;
            case 2:
                file = fopen("medium.txt", "r");
                fseek(file, 0, SEEK_END);
                lastOffset = ftell(file);
                fclose(file);
                file = fopen("medium.txt", "a");
                break;
            case 3:
                file = fopen("hard.txt", "r");
                fseek(file, 0, SEEK_END);
                lastOffset = ftell(file);
                fclose(file);
                file = fopen("hard.txt", "a");
                break; 
        }
        fprintf(file, "\n");
        for (int i = 0; i < size; i++) {
            if (i == size-1) {
                fprintf(file, "%d\n", colsSum[i]);
            } else {
                fprintf(file, "%d ", colsSum[i]);
            }
        }
        for (int i = 0; i < size; i++) {
            if (i == size-1) {
                fprintf(file, "%d\n", rowsSum[i]);
            } else {
                fprintf(file, "%d ", rowsSum[i]);
            }
        }
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (j == size-1) {
                    fprintf(file, "%d\n", mainMatrix[i][j].number);
                } else {
                    fprintf(file, "%d ", mainMatrix[i][j].number);
                }
            }
        }
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (j == size-1) {
                    fprintf(file, "%d\n", mainMatrix[i][j].delete);
                } else {
                    fprintf(file, "%d ", mainMatrix[i][j].delete);
                }
            }
        }
        fprintf(file, "*");
        fclose(file);
        FILE* offsets = fopen("offsets.txt", "w");
        totals[choice-1]++;
        easyOffsets = (int*)realloc(easyOffsets, totals[0] * sizeof(int));
        switch (choice) {
            case 1:
                easyOffsets = (int*)realloc(easyOffsets, totals[0] * sizeof(int));
                easyOffsets[totals[choice-1]-1] = lastOffset;
                break;
            case 2:
                mediumOffsets = (int*)realloc(mediumOffsets, totals[1] * sizeof(int));
                mediumOffsets[totals[choice-1]-1] = lastOffset;
                break;
            case 3:
                hardOffsets = (int*)realloc(hardOffsets, totals[2] * sizeof(int));
                hardOffsets[totals[choice-1]-1] = lastOffset;
                break;
        }
        fprintf(offsets, "%d\n", totals[0]);
        for (int i = 0; i < totals[0]; i++) {
            if (i == totals[0]-1) {
                fprintf(offsets, "%d\n", easyOffsets[i]);
            } else {
                fprintf(offsets, "%d ", easyOffsets[i]);
            }
        }
        fprintf(offsets, "%d\n", totals[1]);
        for (int i = 0; i < totals[1]; i++) {
            if (i == totals[1]-1) {
                fprintf(offsets, "%d\n", mediumOffsets[i]);
            } else {
                fprintf(offsets, "%d ", mediumOffsets[i]);
            }
        }
        fprintf(offsets, "%d\n", totals[2]);
        for (int i = 0; i < totals[2]; i++) {
            if (i == totals[2]-1) {
                fprintf(offsets, "%d\n", hardOffsets[i]);
            } else {
                fprintf(offsets, "%d ", hardOffsets[i]);
            }
        }
        fclose(offsets);
        addingGame = 0;
        break;
    }
    printf("\nNova fase adicionada com sucesso!\n");
    suggesting = 0;
    menu = 1;
}

void generateGame(int size) {
    deletes = 0;
    FILE *file;
    switch (dificulty) {
        case 1:
            file = fopen("easy.txt", "r");
            break;
        case 2:
            file = fopen("medium.txt", "r");
            break;
        case 3:
            file = fopen("hard.txt", "r");
            break;
    }
    if (playing == 0) {
        srand(time(0));
        random = (rand() % totals[dificulty-1]);
        switch (dificulty) {
            case 1:
                fseek(file, easyOffsets[random], SEEK_SET);
                break;
            case 2:
                fseek(file, mediumOffsets[random], SEEK_SET);
                break;
            case 3:
                fseek(file, hardOffsets[random], SEEK_SET);
                break;
        }
    }
        for (int i = 0; i < size; i++) {
            if (i == size-1) {
                fscanf(file, "%d\n", &colsSum[i]);
            } else {
                fscanf(file, "%d ", &colsSum[i]);
            }
        }
        for (int i = 0; i < size; i++) {
            if (i == size-1) {
                fscanf(file, "%d\n", &rowsSum[i]);
            } else {
                fscanf(file, "%d ", &rowsSum[i]);
            }
        }
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (j == size-1) {
                    fscanf(file, "%d\n", &mainMatrix[i][j].number);
                } else {
                    fscanf(file, "%d ", &mainMatrix[i][j].number);
                }
            }
        }
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (j == size-1) {
                    fscanf(file, "%d\n", &mainMatrix[i][j].delete);
                } else {
                    fscanf(file, "%d ", &mainMatrix[i][j].delete);
                }
                if (mainMatrix[i][j].delete == 0) {
                    deletes++;
                }
            }
        }
        fclose(file);
        playing = 1;
        correct = lastTurn = 0;
        playGame(size);
}

void Play() {
    memset(mainMatrix, 0, sizeof(mainMatrix));
    memset(colsSum, 0, sizeof(colsSum));
    memset(rowsSum, 0, sizeof(rowsSum));
    switch(dificulty) {
        case 1:
            generateGame(4);
            break;
        case 2:
            generateGame(6);
            break;
        case 3:
            generateGame(7);
            break;
    }
    if (lives != 0) {
        player.score += 100 * dificulty;
        solved[dificulty-1]++;
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
        int newRandom;
        switch(dificulty) {
            case 1:
                if (solved[dificulty-1] == totals[0]) {
                    available[dificulty-1] = 0;
                    printf("\nParabens! Voce completou todas as fases faceis! Aperte qualquer tecla para continuar: \n");
                    getchar();
                    verifyEndgame();
                } else {
                    newRandom = random+2 < totals[0] ? random+1 : random-1;
                    easyOffsets[random] = easyOffsets[newRandom];
                }
                break;
            case 2:
                if (solved[dificulty-1] == totals[1]) {
                    available[dificulty-1] = 0;
                    printf("\nParabens! Voce completou todas as fases medias! Aperte qualquer tecla para continuar: \n");
                    getchar();
                    verifyEndgame();
                } else {
                    newRandom = random+2 < totals[1] ? random+1 : random-1;
                    mediumOffsets[random] = mediumOffsets[newRandom];
                }
                break;
            case 3:
                if (solved[dificulty-1] == totals[2]) {
                    available[dificulty-1] = 0;
                    printf("\nParabens! Voce completou todas as fases dificeis! Aperte qualquer tecla para continuar: \n");
                    getchar();
                    verifyEndgame();
                } else {
                    newRandom = random+2 < totals[2] ? random+1 : random-1;
                    hardOffsets[random] = hardOffsets[newRandom];
                }
                break;
        }
    }
}

int main() {
    menu = settings = chooseDificulty = ranking = played = 0;
    setupGame();
    char menuOption;
    EnterNickname();
    while (menu && !block) {
        clear();
        lives = 5;
        printMenu();
        scanf("\n%c", &menuOption);
        switch(menuOption) {
            case '1':
                clear();
                Play();
                break;
            case '2':
                clear();
                settings = 1;
                while (settings) {
                    clear();
                    printConfig();
                    scanf("\n%c", &menuOption);
                    switch(menuOption) {
                        case '1':
                            settings = 0;
                            chooseDificulty = 1;
                            while (chooseDificulty) {
                                clear();
                                printDificulty();
                                scanf("\n%c", &menuOption);
                                switch(menuOption) {
                                    case '1':
                                        if (totals[0] == solved[0]) {
                                            printf("\nVoce ja completou todas as fases faceis! Aperte qualquer tecla para continuar: \n");
                                            getchar();
                                            getchar();
                                            break;
                                        }
                                        dificulty = 1;
                                        chooseDificulty = 0;
                                        break;
                                    case '2':
                                        if (totals[1] == solved[1]) {
                                            printf("\nVoce ja completou todas as fases medias! Aperte qualquer tecla para continuar: \n");
                                            getchar();
                                            getchar();
                                            break;
                                        }
                                        dificulty = 2;
                                        chooseDificulty = 0;
                                        break;
                                    case '3':
                                        if (totals[2] == solved[2]) {
                                            printf("\nVoce ja completou todas as fases dificeis! Aperte qualquer tecla para continuar: \n");
                                            getchar();
                                            getchar();
                                            break;
                                        }
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
                            suggesting = 1;
                            while (suggesting) {
                                clear();
                                printSuggestLevel();
                                scanf("\n%c", &menuOption);
                                switch(menuOption) {
                                    case '1':
                                        clear();
                                        suggestGame(1, 4);
                                        break;
                                    case '2':
                                        clear();
                                        suggestGame(2, 6);
                                        break;
                                    case '3':
                                        clear();
                                        suggestGame(3, 7);
                                        break;
                                    case '4':
                                        suggesting = 0;
                                        settings = 1;
                                        break;
                                    default:
                                        printf("\nOpcao invalida! Selecione uma opcao de configuracao, por favor.\n");
                                        break;
                                }
                            }
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
    return 0;
}