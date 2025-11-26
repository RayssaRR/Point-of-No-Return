#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"
#include "screen.h"
#include "keyboard.h"

#define COLUMN 40
#define LINE 19

int player_x = 1;
int player_y = 1;

int terminal_a = 0;
int terminal_b = 0;
int terminal_c = 0;

typedef struct {
    char name[100];
    double score;
    int win;
} Player;

void homeMenu(int *running, int *stop);
void printMap(char **map, Player player, int terminals[]);
void movePlayer(int x, int y, char **map, int *running, Player *player, int terminals[]);
void resetGame(Player *player, char **map, int terminals[]);

char lastChar = '.';

char initialMap[LINE][COLUMN + 1] = {
    "########################################",
    "#O.....................................#",
    "####.###########################.#######",
    "##........................######.....###",
    "########.###########################.###",
    "########...........##.................B#",
    "###############.##########.####.########",
    "#..................###.......##.########",
    "#########.#####################.########",
    "A......................................#",
    "######.#################.###############",
    "######...#######......................##",
    "########.###################.########.##",
    "###.........#############.........###.##",
    "########.###################.#######C.##",
    "######.......................###########",
    "##################.#####################",
    "######...............................###",
    "####################S###################"
};

int main() {

    int i;
    int running = 1;
    int stop = 1;
    char **map;

    int terminals[3] = { 0, 0, 0 };

    Player player;

    map = (char **) calloc(LINE, sizeof(char *));
    for (i = 0; i < LINE; i++) {
        map[i] = (char *) calloc(COLUMN + 1, sizeof(char));
    }

    while (1) {
        homeMenu(&running, &stop);
        if (stop == 0) break;

        while (1) {
            resetGame(&player, map, terminals);
            timerInit(1);

            while (running) {
                if (keyhit()) {
                    char ch = readch();

                    if (ch == 'w') {
                        movePlayer(player_x, player_y - 1, map, &running, &player, terminals);
                    } else if (ch == 's') {
                        movePlayer(player_x, player_y + 1, map, &running, &player, terminals);
                    } else if (ch == 'a') {
                        movePlayer(player_x - 1, player_y, map, &running, &player, terminals);
                    } else if (ch == 'd') {
                        movePlayer(player_x + 1, player_y, map, &running, &player, terminals);
                    } else if (ch == 'l') {
                        running = 0;
                        player.win = 0;
                    }

                    printMap(map, player, terminals);
                }
            }

            if (running == 0) break;
        }

    }


    keyboardInit();
    screenInit(1);

    return 0;
}

void homeMenu(int *running, int *dbreak) {
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

void printMap(char **map, Player player, int terminals[]) {
    screenClear();

    int offsetX = (MAXX - COLUMN) / 2;
    int offsetY = (MAXY - LINE) / 2;

    for (int y = 0; y < LINE; y++) {
        screenGotoxy(offsetX + 1, offsetY + y + 1);
        for (int x = 0; x < COLUMN; x++) {
            char ch = map[y][x];
            if (ch == '#') {
                screenSetColor(BLUE, BLUE);
            } else if (ch == 'O') {
                screenSetColor(GREEN, BLACK);
            } else if (ch == 'S') {
                if (terminals[0] + terminals[1] + terminals[2] == 3) {
                  screenSetColor(BLUE, GREEN);
                } else {
                  screenSetColor(BLUE, RED);
                }

            } else if (ch == 'A') {
                if (terminals[0]) {
                  screenSetColor(BLUE, GREEN);
                } else {
                  screenSetColor(RED, YELLOW);
                }
            } else if (ch == 'B') {
                if (terminals[1]) {
                  screenSetColor(BLUE, GREEN);
                } else {
                  screenSetColor(RED, YELLOW);
                }
            } else if (ch == 'C') {
                if (terminals[2]) {
                  screenSetColor(BLUE, GREEN);
                } else {
                  screenSetColor(RED, YELLOW);
                }

            } else {
                screenSetColor(BLACK, BLACK);
            }
            printf("%c", ch);
        }
    }
    screenUpdate();
}

void movePlayer(int x, int y, char **map, int *running, Player *player, int terminals[]) {

    if (map[y][x] == '.' || map[y][x] == 'A' || map[y][x] == 'B' || map[y][x] == 'C') {

      if (map[y][x] != '.') {
          // TODO: DEVE ABRIR UMA NOVA TELA COM A SENTENÇA LÓGICA E IR ESCREVENDO AS RESPOSTAS

          if (map[y][x] == 'A') terminals[0] = 1;
          if (map[y][x] == 'B') terminals[1] = 1;
          if (map[y][x] == 'C') terminals[2] = 1;

          lastChar = map[y][x];
          map[player_y][player_x] = '.';
      } else {
          if (lastChar != '.') {
            map[player_y][player_x] = lastChar;
            lastChar = '.';
          } else {
            map[player_y][player_x] = '.';
          }
      }

      if (terminals[0] + terminals[1] + terminals[2] == 3) {
          map[17][9] = '.';
      }

      player_x = x;
      player_y = y;
      map[player_y][player_x] = 'O';

    } else if (map[y][x] == 'S') {
        if (terminals[0] + terminals[1] + terminals[2] == 3) {
          // TODO: PASSA DE FASE
            *running = 0;
        }
    }
}

void resetGame(Player *player, char **map, int terminals[]) {
    player_x = 1;
    player_y = 1;

    for (int i = 0; i < LINE; i++) strcpy(map[i], initialMap[i]);

    player->win = 0;

    terminals[0] = 0;
    terminals[1] = 0;
    terminals[2] = 0;

    printMap(map, *player, terminals);
}