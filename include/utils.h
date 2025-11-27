#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include "player.h"

#define COLUMN 40
#define LINE 19

typedef enum { UP, DOWN, LEFT, RIGHT } MoveDirection;

void showInitialMenu(int *running, int *dbreak);

void inputUsername(char username[]);

void type_effect(const char *text, useconds_t delay);

void pixie_speak(const char *text);

void dialogue_zero();
void dialogue_one();
void dialogue_two();
void dialogue_three();
void dialogue_final();

#endif