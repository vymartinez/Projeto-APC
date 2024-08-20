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

int menu, playing, settings, suggesting, ranking, chooseDificulty, played, random, level, row, col,
correct, lastTurn, lives, easyDeletes, mediumDeletes, hardDeletes, totalEasy, totalMedium, totalHard, addingGame;
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
    FILE *offsets = fopen("offsets.txt", "r");
    fscanf(offsets, "%d\n", &totalEasy);
    easyOffsets = (int*)calloc(totalEasy, sizeof(int));
    for (int i = 0; i < totalEasy; i++) {
        if (i == totalEasy-1) {
            fscanf(offsets, "%d\n", &easyOffsets[i]);
        } else {
            fscanf(offsets, "%d ", &easyOffsets[i]);
        }
    }
    fscanf(offsets, "%d\n", &totalMedium);
    mediumOffsets = (int*)calloc(totalMedium, sizeof(int));
    for (int i = 0; i < totalMedium; i++) {
        if (i == totalMedium-1) {
            fscanf(offsets, "%d\n", &mediumOffsets[i]);
        } else {
            fscanf(offsets, "%d ", &mediumOffsets[i]);
        }
    }
    fscanf(offsets, "%d\n", &totalHard);
    hardOffsets = (int*)calloc(totalHard, sizeof(int));
    for (int i = 0; i < totalHard; i++) {
        if (i == totalHard-1) {
            fscanf(offsets, "%d\n", &hardOffsets[i]);
        } else {
            fscanf(offsets, "%d ", &hardOffsets[i]);
        }
    }
    fclose(offsets);
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
        switch (easyMatrix[row-1][col-1].delete) {
            case -1:
                printf("\n\nOps, vc já selecionou este espaço! Digite qualquer tecla para continuar: ");
                getchar();
                getchar();
                break;
            case 0:
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
        switch (mediumMatrix[row-1][col-1].delete) {
            case -1:
                printf("\n\nOps, vc já selecionou este espaço! Digite qualquer tecla para continuar: ");
                getchar();
                getchar();
                break;
            case 0:
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
            default:
                printf("\n\nSelecione uma opcao valida! Aperte qualquer tecla para tentar novamente:  ");
                getchar();
                getchar();
                printf("\n");
                break;
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
        switch (hardMatrix[row-1][col-1].delete) {
            case -1:
                printf("\n\nOps, vc já selecionou este espaço! Digite qualquer tecla para continuar: ");
                getchar();
                getchar();
                break;
            case 0:
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
    }
}

void verifySuggestion(int choice) {
    int sum;
    char answer;
    switch (choice) {
        case 1:
            for (int i = 0; i < 4; i++) {
                sum = 0;
                for (int j = 0; j < 4; j++) {
                    if (easyMatrix[i][j].delete == 1) {
                        sum += easyMatrix[i][j].number;
                    }
                }
                if (sum != easyRows[i]) {
                    printf("\nA soma dos numeros da linha %d nao corresponde a resposta (%d). Tente novamente.\n", i+1, easyRows[i]);
                    printf("\nVoce deseja alterar o numero %d? (S/N): ", easyRows[i]);
                    scanf("\n%c", &answer);
                    printf("\n");
                    switch (answer) {
                        case 'S':
                            printf("\nDigite um novo valor para esse numero: ");
                            scanf("\n%d", &easyRows[i]);
                            verifySuggestion(1);
                            return;
                            break;
                        case 'N':
                            break;
                        default:
                            printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                            break;
                    }
                    for (int k = 0; k < 4; k++) {
                        printf("\nVoce deseja alterar o numero %d? (S/N): ", easyMatrix[i][k].number);
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
                                        scanf("\n%d", &easyMatrix[i][k].number);
                                        verifySuggestion(1);
                                        return;
                                        break;
                                    case 'D':
                                        printf("\nEste numero deve ser excluido? (S/N): ");
                                        scanf("\n%c", &answer);
                                        printf("\n");
                                        switch (answer) {
                                            case 'S':
                                                easyMatrix[i][k].delete = 0;
                                                verifySuggestion(1);
                                                break;
                                            case 'N':
                                                easyMatrix[i][k].delete = 1;
                                                 verifySuggestion(1);
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
            for (int i = 0; i < 4; i++) {
                sum = 0;
                for (int j = 0; j < 4; j++) {
                    if (easyMatrix[j][i].delete == 1) {
                        sum += easyMatrix[j][i].number;
                    }
                }
                if (sum != easyCols[i]) {
                    printf("\nA soma dos numeros da linha %d nao corresponde a resposta (%d). Tente novamente.\n", i+1, easyCols[i]);
                    printf("\nVoce deseja alterar o numero %d? (S/N): ", easyCols[i]);
                    scanf("\n%c", &answer);
                    printf("\n");
                    switch (answer) {
                        case 'S':
                            printf("\nDigite um novo valor para esse numero: ");
                            scanf("\n%d", &easyCols[i]);
                            verifySuggestion(1);
                            return;
                            break;
                        case 'N':
                            break;
                        default:
                            printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                            break;
                    }
                    for (int k = 0; k < 4; k++) {
                        printf("\nVoce deseja alterar o numero %d? (S/N): ", easyMatrix[k][i].number);
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
                                        scanf("\n%d", &easyMatrix[k][i].number);
                                        verifySuggestion(1);
                                        return;
                                        break;
                                    case 'D':
                                        printf("\nEste numero deve ser excluido? (S/N): ");
                                        scanf("\n%c", &answer);
                                        printf("\n");
                                        switch (answer) {
                                            case 'S':
                                                easyMatrix[k][i].delete = 0;
                                                verifySuggestion(1);
                                                break;
                                            case 'N':
                                                easyMatrix[k][i].delete = 1;
                                                verifySuggestion(1);
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
        case 2:
            for (int i = 0; i < 6; i++) {
                sum = 0;
                for (int j = 0; j < 6; j++) {
                    if (mediumMatrix[i][j].delete == 1) {
                        sum += mediumMatrix[i][j].number;
                    }
                }
                if (sum != mediumRows[i]) {
                    printf("\nA soma dos numeros da linha %d nao corresponde a resposta (%d). Tente novamente.\n", i+1, mediumRows[i]);
                    printf("\nVoce deseja alterar o numero %d? (S/N): ", mediumRows[i]);
                    scanf("\n%c", &answer);
                    printf("\n");
                    switch (answer) {
                        case 'S':
                            printf("\nDigite um novo valor para esse numero: ");
                            scanf("\n%d", &mediumRows[i]);
                            verifySuggestion(2);
                            return;
                            break;
                        case 'N':
                            break;
                        default:
                            printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                            break;
                    }
                    for (int k = 0; k < 6; k++) {
                        printf("\nVoce deseja alterar o numero %d? (S/N): ", mediumMatrix[i][k].number);
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
                                        scanf("\n%d", &mediumMatrix[i][k].number);
                                        verifySuggestion(2);
                                        return;
                                        break;
                                    case 'D':
                                        printf("\nEste numero deve ser excluido? (S/N): ");
                                        scanf("\n%c", &answer);
                                        printf("\n");
                                        switch (answer) {
                                            case 'S':
                                                mediumMatrix[i][k].delete = 0;
                                                verifySuggestion(2);
                                                break;
                                            case 'N':
                                                mediumMatrix[i][k].delete = 1;
                                                verifySuggestion(2);
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
            for (int i = 0; i < 6; i++) {
                sum = 0;
                for (int j = 0; j < 6; j++) {
                    if (mediumMatrix[j][i].delete == 1) {
                        sum += mediumMatrix[j][i].number;
                    }
                }
                if (sum != mediumCols[i]) {
                    printf("\nA soma dos numeros da linha %d nao corresponde a resposta (%d). Tente novamente.\n", i+1, mediumCols[i]);
                    printf("\nVoce deseja alterar o numero %d? (S/N): ", mediumCols[i]);
                    scanf("\n%c", &answer);
                    printf("\n");
                    switch (answer) {
                        case 'S':
                            printf("\nDigite um novo valor para esse numero: ");
                            scanf("\n%d", &mediumCols[i]);
                            verifySuggestion(2);
                            return;
                            break;
                        case 'N':
                            break;
                        default:
                            printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                            break;
                    }
                    for (int k = 0; k < 6; k++) {
                        printf("\nVoce deseja alterar o numero %d? (S/N): ", mediumMatrix[k][i].number);
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
                                        scanf("\n%d", &mediumMatrix[k][i].number);
                                        verifySuggestion(2);
                                        return;
                                        break;
                                    case 'D':
                                        printf("\nEste numero deve ser excluido? (S/N): ");
                                        scanf("\n%c", &answer);
                                        printf("\n");
                                        switch (answer) {
                                            case 'S':
                                                mediumMatrix[k][i].delete = 0;
                                                verifySuggestion(2);
                                                break;
                                            case 'N':
                                                mediumMatrix[k][i].delete = 1;
                                                verifySuggestion(2);
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
        case 3:
            for (int i = 0; i < 7; i++) {
                sum = 0;
                for (int j = 0; j < 7; j++) {
                    if (hardMatrix[i][j].delete == 1) {
                        sum += hardMatrix[i][j].number;
                    }
                }
                if (sum != hardRows[i]) {
                    printf("\nA soma dos numeros da linha %d nao corresponde a resposta (%d). Tente novamente.\n", i+1, hardRows[i]);
                    printf("\nVoce deseja alterar o numero %d? (S/N): ", hardRows[i]);
                    scanf("\n%c", &answer);
                    printf("\n");
                    switch (answer) {
                        case 'S':
                            printf("\nDigite um novo valor para esse numero: ");
                            scanf("\n%d", &hardRows[i]);
                            verifySuggestion(3);
                            return;
                            break;
                        case 'N':
                            break;
                        default:
                            printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                            break;
                    }
                    for (int k = 0; k < 7; k++) {
                        printf("\nVoce deseja alterar o numero %d? (S/N): ", hardMatrix[i][k].number);
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
                                        scanf("\n%d", &hardMatrix[i][k].number);
                                        verifySuggestion(3);
                                        return;
                                        break;
                                    case 'D':
                                        printf("\nEste numero deve ser excluido? (S/N): ");
                                        scanf("\n%c", &answer);
                                        printf("\n");
                                        switch (answer) {
                                            case 'S':
                                                hardMatrix[i][k].delete = 0;
                                                verifySuggestion(3);
                                                break;
                                            case 'N':
                                                hardMatrix[i][k].delete = 1;
                                                verifySuggestion(3);
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
            for (int i = 0; i < 7; i++) {
                sum = 0;
                for (int j = 0; j < 7; j++) {
                    if (hardMatrix[j][i].delete == 1) {
                        sum += hardMatrix[j][i].number;
                    }
                }
                if (sum != hardCols[i]) {
                    printf("\nA soma dos numeros da linha %d nao corresponde a resposta (%d). Tente novamente.\n", i+1, hardCols[i]);
                    printf("\nVoce deseja alterar o numero %d? (S/N): ", hardCols[i]);
                    scanf("\n%c", &answer);
                    printf("\n");
                    switch (answer) {
                        case 'S':
                            printf("\nDigite um novo valor para esse numero: ");
                            scanf("\n%d", &hardCols[i]);
                            verifySuggestion(3);
                            return;
                            break;
                        case 'N':
                            break;
                        default:
                            printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                            break;
                    }
                    for (int k = 0; k < 7; k++) {
                        printf("\nVoce deseja alterar o numero %d? (S/N): ", hardMatrix[k][i].number);
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
                                        scanf("\n%d", &hardMatrix[k][i].number);
                                        verifySuggestion(3);
                                        return;
                                        break;
                                    case 'D':
                                        printf("\nEste numero deve ser excluido? (S/N): ");
                                        scanf("\n%c", &answer);
                                        printf("\n");
                                        switch (answer) {
                                            case 'S':
                                                hardMatrix[k][i].delete = 0;
                                                verifySuggestion(3);
                                                break;
                                            case 'N':
                                                hardMatrix[k][i].delete = 1;
                                                verifySuggestion(3);
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

void suggestGame(int choice) {
    addingGame = 0;
    char answer;
    switch (choice) {
        case 1:
            for (int i = 0; i < 4; i++) {
                while (suggesting) {
                    printf("\nDigite a resposta da coluna %d: ", i+1);
                    scanf("\n%d", &easyCols[i]);
                    break;
                }
            }
            for (int i = 0; i < 4; i++) {
                while (suggesting) {
                    printf("\nDigite a resposta da linha %d: ", i+1);
                    scanf("\n%d", &easyRows[i]);
                    break;
                }
            }
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                   while (suggesting) {
                        printf("\nDigite o numero da linha %d e coluna %d: ", i+1, j+1);
                        scanf("\n%d", &easyMatrix[i][j].number);
                        printf("\n");
                        if (easyMatrix[i][j].number < 0 || easyMatrix[i][j].number > 9) {
                            printf("\nNumero invalido! Digite um numero entre 0 e 9, por favor.\n");
                        } else {
                            printf("\nEste numero deve ser excluido? (S/N): ");
                            scanf("\n%c", &answer);
                            printf("\n");
                            switch (answer) {
                                case 'S':
                                    easyMatrix[i][j].delete = 0;
                                    break;
                                case 'N':
                                    easyMatrix[i][j].delete = 1;
                                    break;
                                default:
                                    printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                                    break;
                            }
                            break;
                        }
                    }
                   }
                }
            verifySuggestion(1);
            while (addingGame) {
                FILE* easy = fopen("easy.txt", "r");
                fseek(easy, 0, SEEK_END);
                lastEasyOffset = ftell(easy);
                fclose(easy);
                easy = fopen("easy.txt", "a");
                fprintf(easy, "\n");
                for (int i = 0; i < 7; i++) {
                    if (i == 6) {
                        fprintf(easy, "%d\n", easyCols[i]);
                    } else {
                        fprintf(easy, "%d ", easyCols[i]);
                    }
                }
                for (int i = 0; i < 7; i++) {
                    if (i == 6) {
                        fprintf(easy, "%d\n", easyRows[i]);
                    } else {
                        fprintf(easy, "%d ", easyRows[i]);
                    }
                }
                for (int i = 0; i < 7; i++) {
                    for (int j = 0; j < 7; j++) {
                        if (j == 6) {
                            fprintf(easy, "%d\n", easyMatrix[i][j].number);
                        } else {
                            fprintf(easy, "%d ", easyMatrix[i][j].number);
                        }
                    }
                }
                for (int i = 0; i < 7; i++) {
                    for (int j = 0; j < 7; j++) {
                        if (j == 6) {
                            fprintf(easy, "%d\n", easyMatrix[i][j].delete);
                        } else {
                            fprintf(easy, "%d ", easyMatrix[i][j].delete);
                        }
                    }
                }
                fprintf(easy, "*");
                fclose(easy);
                FILE* offsets = fopen("offsets.txt", "w");
                totalEasy++;
                easyOffsets = (int*)realloc(easyOffsets, totalEasy * sizeof(int));
                easyOffsets[totalEasy-1] = lastEasyOffset;
                fprintf(offsets, "%d\n", totalEasy);
                for (int i = 0; i < totalEasy; i++) {
                    if (i == totalEasy-1) {
                        fprintf(offsets, "%d\n", easyOffsets[i]);
                    } else {
                        fprintf(offsets, "%d ", easyOffsets[i]);
                    }
                }
                fprintf(offsets, "%d\n", totalMedium);
                for (int i = 0; i < totalMedium; i++) {
                    if (i == totalMedium-1) {
                        fprintf(offsets, "%d\n", mediumOffsets[i]);
                    } else {
                        fprintf(offsets, "%d ", mediumOffsets[i]);
                    }
                }
                fprintf(offsets, "%d\n", totalHard);
                for (int i = 0; i < totalHard; i++) {
                    if (i == totalHard-1) {
                        fprintf(offsets, "%d\n", hardOffsets[i]);
                    } else {
                        fprintf(offsets, "%d ", hardOffsets[i]);
                    }
                }
                fclose(offsets);
                addingGame = 0;
                break;
            }
            break;
        case 2:
            for (int i = 0; i < 6; i++) {
                while (suggesting) {
                    printf("\nDigite a resposta da coluna %d: ", i+1);
                    scanf("\n%d", &mediumCols[i]);
                    break;
                }
            }
            for (int i = 0; i < 6; i++) {
                while (suggesting) {
                    printf("\nDigite a resposta da linha %d: ", i+1);
                    scanf("\n%d", &mediumRows[i]);
                    break;
                }
            }
            for (int i = 0; i < 6; i++) {
                for (int j = 0; j < 6; j++) {
                   while (suggesting) {
                        printf("\nDigite o numero da linha %d e coluna %d: ", i+1, j+1);
                        scanf("\n%d", &mediumMatrix[i][j].number);
                        printf("\n");
                        if (mediumMatrix[i][j].number < 0 || mediumMatrix[i][j].number > 9) {
                            printf("\nNumero invalido! Digite um numero entre 0 e 9, por favor.\n");
                        } else {
                            printf("\nEste numero deve ser excluido? (S/N): ");
                            scanf("\n%c", &answer);
                            printf("\n");
                            switch (answer) {
                                case 'S':
                                    mediumMatrix[i][j].delete = 0;
                                    break;
                                case 'N':
                                    mediumMatrix[i][j].delete = 1;
                                    break;
                                default:
                                    printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                                    break;
                            }
                            break;
                        }
                    }
                   }
                }
            verifySuggestion(2);
            while (addingGame) {
                FILE* medium = fopen("medium.txt", "r");
                fseek(medium, 0, SEEK_END);
                lastMediumOffset = ftell(medium);
                fclose(medium);
                medium = fopen("medium.txt", "a");
                fprintf(medium, "\n");
                for (int i = 0; i < 7; i++) {
                    if (i == 6) {
                        fprintf(medium, "%d\n", mediumCols[i]);
                    } else {
                        fprintf(medium, "%d ", mediumCols[i]);
                    }
                }
                for (int i = 0; i < 7; i++) {
                    if (i == 6) {
                        fprintf(medium, "%d\n", mediumRows[i]);
                    } else {
                        fprintf(medium, "%d ", mediumRows[i]);
                    }
                }
                for (int i = 0; i < 7; i++) {
                    for (int j = 0; j < 7; j++) {
                        if (j == 6) {
                            fprintf(medium, "%d\n", mediumMatrix[i][j].number);
                        } else {
                            fprintf(medium, "%d ", mediumMatrix[i][j].number);
                        }
                    }
                }
                for (int i = 0; i < 7; i++) {
                    for (int j = 0; j < 7; j++) {
                        if (j == 6) {
                            fprintf(medium, "%d\n", mediumMatrix[i][j].delete);
                        } else {
                            fprintf(medium, "%d ", mediumMatrix[i][j].delete);
                        }
                    }
                }
                fprintf(medium, "*");
                fclose(medium);
                FILE* offsets = fopen("offsets.txt", "w");
                totalMedium++;
                mediumOffsets = (int*)realloc(mediumOffsets, totalMedium * sizeof(int));
                mediumOffsets[totalMedium-1] = lastMediumOffset;
                fprintf(offsets, "%d\n", totalEasy);
                for (int i = 0; i < totalEasy; i++) {
                    if (i == totalEasy-1) {
                        fprintf(offsets, "%d\n", easyOffsets[i]);
                    } else {
                        fprintf(offsets, "%d ", easyOffsets[i]);
                    }
                }
                fprintf(offsets, "%d\n", totalMedium);
                for (int i = 0; i < totalMedium; i++) {
                    if (i == totalMedium-1) {
                        fprintf(offsets, "%d\n", mediumOffsets[i]);
                    } else {
                        fprintf(offsets, "%d ", mediumOffsets[i]);
                    }
                }
                fprintf(offsets, "%d\n", totalHard);
                for (int i = 0; i < totalHard; i++) {
                    if (i == totalHard-1) {
                        fprintf(offsets, "%d\n", hardOffsets[i]);
                    } else {
                        fprintf(offsets, "%d ", hardOffsets[i]);
                    }
                }
                fclose(offsets);
                addingGame = 0;
                break;
            }
            break;
        case 3:
          for (int i = 0; i < 7; i++) {
                while (suggesting) {
                    printf("\nDigite a resposta da coluna %d: ", i+1);
                    scanf("\n%d", &hardCols[i]);
                    break;
                }
            }
            for (int i = 0; i < 7; i++) {
                while (suggesting) {
                    printf("\nDigite a resposta da linha %d: ", i+1);
                    scanf("\n%d", &hardRows[i]);
                    break;
                }
            }
            for (int i = 0; i < 7; i++) {
                for (int j = 0; j < 7; j++) {
                   while (suggesting) {
                        printf("\nDigite o numero da linha %d e coluna %d: ", i+1, j+1);
                        scanf("\n%d", &hardMatrix[i][j].number);
                        printf("\n");
                        if (hardMatrix[i][j].number < 0 || hardMatrix[i][j].number > 9) {
                            printf("\nNumero invalido! Digite um numero entre 0 e 9, por favor.\n");
                        } else {
                            printf("\nEste numero deve ser excluido? (S/N): ");
                            scanf("\n%c", &answer);
                            printf("\n");
                            switch (answer) {
                                case 'S':
                                    hardMatrix[i][j].delete = 0;
                                    break;
                                case 'N':
                                    hardMatrix[i][j].delete = 1;
                                    break;
                                default:
                                    printf("\nOpcao invalida! Digite S para sim ou N para nao.\n");
                                    break;
                            }
                            break;
                        }
                    }
                   }
                }
            verifySuggestion(3);
            while (addingGame) {
                FILE* hard = fopen("hard.txt", "r");
                fseek(hard, 0, SEEK_END);
                lastHardOffset = ftell(hard);
                fclose(hard);
                hard = fopen("hard.txt", "a");
                fprintf(hard, "\n");
                for (int i = 0; i < 7; i++) {
                    if (i == 6) {
                        fprintf(hard, "%d\n", hardCols[i]);
                    } else {
                        fprintf(hard, "%d ", hardCols[i]);
                    }
                }
                for (int i = 0; i < 7; i++) {
                    if (i == 6) {
                        fprintf(hard, "%d\n", hardRows[i]);
                    } else {
                        fprintf(hard, "%d ", hardRows[i]);
                    }
                }
                for (int i = 0; i < 7; i++) {
                    for (int j = 0; j < 7; j++) {
                        if (j == 6) {
                            fprintf(hard, "%d\n", hardMatrix[i][j].number);
                        } else {
                            fprintf(hard, "%d ", hardMatrix[i][j].number);
                        }
                    }
                }
                for (int i = 0; i < 7; i++) {
                    for (int j = 0; j < 7; j++) {
                        if (j == 6) {
                            fprintf(hard, "%d\n", hardMatrix[i][j].delete);
                        } else {
                            fprintf(hard, "%d ", hardMatrix[i][j].delete);
                        }
                    }
                }
                fprintf(hard, "*");
                fclose(hard);
                FILE* offsets = fopen("offsets.txt", "w");
                totalHard++;
                hardOffsets = (int*)realloc(hardOffsets, totalHard * sizeof(int));
                hardOffsets[totalHard-1] = lastHardOffset;
                fprintf(offsets, "%d\n", totalEasy);
                for (int i = 0; i < totalEasy; i++) {
                    if (i == totalEasy-1) {
                        fprintf(offsets, "%d\n", easyOffsets[i]);
                    } else {
                        fprintf(offsets, "%d ", easyOffsets[i]);
                    }
                }
                fprintf(offsets, "%d\n", totalMedium);
                for (int i = 0; i < totalMedium; i++) {
                    if (i == totalMedium-1) {
                        fprintf(offsets, "%d\n", mediumOffsets[i]);
                    } else {
                        fprintf(offsets, "%d ", mediumOffsets[i]);
                    }
                }
                fprintf(offsets, "%d\n", totalHard);
                for (int i = 0; i < totalHard; i++) {
                    if (i == totalHard-1) {
                        fprintf(offsets, "%d\n", hardOffsets[i]);
                    } else {
                        fprintf(offsets, "%d ", hardOffsets[i]);
                    }
                }
                fclose(offsets);
                addingGame = 0;
                break;
            }
            break;  
    }
    printf("\nNova fase adicionada com sucesso!\n");
    suggesting = 0;
    menu = 1;
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
                            suggesting = 1;
                            while (suggesting) {
                                printSuggestLevel();
                                scanf("\n%c", &menuOption);
                                switch(menuOption) {
                                    case '1':
                                        suggestGame(1);
                                        break;
                                    case '2':
                                        suggestGame(2);
                                        break;
                                    case '3':
                                        suggestGame(3);
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
}