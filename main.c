#include <stdio.h>
#include <ctype.h>
#include <string.h>

int dificulty = 1;
char player[20];

void printMenu(int invalid) {
    printf("\n");
    if (invalid == 1) {
        printf("Opcao invalida! Selecione uma opcao do menu, por favor.\n\n");
    }
    printf("        Number Sums - Made by: Victor Yan        \n");
    printf("################################################\n\n");
    printf("[1] - Jogar\n");
    printf("[2] - Configuracoes\n");
    printf("[3] - Ranking\n");
    printf("[4] - Sair\n\n");
    printf("################################################\n\n");
    printf("Escolha uma opcao: ");
    return;
}

int Play() {
    printf("playing");
    return 1;
}

void printConfig(int invalid) {
    printf("\n");
    if (invalid != 0) {
        printf("Opcao invalida! Selecione uma opcao do menu, por favor.\n\n");
    }
    printf("                  Configuracoes                   \n");
    printf("################################################\n\n");
    printf("[1] - Dificuldade\n");
    printf("[2] - Sugerir novo nivel\n");
    printf("[3] - Voltar\n\n");
    printf("################################################\n\n");
    printf("Escolha uma opcao: ");
    return;
}

void EnterNickname() {
    printf("\nDigite seu nome: ");
    scanf("%s", player);
    int len = strlen(player);
    for (int i = 0; i < len; i++) {
        if (!isalpha(player[i]) && player[i] != ' ') { 
            printf("\nNome invalido! Digite um nome valido, por favor.\n");
            EnterNickname();
            return;
        }
    }
    return;
}

int main() {
    EnterNickname();
    while (1) {
        char option = '0';
        printMenu(0);
        scanf("%c", &option);
        printf("\n");
        switch(option) {
            case '1':
                Play();
            break;
            case '2':
                printConfig(0);
                option = '0';
                while ((option-'0') < 1 || (option-'0') > 3) {
                    scanf("%c", &option);
                    printf("\n");
                    switch(option) {
                        case '1':
                            printf("Dificuldade\n");
                        break;
                        case '2':
                            printf("Sugerir novo nivel\n");
                        break;
                        case '3':
                        break;
                        default:
                            printConfig(option-'0');
                        break;
                    }
                }
            break;
            case '3':
                printf("Ranking\n");
            break;
            case '4':
                return 0;
        }
    }
}