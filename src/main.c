#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "screen.h"
#include "timer.h"
#include "stage_one.h"
#include "stage_two.h"
#include "stage_three.h"
#include "stage_four.h"
#include "utils.h"

void free_map_memory(char **map);

int main() {
    int running = 1;
    int stop = 1;
    char username[100];
    int penalty = 0;

    char **allocated_map;
    allocated_map = (char **) calloc(LINE, sizeof(char *));
    for (int i = 0; i < LINE; i++) {
        allocated_map[i] = (char *) calloc(COLUMN + 1, sizeof(char));
    }

    showInitialMenu(&running, &stop);
    if (stop == 0) return 0;

    inputUsername(username);

    timerInit(0);
    time_t inicio = time(NULL);

    Player player;
    player.x = 1;
    player.y = 1;

    stage_one(allocated_map, &player);
    stage_two(allocated_map, &player, &penalty);
    stage_three(allocated_map, &player);
    stage_four(allocated_map, &player, &penalty);

    time_t fim = time(NULL); // Finaliza o cronometro
    int tempo_jogo = (int) difftime(fim, inicio);

    if (tempo_jogo + penalty < 0) tempo_jogo = 0;
    else tempo_jogo = tempo_jogo + penalty;

    screenClear();
    screenGotoxy(1,1);
    screenSetColor(BLACK, BLUE);
    printf("Parabéns %s, sua pontuação foi: %d\n\n", username, tempo_jogo);

    printf("Obrigado por jogar Point of No Return!\n");
    //EXIT

    free_map_memory(allocated_map);
    return 0;
}

void free_map_memory(char **map) {
    for (int i = 0; i < LINE; i++) free(map[i]);
    free(map);
}