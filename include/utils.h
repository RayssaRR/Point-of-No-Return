#ifndef UTILS_H
#define UTILS_H

#include "player.h"

#define COLUMN 40
#define LINE 19

typedef enum { UP, DOWN, LEFT, RIGHT } MoveDirection;

void showInitialMenu(int *running, int *dbreak);

void inputUsername(char username[]);

#endif