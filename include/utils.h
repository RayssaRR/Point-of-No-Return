#ifndef UTILS_H
#define UTILS_H

#include "player.h"

#define COLUMN 40
#define LINE 19

typedef enum { UP, DOWN, LEFT, RIGHT } MoveDirection;

void showInitialMenu(int *running, int *dbreak);

// void printMap(char **map, Player *player, int terminals[], int *camera);

// char movePlayer(
//   MoveDirection direction,
//   char **map,
//   int *running,
//   Player *player
// );

// void setupGame(
//   Player *player,
//   char **allocated_map,
//   int terminals[],
//   int *camera,
//   char stage_map[LINE][COLUMN + 1]
// );

#endif