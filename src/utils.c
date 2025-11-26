#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "timer.h"
#include "screen.h"
#include "keyboard.h"

void showInitialMenu(int *running, int *dbreak) {
    screenClear();

    char ch = '\0';

    printf(
        "░█▀█░█▀█░▀█▀░█▀█░▀█▀░░░█▀█░█▀▀    \n"
        "░█▀▀░█░█░░█░░█░█░░█░░░░█░█░█▀▀    \n"
        "░▀░░░▀▀▀░▀▀▀░▀░▀░░▀░░░░▀▀▀░▀░░    \n"
        "░█▀█░█▀█░░░█▀▄░█▀▀░▀█▀░█░█░█▀▄░█▀█\n"
        "░█░█░█░█░░░█▀▄░█▀▀░░█░░█░█░█▀▄░█░█\n"
        "░▀░▀░▀▀▀░░░▀░▀░▀▀▀░░▀░░▀▀▀░▀░▀░▀░▀\n"
    );
    printf("\nVocê recebe um chamado, aceita? (y/n)\n");
    screenUpdate();

    while (1) {
        ch = readch();
        if (ch == 'y') {
            *running = 1;
            break;
        } else if (ch == 'n') {
            *running = 0;
            *dbreak=0;
            break;
        }
    }
}

void inputUsername(char username[]) {
    screenClear();
    printf("\nDigite o seu nome: \n");
    scanf("%s", username);
}
