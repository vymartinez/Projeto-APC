//Nome: Victor Yan Martinez
//Matrícula: 241032994

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
int menu, playing, settings, suggesting, ranking, chooseDificulty, played, addingGame, block, instructions, answering;
//VARIABLES
int correct, lastTurn, lives, random, row, col, lastOffset, deletes, len;
int colsSum[7], rowsSum[7];//somas das linhas e colunas
int totals[3];//numero de fases de cada dificuldade
int *easyOffsets, *mediumOffsets, *hardOffsets;//inicio de cada uma das fases
int solved[3];//numero de fases resolvidas de cada dificuldade
int traces[3] = {22, 32, 37};//quantidade de traços da matriz de acordo com a dificuldade
int available[3];//dificuldades disponiveis
matrix mainMatrix[7][7];
int dificulty = 1;
char modes[3][10] = {"Facil", "Medio", "Dificil"};
players player;
char answer[100];//respostas de inputs dos jogadores

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

void setupGame() {//Lê os offsets dos arquivos e armazenar em um vetor
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
    available[0] = available[1] = available[2] = 1;
    solved[0] = solved[1] = solved[2] = 0;
}

void verifyEndgame() {//verifica se zerou todas as fases, se não só troca de dificuldade
    if (solved[0] == totals[0] && solved[1] == totals[1] && solved[2] == totals[2]) {
        printf("\n");
        printf("#   #  #####  #####  #####       #   #  #####  #   # ##### ##### #   #    #\n");
        printf("#   #  #   #  #      #           #   #  #      ##  # #     #     #   #    #\n");
        printf("#   #  #   #  #      #####       #   #  #####  # # # #     ##### #   #    #\n");
        printf(" # #   #   #  #      #            # #   #      #  ## #     #     #   #     \n");
        printf("  #    #####  #####  #####         #    #####  #   # ##### ##### #####    #\n");
        printf("\nParabens! Voce completou todos os niveis! Voce ZEROU o jogo!!! Aperte [Enter] para encerrar o jogo: \n");
        fgets(answer, 100, stdin);
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

void adaptPlayerName(int spaces, int length) {
    for (int i = spaces; i < length; i++) {
        player.name[i-spaces] = player.name[i];
    }
}

void EnterNickname() {//armazena o novo player e verifica se já jogou antes para puxar dados do ranking
    printf("\nSeja bem-vindo(a) ao meu Number Sums! Vamos comecar!!\n");
    printf("\nDigite seu nome(ate 20 caracteres): ");
    fgets(player.name, 25, stdin);
    len = strlen(player.name);
    player.name[len-1] = '\0';
    if (len > 20) {
        clear();
        printf("\nNome invalido! Digite um nome valido, por favor.\n");
        EnterNickname();
        return;
    }
    int isSpace = 1;
    int spaces = 0;
    for (int i = 0; i < len; i++) {
        if (player.name[i] != ' ' && player.name[i] != '\0') {
            isSpace = 0;
        }
        if (isSpace == 1) {
            spaces++;
        }
        if (!isalpha(player.name[i]) && player.name[i] != ' ' && player.name[i] != '\0') {
            clear();
            printf("\nNome invalido! Digite um nome valido, por favor.\n");
            EnterNickname();
            return;
        }
    }
    if (isSpace == 1) {
        clear();
        printf("\nNome invalido! Digite um nome valido, por favor.\n");
        EnterNickname();
        return;
    }
    if (spaces != 0) {
        adaptPlayerName(spaces, len);
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
        fread(&played, sizeof(int), 1, file);
        players allPlayers[played];
        for (int i = 0; i < played; i++) {
            fread(&allPlayers[i], sizeof(players), 1, file);
            if (strcmp(allPlayers[i].name, player.name) == 0) {
                fclose(file);
                continuation = 1;
                while (continuation) {
                    printf("\no usuario '%s' ja existe. Deseja continuar? (S/N): ", player.name);
                    fgets(answer, 100, stdin);
                    len = strlen(answer);
                    if ((answer[0] == 'S' || answer[0] == 's') && len == 2) {
                        continuation = 0;
                        menu = 1;
                        player.score = allPlayers[i].score;
                        return;
                    } else if ((answer[0] == 'N' || answer[0] == 'n') && len == 2) {
                        clear();
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

void resetRanking() {//reseta o ranking
    player.score = 0;
    played = 1;
    FILE *file = fopen("ranking.bin","wb");
    fwrite(&played, sizeof(int), 1, file);
    fwrite(&player, sizeof(players), 1, file);
    fclose(file);
    printf("\n\nRanking reiniciado com sucesso! Pressione [Enter] para continuar: ");
    fgets(answer, 100, stdin);
    printf("\n");
    setupGame();
    return;
}

void printMenu() {//printa o menu na tela
    printf("\n");
    printf("        Number Sums - Made by: Victor Yan        \n");
    printf("################################################\n\n");
    printf("[1] - Jogar [Modo %s]\n", modes[dificulty-1]);
    printf("[2] - Configuracoes\n");
    printf("[3] - Ranking\n");
    printf("[4] - Instrucoes\n");
    printf("[5] - Sair\n\n");
    printf("################################################\n\n");
    printf("Escolha uma opcao: ");
    return;
}

void printConfig() {//printa as opções de configuração na tela
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

void printDificulty() {//printa as opções de dificuldade na tela
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

void printSuggestLevel() {//printa as opções de sugestão de nível na tela
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

void printRanking() {//printa o ranking na tela
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
            printf("%d. %s", i, rankingPlayer.name);
            int length = strlen(rankingPlayer.name);
            for (int i = 0; i < 30-length; i++) {
                printf(" ");
            }
            printf("PONTOS: %7d\n", rankingPlayer.score);
        }
        fclose(file);
        printf("\n################################################\n\n");
    }
    printf("Pressione [Enter] para voltar ao menu: ");
    return;
}

void printInstructions() {//printa as instruções na tela
    while (instructions) {
        clear();
        printf("\n");
        printf("                          Instrucoes                      \n");
        printf("###############################################################\n\n");
        printf("OBJETIVO: remover numeros da grade para que restem numeros\n");
        printf("que, em sua respectiva linha e coluna, sejam parte da soma\n");
        printf("que resulta no seu representante de linha e coluna\n\n");
        printf("Aperte [Enter] para continuar ou 'E' para sair: ");
        fgets(answer, 100, stdin);
        len = strlen(answer);
        switch (answer[0]) {
            case 'e' | 'E':
                if (len == 2) {
                    instructions = 0;
                    menu = 1;
                    return;
                } else {
                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                    fgets(answer, 100, stdin);
                }
                break;
            default:
                printf("\n\nOpcao invalida. Selecione uma opcao valida, por favor.\n");
                break;
        }
        clear();
        printf("\n");
        printf("                          Instrucoes                      \n");
        printf("###############################################################\n\n");
        printf("COMO JOGAR: ao receber a grade, selecione a linha e a coluna,\n");
        printf("respectivamente, do numero que voce deseja eliminar.\n");
        printf("Ao acertar a resposta ou fechar uma linha ou coluna com as somas\n");
        printf("corretas, os numeros desaparecerao. Voce possui 5 vidas e,\n");
        printf("ao perde-las, voce perde o jogo.\n\n");
        printf("Aperte [Enter] para continuar ou 'E' para sair: ");
        fgets(answer, 100, stdin);
        len = strlen(answer);
        switch (answer[0]) {
            case 'e' | 'E':
                if (len == 2) {
                    instructions = 0;
                    menu = 1;
                    return;
                } else {
                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                    fgets(answer, 100, stdin);
                }
                break;
            default:
                printf("\n\nOpcao invalida. Selecione uma opcao valida, por favor.\n");
                break;
        }
        clear();
        printf("\n");
        printf("                          Instrucoes                      \n");
        printf("###############################################################\n\n");
        printf("PONTUACAO: Ao resolver uma fase facil, voce recebe 100 pontos.\n");
        printf("Ao resolver uma fase media, voce recebe 200 pontos.\n");
        printf("Ao resolver uma fase dificil, voce recebe 300 pontos.\n");
        printf("Ao resolver todas as fases, voce recebe um bonus de 500 pontos.\n");
        int verification = 1;
        while (verification) {
            printf("\nEsse eh o fim do tutorial. Deseja ver novamente? (S/N): ");
            fgets(answer, 100, stdin);
            len = strlen(answer);
            switch (answer[0]) {
                case 'S' | 's':
                    if (len == 2) {
                        verification = 0;
                    } else {
                        printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                        fgets(answer, 100, stdin);
                    }
                    break;
                case 'N' | 'n':
                    if (len == 2) {
                        verification = 0;
                        instructions = 0;
                        menu = 1;
                        return;
                    } else {
                        printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                        fgets(answer, 100, stdin);
                    }
                    break;
                default:
                    clear();
                    printf("\n\nOpcao invalida. Selecione uma opcao valida, por favor.\n");
                    break;
            }
        }
    }
    return;
}

void verifyChoice(int line, int col, int lineSum, int colSum) {//verifica se a escolha do jogador fechou coluna ou linha
    if (rowsSum[line-1] == lineSum) {
        printf("\nMuito bem! Voce fechou a linha %d! Aperte [Enter] para continuar: ", line);
        fgets(answer, 100, stdin);
        rowsSum[line-1] = -1;
    }
    if (colsSum[col-1] == colSum) {
        printf("\nMuito bem! Voce fechou a coluna %d! Aperte [Enter] para continuar: ", col);
        fgets(answer, 100, stdin);
        colsSum[col-1] = -1;
    }
    return;
}

void playGame(int size) {//printa a matriz atualizada na tela e verifica se o jogador acertou ou errou
    while (playing) {
        lastTurn = correct;
        printf("\n X | ");
        for (int i = 0; i < size; i++) {
            if (i == size-1) {
                if (colsSum[i] == -1) {
                    printf("   \n");
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
        scanf("%d %d", &row, &col);
        if (row < 1 || row > size || col < 1 || col > size) {
            printf("\n\nSelecione uma opcao valida! Aperte [Enter] para tentar novamente:  ");
            fgets(answer, 100, stdin);
            fgets(answer, 100, stdin);
        } else {
            switch (mainMatrix[row-1][col-1].delete) {
                case -1:
                    printf("\n\nOps, vc ja selecionou este espaco! Aperte [Enter] para continuar: ");
                    fgets(answer, 100, stdin);
                    fgets(answer, 100, stdin);
                    break;
                case 0:
                    mainMatrix[row-1][col-1].delete = -1;
                    correct++;
                    if (correct == deletes) {
                        printf("\n\nVoce ganhou! Parabens! Aperte [Enter] para continuar: ");
                        fgets(answer, 100, stdin);
                        fgets(answer, 100, stdin);
                        playing = 0;
                        return;
                    }
                    if (correct != lastTurn) {
                        printf("\n\nVoce acertou, parabens! Aperte [Enter] para continuar.\n");
                        fgets(answer, 100, stdin);
                        fgets(answer, 100, stdin);
                    }
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
                    verifyChoice(row, col, lineSum, colSum);
                    break;
                case 1:
                    if (correct == lastTurn) {
                        lives--;
                        printf("\n\nVoce errou. Ainda restam %d vidas! Aperte [Enter] para continuar: ", lives);
                        fgets(answer, 100, stdin);
                        fgets(answer, 100, stdin);
                        printf("\n");
                    }
                    if (lives == 0) {
                        printf("\n\nVoce perdeu! Aperte [Enter] para continuar: ");
                        fgets(answer, 100, stdin);
                        playing = 0;
                    }
                    break;
                default:
                    printf("\n\nSelecione uma opcao valida! Aperte [Enter] para tentar novamente:  ");
                    fgets(answer, 100, stdin);
                    printf("\n");
                    break;
            }
        }
        clear();
    }
}

void verifySuggestion(int choice, int size) {//verifica se a sugestão do jogador não possui erros (linha primeiro, coluna depois)
    int sum;
    int nums[size];
    int total = 0;
    for (int i = 0; i < size; i++) {
        sum = 0;
        for (int j = 0; j < size; j++) {
            if (mainMatrix[i][j].delete == 1) {
                sum += mainMatrix[i][j].number;
                nums[total] = mainMatrix[i][j].number;
                total++;
            }
        }
        if (sum != rowsSum[i]) {
            printf("\nA soma dos numeros da linha %d nao corresponde a resposta (%d). Tente novamente.\n", i+1, rowsSum[i]);
            printf("\nSoma: ");
            for (int k = 0; k < total; k++) {
                if (k == total-1) {
                    printf("%d = %d\n", nums[k], rowsSum[i]);
                } else {
                    printf("%d + ", nums[k]);
                }
            }
            answering = 1;
            while (answering) {
                printf("\nVoce deseja alterar o numero %d? (S/N): ", rowsSum[i]);
                fgets(answer, 100, stdin);
                len = strlen(answer);
                printf("\n");
                switch (answer[0]) {
                    case 'S' | 's':
                        if (len == 2) {
                            printf("\nDigite um novo valor para esse numero: ");
                            scanf("%d", &rowsSum[i]);
                            fgets(answer, 100, stdin);//limpa o buffer
                            verifySuggestion(choice, size);
                            return;
                        } else {
                            printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                            fgets(answer, 100, stdin);
                        }
                        break;
                    case 'N' | 'n':
                        if (len == 2) {
                            answering = 0;
                        } else {
                            printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                            fgets(answer, 100, stdin);
                        }
                        break;
                    default:
                        printf("\nOpcao invalida! Digite S para sim ou N para nao. Aperte [Enter] para continuar: \n");
                        fgets(answer, 100, stdin);
                        break;
                }
                clear();
            }
            for (int k = 0; k < size; k++) {
                answering = 1;
                while (answering) {
                    printf("\nVoce deseja alterar o numero %d? (S/N): ", mainMatrix[i][k].number);
                    fgets(answer, 100, stdin);
                    len = strlen(answer);
                    printf("\n");
                    switch (answer[0]) {
                        case 'S' | 's':
                            if (len == 2) {
                                while (answering) {
                                    printf("\nVoce deseja alterar o valor ou se deve, ou nao, ser deletado? (V/D): ");
                                    fgets(answer, 100, stdin);
                                    len = strlen(answer);
                                    printf("\n");
                                    switch (answer[0]) {
                                        case 'V' | 'v':
                                            if (len == 2) {
                                                printf("\nDigite um novo valor para esse numero: ");
                                                scanf("%d", &mainMatrix[i][k].number);
                                                verifySuggestion(choice, size);
                                                return;
                                            } else {
                                                printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                                fgets(answer, 100, stdin);
                                            }
                                            break;
                                        case 'D' | 'd':
                                            if (len == 2) {
                                                while (answering) {
                                                    printf("\nEste numero deve ser excluido? (S/N): ");
                                                    fgets(answer, 100, stdin);
                                                    len = strlen(answer);
                                                    printf("\n");
                                                    switch (answer[0]) {
                                                        case 'S' | 's':
                                                            if (len == 2) {
                                                                mainMatrix[i][k].delete = 0;
                                                                verifySuggestion(choice, size);
                                                                return;
                                                            } else {
                                                                printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                                                fgets(answer, 100, stdin);
                                                            }
                                                            break;
                                                        case 'N' | 'n':
                                                            if (len == 2) {
                                                                mainMatrix[i][k].delete = 1;
                                                                verifySuggestion(choice, size);
                                                                return;
                                                            } else {
                                                                printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                                                fgets(answer, 100, stdin);
                                                            }
                                                            break;
                                                        default:
                                                            printf("\nOpcao invalida! Digite S para sim ou N para nao. Aperte [Enter] para continuar: \n");
                                                            fgets(answer, 100, stdin);
                                                            break;
                                                    }
                                                    clear();
                                                }
                                            } else {
                                                printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                                fgets(answer, 100, stdin);
                                            }
                                            break;
                                        default:
                                            printf("\nOpcao invalida! Digite V para alterar o valor ou D para deletar o numero. Aperte [Enter] para continuar: \n");
                                            fgets(answer, 100, stdin);
                                            break;
                                    }
                                    clear();
                                }
                                break;
                            case 'N' | 'n':
                                if (len == 2) {
                                    answering = 0;
                                } else {
                                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                    fgets(answer, 100, stdin);
                                }
                                break;
                            default:
                                printf("\nOpcao invalida! Digite S para sim ou N para nao. Aperte [Enter] para continuar: \n");
                                fgets(answer, 100, stdin);
                                break;
                        } else {
                            printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                            fgets(answer, 100, stdin);
                        }
                    }
                    clear();
                }
            }
        }
    }
    for (int i = 0; i < size; i++) {
        sum = 0;
        int nums[size];
        int total = 0;
        for (int j = 0; j < size; j++) {
            if (mainMatrix[j][i].delete == 1) {
                sum += mainMatrix[j][i].number;
                nums[total] = mainMatrix[j][i].number;
                total++;
            }
        }
        if (sum != colsSum[i]) {
            printf("\nA soma dos numeros da linha %d nao corresponde a resposta (%d). Tente novamente.\n", i+1, colsSum[i]);
            printf("\nSoma: ");
            for (int k = 0; k < total; k++) {
                if (k == total-1) {
                    printf("%d = %d\n", nums[k], colsSum[i]);
                } else {
                    printf("%d + ", nums[k]);
                }
            }
            answering = 1;
            while (answering) {
                printf("\nVoce deseja alterar o numero %d? (S/N): ", colsSum[i]);
                fgets(answer, 100, stdin);
                len = strlen(answer);
                printf("\n");
                switch (answer[0]) {
                    case 'S' | 's':
                        if (len == 2) {
                            printf("\nDigite um novo valor para esse numero: ");
                            scanf("%d", &colsSum[i]);
                            fgets(answer, 100, stdin);
                            verifySuggestion(choice, size);
                            return;
                        } else {
                            printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                            fgets(answer, 100, stdin);
                        }
                        break;
                    case 'N' | 'n':
                        if (len == 2) {
                            answering = 0;
                        } else {
                            printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                            fgets(answer, 100, stdin);
                        }
                        break;
                    default:
                        printf("\nOpcao invalida! Digite S para sim ou N para nao. Aperte [Enter] para continuar: \n");
                        fgets(answer, 100, stdin);
                        break;
                }
                clear();
            }
            for (int k = 0; k < size; k++) {
                answering = 1;
                while (answering) {
                    printf("\nVoce deseja alterar o numero %d? (S/N): ", mainMatrix[i][k].number);
                    fgets(answer, 100, stdin);
                    len = strlen(answer);
                    printf("\n");
                    switch (answer[0]) {
                        case 'S' | 's':
                            if (len == 2) {
                                while (answering) {
                                    printf("\nVoce deseja alterar o valor ou se deve, ou nao, ser deletado? (V/D): ");
                                    fgets(answer, 100, stdin);
                                    len = strlen(answer);
                                    printf("\n");
                                    switch (answer[0]) {
                                        case 'V' | 'v':
                                            if (len == 2) {
                                                printf("\nDigite um novo valor para esse numero: ");
                                                scanf("%d", &mainMatrix[i][k].number);
                                                fgets(answer, 100, stdin);
                                                verifySuggestion(choice, size);
                                                return;
                                            } else {
                                                printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                                fgets(answer, 100, stdin);
                                            }
                                            break;
                                        case 'D' | 'd':
                                            if (len == 2) {
                                                while (answering) {
                                                    printf("\nEste numero (%d) deve ser excluido? (S/N): ", mainMatrix[i][k].number);
                                                    fgets(answer, 100, stdin);
                                                    len = strlen(answer);
                                                    printf("\n");
                                                    switch (answer[0]) {
                                                        case 'S' | 's':
                                                            if (len == 2) {
                                                                mainMatrix[i][k].delete = 0;
                                                                verifySuggestion(choice, size);
                                                                return;
                                                            } else {
                                                                printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                                                fgets(answer, 100, stdin);
                                                            }
                                                            break;
                                                        case 'N' | 'n':
                                                            if (len == 2) {
                                                                mainMatrix[i][k].delete = 1;
                                                                verifySuggestion(choice, size);
                                                                return;
                                                            } else {
                                                                printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                                                fgets(answer, 100, stdin);
                                                            }
                                                            break;
                                                        default:
                                                            printf("\nOpcao invalida! Digite S para sim ou N para nao. Aperte [Enter] para continuar: \n");
                                                            fgets(answer, 100, stdin);
                                                            break;
                                                    }
                                                    clear();
                                                }
                                            } else {
                                                printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                                fgets(answer, 100, stdin);
                                            }
                                        default:
                                            printf("\nOpcao invalida! Digite V para alterar o valor ou D para deletar o numero Aperte [Enter] para continuar: \n");
                                            fgets(answer, 100, stdin);
                                            break;
                                    }
                                    clear();
                                }
                                break;
                            case 'N' | 'n':
                                if (len == 2) {
                                    answering = 0;
                                } else {
                                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                    fgets(answer, 100, stdin);
                                }
                                break;
                            default:
                                printf("\nOpcao invalida! Digite S para sim ou N para nao. Aperte [Enter] para continuar: \n");
                                fgets(answer, 100, stdin);
                                break;
                        } else {
                            printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                            fgets(answer, 100, stdin);
                        }
                    }
                    clear();
                }
            }
        }
    }
    addingGame = 1;
    return;
}

void suggestGame(int choice, int size) {
    memset(mainMatrix, -1, sizeof(mainMatrix));
    memset(colsSum, -1, sizeof(colsSum));
    memset(rowsSum, -1, sizeof(rowsSum));
    addingGame = 0;
    for (int i = 0; i < size; i++) {
        while (suggesting) {    
            printf("\nDigite a resposta da coluna %d: ", i+1);
            scanf("%d", &colsSum[i]);
            break;
        }
        clear();
    }
    for (int i = 0; i < size; i++) {
        while (suggesting) {
            printf("\nDigite a resposta da linha %d: ", i+1);
            scanf("%d", &rowsSum[i]);
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
                            printf("   \n");
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
                for (int i = 0; i < traces[choice-1]; i++) {
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
                            if (mainMatrix[i][j].delete == -1 || mainMatrix[i][j].delete == 0) {
                                printf("  \n");
                            } else {
                                printf("%2d\n", mainMatrix[i][j].number);
                            }
                        } else {
                            if (mainMatrix[i][j].delete == -1 || mainMatrix[i][j].delete == 0) {
                                printf("     ");
                            } else {
                                printf("%2d   ", mainMatrix[i][j].number);
                            }
                        }
                    }
                }
                printf("\nDigite o numero da linha %d e coluna %d: ", i+1, j+1);
                scanf("%d", &mainMatrix[i][j].number);
                fgets(answer, 100, stdin);
                printf("\n");
                if (mainMatrix[i][j].number < 0 || mainMatrix[i][j].number > 9) {
                    printf("\nNumero invalido! Digite um numero entre 0 e 9, por favor. Aperte [Enter] para continuar: \n");
                    fgets(answer, 100, stdin);
                } else {
                    answering = 1;
                    while (answering) {
                        printf("\nEste numero (%d) deve ser excluido? (S/N): ", mainMatrix[i][j].number);
                        fgets(answer, 100, stdin);
                        len = strlen(answer);
                        printf("\n");
                        switch (answer[0]) {
                            case 'S' | 's':
                                if (len == 2) {
                                    mainMatrix[i][j].delete = 0;
                                    answering = 0;
                                } else {
                                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                    fgets(answer, 100, stdin);
                                }
                                break;
                            case 'N' | 'n':
                                if (len == 2) {
                                    mainMatrix[i][j].delete = 1;
                                    answering = 0;
                                } else {
                                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                    fgets(answer, 100, stdin);
                                }
                                break;
                            default:
                                printf("\nOpcao invalida! Digite S para sim ou N para nao. Aperte [Enter] para continuar: \n");
                                fgets(answer, 100, stdin);
                                break;
                        }
                        clear();
                    }
                    break;
                }
                clear();
            }
        }
    }
    verifySuggestion(choice, size);
    while (addingGame) {//adiciona a nova fase aos arquivos
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
        //adiciona o offset da nova fase ao arquivo de offsets
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
    printf("\nNova fase adicionada com sucesso! Aperte enter para continuar: \n");
    fgets(answer, 100, stdin);
    fgets(answer, 100, stdin);
    suggesting = 0;
    menu = 1;
}

void generateGame(int size) {//pega os dados da matriz do jogo e sorteia a fase
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
    if (lives != 0) {//verifica se o jogador ganhou e atualiza o score e o ranking
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
        switch(dificulty) {//verifica se o jogador completou todas as fases da dificuldade que jogou
            case 1:
                if (solved[dificulty-1] == totals[0]) {
                    available[dificulty-1] = 0;
                    printf("\nParabens! Voce completou todas as fases faceis! Aperte [Enter] para continuar: \n");
                    fgets(answer, 100, stdin);
                    verifyEndgame();
                } else {
                    for (int i = 0; i < totals[0]; i++) {
                        if (easyOffsets[i] != easyOffsets[random]) {
                            newRandom = i;
                            break;
                        }
                    }
                    easyOffsets[random] = easyOffsets[newRandom];
                }
                break;
            case 2:
                if (solved[dificulty-1] == totals[1]) {
                    available[dificulty-1] = 0;
                    printf("\nParabens! Voce completou todas as fases medias! Aperte [Enter] para continuar: \n");
                    fgets(answer, 100, stdin);
                    verifyEndgame();
                } else {
                    for (int i = 0; i < totals[1]; i++) {
                        if (mediumOffsets[i] != mediumOffsets[random]) {
                            newRandom = i;
                            break;
                        }
                    }
                    mediumOffsets[random] = mediumOffsets[newRandom];
                }
                break;
            case 3:
                if (solved[dificulty-1] == totals[2]) {
                    available[dificulty-1] = 0;
                    printf("\nParabens! Voce completou todas as fases dificeis! Aperte [Enter] para continuar: \n");
                    fgets(answer, 100, stdin);
                    verifyEndgame();
                } else {
                    for (int i = 0; i < totals[2]; i++) {
                        if (hardOffsets[i] != hardOffsets[random]) {
                            newRandom = i;
                            break;
                        }
                    }
                    hardOffsets[random] = hardOffsets[newRandom];
                }
                break;
        }
    }
}

int main() {
    menu = settings = chooseDificulty = ranking = played = block = 0;
    setupGame();
    EnterNickname();
    while (menu && !block) {//block é uma variável que impede que o jogador volte ao menu principal após zerar todas as fases
        clear();
        lives = 5;
        printMenu();
        fgets(answer, 100, stdin);
        len = strlen(answer);
        switch(answer[0]) {
            case '1'://Jogar
            if (len == 2) {
                clear();
                Play();
            } else {
                printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                fgets(answer, 100, stdin);
            }
            break;
            case '2'://Menu de configurações
                if (len == 2) {
                    clear();
                    settings = 1;
                    while (settings) {
                        clear();
                        printConfig();
                        fgets(answer, 100, stdin);
                        len = strlen(answer);
                        switch(answer[0]) {
                            case '1'://Menu de dificuldades
                                if (len == 2) {
                                    settings = 0;
                                    chooseDificulty = 1;
                                    while (chooseDificulty) {
                                        clear();
                                        printDificulty();
                                        fgets(answer, 100, stdin);
                                        len = strlen(answer);
                                        switch(answer[0]) {//verifica se o jogador já zerou as fases da dificuldade que escolheu e muda se não tiver zerado todas
                                            case '1':
                                                if (len == 2) {
                                                    if (totals[0] == solved[0]) {
                                                        printf("\nVoce ja completou todas as fases faceis! Aperte [Enter] para continuar: \n");
                                                        fgets(answer, 100, stdin);
                                                        break;
                                                    }
                                                    dificulty = 1;
                                                    chooseDificulty = 0;
                                                } else {
                                                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                                    fgets(answer, 100, stdin);
                                                }
                                                break;
                                            case '2':
                                                if (len == 2) {
                                                    if (totals[1] == solved[1]) {
                                                        printf("\nVoce ja completou todas as fases medias! Aperte [Enter] para continuar: \n");
                                                        fgets(answer, 100, stdin);
                                                        break;
                                                    }
                                                    dificulty = 2;
                                                    chooseDificulty = 0;
                                                } else {
                                                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                                    fgets(answer, 100, stdin);
                                                }
                                                break;
                                            case '3':
                                                if (len == 2) {
                                                    if (totals[2] == solved[2]) {
                                                        printf("\nVoce ja completou todas as fases dificeis! Aperte [Enter] para continuar: \n");
                                                        fgets(answer, 100, stdin);
                                                        break;
                                                    }
                                                    dificulty = 3;
                                                    chooseDificulty = 0;
                                                } else {
                                                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                                    fgets(answer, 100, stdin);
                                                }
                                                break;
                                            case '4':
                                                if (len == 2) {
                                                    chooseDificulty = 0;
                                                    settings = 1;
                                                } else {
                                                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                                    fgets(answer, 100, stdin);
                                                }
                                                break;
                                            default:
                                                printf("\nOpcao invalida! Selecione uma dificuldade, por favor. Aperte [Enter] para continuar: \n");
                                                fgets(answer, 100, stdin);
                                                break;
                                        }
                                    }
                                } else {
                                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                    fgets(answer, 100, stdin);
                                }
                                break;
                            case '2'://Menu de sugerir novo nivel
                                if (len == 2) {
                                    settings = 0;
                                    suggesting = 1;
                                    while (suggesting) {
                                        clear();
                                        printSuggestLevel();
                                        fgets(answer, 100, stdin);
                                        switch(answer[0]) {
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
                                                printf("\nOpcao invalida! Selecione uma opcao de configuracao, por favor. Aperte [Enter] para continuar: \n");
                                                fgets(answer, 100, stdin);
                                                break;
                                        }
                                    }
                                } else {
                                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                    fgets(answer, 100, stdin);
                                }
                                break;
                            case '3'://Reseta o ranking e mostra na tela q excluiu
                                if (len == 2) {
                                    clear();
                                    settings = 0;
                                    resetRanking();
                                } else {
                                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                    fgets(answer, 100, stdin);
                                }
                            case '4':
                                if (len == 2) {
                                    settings = 0;
                                } else {
                                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                                    fgets(answer, 100, stdin);
                                }
                                break;
                            default:
                                printf("\nOpcao invalida! Selecione uma opcao de configuracao, por favor. Aperte [Enter] para continuar: \n");
                                fgets(answer, 100, stdin);
                                break;
                        }
                    }
                } else {
                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                    fgets(answer, 100, stdin);
                }
                break;
            case '3'://Menu de ranking
                if (len == 2) {
                    ranking = 1;
                    while (ranking) {
                        clear();
                        printRanking();
                        fgets(answer, 100, stdin);
                        ranking = 0;
                    }
                } else {
                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                    fgets(answer, 100, stdin);
                }
                break;
            case '4'://Menu de instruções
                if (len == 2) {
                    clear();
                    instructions = 1;
                    printInstructions();
                } else {
                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                    fgets(answer, 100, stdin);
                }
                break;
            case '5':
                if (len == 2) {
                    return 0;
                } else {
                    printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                    fgets(answer, 100, stdin);
                }
                break;
            default:
                printf("\nOpcao invalida! Selecione uma opcao do menu, por favor. Aperte [Enter] para continuar: \n");
                fgets(answer, 100, stdin);
                break;
        }
    }
    return 0;
}