#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "timer.h"
#include "screen.h"
#include "keyboard.h"

#define COLUMN 40
#define LINE 19

char lastChar = '.';

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

// char movePlayer(
//   MoveDirection direction,
//   char **map,
//   int *running,
//   Player *player
// ) {

//   int x = player->x;
//   int y = player->y;

//   if (direction == UP) {
//     y = player->y - 1;
//     x = player->x;
//   } else if (direction == DOWN) {
//     y = player->y + 1;
//     x = player->x;
//   } else if (direction == LEFT) {
//     y = player->y;
//     x = player->x - 1;
//   } else if (direction == RIGHT) {
//     y = player->y;
//     x = player->x + 1;
//   }

//   if (map[y][x] == '.' || map[y][x] == 'S') {
//     if (map[y][x] == 'S') {
//       player->win = 1;
//       *running = 0;
//     }

//     map[player->y][player->x] = '.';

//     player->x = x;
//     player->y = y;
//     map[player->y][player->x] = 'O';
//   }

//   return map[y][x];
// }

