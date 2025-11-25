#include <stdlib.h>
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

    char **allocated_map;
    allocated_map = (char **) calloc(LINE, sizeof(char *));
    for (int i = 0; i < LINE; i++) {
        allocated_map[i] = (char *) calloc(COLUMN + 1, sizeof(char));
    }

    showInitialMenu(&running, &stop);
    if (stop == 0) return 0;

    timerInit(0); // Inicializa o timer sem contagem regressiva

    Player player;
    player.x = 1;
    player.y = 1;

    stage_one(allocated_map, &player);
    stage_two(allocated_map, &player);
    stage_three(allocated_map, &player);
    stage_four(allocated_map, &player);

    //EXIT

    free_map_memory(allocated_map);
    return 0;
}

void free_map_memory(char **map) {
    for (int i = 0; i < LINE; i++) free(map[i]);
    free(map);
}