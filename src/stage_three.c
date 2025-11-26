#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "player.h"
#include "screen.h"
#include "timer.h"
#include "keyboard.h"

#define LINE 19
#define COLUMN 40
#define NUM_TERMINALS 4
#define NUM_CAMERAS 4
#define NUM_SENTENCES 3

typedef struct {
    char sentence[200];
    int terminals[NUM_TERMINALS];
} StageThreeSentence;

char stage_three_map_template[LINE][COLUMN + 1] = {
    "#######################################",
    "#O.............................A........",
    "####.######################...##.######.",
    "..............xxxx.................D##.",
    "##.#####.######X#########.#######....#.",
    ".#.#####......xxxx..##.......B......#.",
    "##.############.##########.####y######.",
    "...................###.......##y######.",
    ".####.##.#####################yY#####.",
    "C...........................yyyyyyyyy....",
    ".#####.#################.############.#.",
    ".#####.....................T..........#.",
    ".#######w###################z########.#.",
    ".....wwwwwww############Zzzzzzzzzz###.#.",
    ".#######wW##################z#######.#.",
    ".#####wwwwwww................##########.",
    ".#################.####################.",
    ".####.P...............................##.",
    "####################S###################"
};

StageThreeSentence stage_three_sentences[NUM_SENTENCES] = {
    {"(¬A -> ¬C) ^ (B <-> D)", {0, 1, 0, 1}},
    {"(A <-> B) -> (C -> D)", {1, 1, 0, 0}},
    {"(A ^ (B -> C)) -> D", {1, 1, 1, 0}}
};

void printMapStageThree(
    char **map,
    Player *player,
    int terminals[],
    StageThreeSentence sentences[],
    int *current_sentence_index,
    int currentCamera,
    int robot_r_y,
    int robot_g_y,
    int *running
);

void setupStageThree(
    Player *player,
    char **allocated_map,
    int terminals[],
    char stage_map[LINE][COLUMN + 1]
);

void openTerminalStageThree(
    int terminals[],
    char terminal,
    StageThreeSentence sentences[],
    int *current_sentence_index,
    Player *player
);

int open_terminal_modelStageThree(char terminal);

void resetIndexStageThree(int *item, int total);

char movePlayer_stageThree(
    MoveDirection direction,
    char **map,
    int *running,
    Player *player,
    char *lastChar
);

void checkTimerStageThree(Player *player, int *running);

int stage_three(char **allocated_map, Player *player) {
    const int ROBOT_R_FIXED_X = 29;
    const int ROBOT_R_START_Y = 3;
    const int ROBOT_R_END_Y = 5;

    const int ROBOT_G_FIXED_X = 5;
    const int ROBOT_G_START_Y = 8;
    const int ROBOT_G_END_Y = 11;

    const int TELEPORT_EXIT_X = 6;
    const int TELEPORT_EXIT_Y = 17;

    static int robot_r_y = 0;
    static int robot_r_x = 0;
    static int robot_r_direction = 1;

    static int robot_g_y = 0;
    static int robot_g_x = 0;
    static int robot_g_direction = 1;

    static int robot_initialized = 0;

    player->x = 1;
    player->y = 1;

    int current_sentence_index = 0;
    int running = 1;
    int currentCamera = 0;
    int terminals[NUM_TERMINALS] = {-1, -1, -1, -1};

    char requiredPosition = '.';
    char lastChar = '.';
    int ch = 0;
    int lastCameraSwitch = 0;
    int lastRobotMove = 0;

    keyboardInit();
    screenInit(0);

    if (!robot_initialized) {
        robot_r_y = ROBOT_R_START_Y;
        robot_r_x = ROBOT_R_FIXED_X;

        robot_g_y = ROBOT_G_START_Y;
        robot_g_x = ROBOT_G_FIXED_X;

        robot_initialized = 1;
    }

    setupStageThree(player, allocated_map, terminals, stage_three_map_template);

    timerInit(0);
    lastCameraSwitch = getTimeDiff();
    lastRobotMove = getTimeDiff();

    while (1) {
        printMapStageThree(allocated_map, player, terminals, stage_three_sentences, &current_sentence_index, currentCamera, robot_r_y, robot_g_y, &running);

        while (running) {
            if (keyhit()) {
                ch = readch();

                if (ch == 'w') {
                    requiredPosition = movePlayer_stageThree(UP, allocated_map, &running, player, &lastChar);
                } else if (ch == 's') {
                    requiredPosition = movePlayer_stageThree(DOWN, allocated_map, &running, player, &lastChar);
                } else if (ch == 'a') {
                    requiredPosition = movePlayer_stageThree(LEFT, allocated_map, &running, player, &lastChar);
                } else if (ch == 'd') {
                    requiredPosition = movePlayer_stageThree(RIGHT, allocated_map, &running, player, &lastChar);
                } else if (ch == 'l') {
                    running = 0; player->win = 0;
                }

                openTerminalStageThree(terminals, requiredPosition, stage_three_sentences, &current_sentence_index, player);

                if (requiredPosition == 'T') {
                    player->x = TELEPORT_EXIT_X;
                    player->y = TELEPORT_EXIT_Y;

                    requiredPosition = stage_three_map_template[player->y][player->x];
                }

                printMapStageThree(allocated_map, player, terminals, stage_three_sentences, &current_sentence_index, currentCamera, robot_r_y, robot_g_y, &running);
            }

            int currentTime = getTimeDiff();

            if (currentTime - lastCameraSwitch >= 3000) {
                resetIndexStageThree(&currentCamera, NUM_CAMERAS);
                lastCameraSwitch = currentTime;
                printMapStageThree(allocated_map, player, terminals, stage_three_sentences, &current_sentence_index, currentCamera, robot_r_y, robot_g_y, &running);
            }

            if (currentTime - lastRobotMove >= 500) {
                int next_r_y = robot_r_y + robot_r_direction;

                if (next_r_y < ROBOT_R_START_Y || next_r_y > ROBOT_R_END_Y || stage_three_map_template[next_r_y][robot_r_x] == '#') {
                    robot_r_direction *= -1;
                }
                robot_r_y += robot_r_direction;

                int next_g_y = robot_g_y + robot_g_direction;

                if (next_g_y < ROBOT_G_START_Y || next_g_y > ROBOT_G_END_Y || stage_three_map_template[next_g_y][robot_g_x] == '#') {
                    robot_g_direction *= -1;
                }
                robot_g_y += robot_g_direction;

                lastRobotMove = currentTime;

                printMapStageThree(allocated_map, player, terminals, stage_three_sentences, &current_sentence_index, currentCamera, robot_r_y, robot_g_y, &running);
            }

            char player_pos_char = stage_three_map_template[player->y][player->x];

            if ((player->x == robot_r_x && player->y == robot_r_y) || (player->x == robot_g_x && player->y == robot_g_y)) {
                player->score -= 20;
                player->x = 1;
                player->y = 1;
                resetIndexStageThree(&current_sentence_index, NUM_SENTENCES);
                for (int i = 0; i < NUM_TERMINALS; i++) terminals[i] = -1;
            }

            int caughtByProjection = (player_pos_char == 'w' && currentCamera == 0) ||
                                     (player_pos_char == 'x' && currentCamera == 1) ||
                                     (player_pos_char == 'y' && currentCamera == 2) ||
                                     (player_pos_char == 'z' && currentCamera == 3);

            int caughtByFixedUnit = (player_pos_char == 'W' || player_pos_char == 'X' || player_pos_char == 'Y' || player_pos_char == 'Z');

            if (caughtByProjection || caughtByFixedUnit) {
                player->score -= 7;

                player->x = 1;
                player->y = 1;

                resetIndexStageThree(&currentCamera, NUM_CAMERAS);
                resetIndexStageThree(&current_sentence_index, NUM_SENTENCES);
                for (int i = 0; i < NUM_TERMINALS; i++) terminals[i] = -1;
            }

            if (player_pos_char == 'o') {
                player->x = 1;
                player->y = 1;
            }

            if (player_pos_char == 'F') {
                player->score = 0;
                player->x = 1;
                player->y = 1;
                resetIndexStageThree(&current_sentence_index, NUM_SENTENCES);
                for (int i = 0; i < NUM_TERMINALS; i++) terminals[i] = -1;
            }

            checkTimerStageThree(player, &running);
            if (player->score < 0) { player->score = 0; }
        }

        if (!running) break;
    }

    return 0;
}

void printMapStageThree(
    char **map,
    Player *player,
    int terminals[],
    StageThreeSentence sentences[],
    int *current_sentence_index,
    int currentCamera,
    int robot_r_y,
    int robot_g_y,
    int *running
) {
    screenClear();

    const int ROBOT_R_FIXED_X = 29;
    const int ROBOT_G_FIXED_X = 5;

    int offsetX = (MAXX - COLUMN - 10) / 2;
    int offsetY = (MAXY - LINE) / 2;

    int all_solved = 1;
    for (int i = 0; i < NUM_TERMINALS; i++) {
        if (terminals[i] != sentences[*current_sentence_index].terminals[i]) {
            all_solved = 0;
            break;
        }
    }

    for (int y = 0; y < LINE; y++) {
        screenGotoxy(offsetX + 1, offsetY + y + 1);

        for (int x = 0; x < COLUMN; x++) {
            char map_char = stage_three_map_template[y][x];
            char char_to_print = map_char;

            int is_player_pos = (x == player->x && y == player->y);
            int is_robot_r_pos = (x == ROBOT_R_FIXED_X && y == robot_r_y);
            int is_robot_g_pos = (x == ROBOT_G_FIXED_X && y == robot_g_y);
            int is_robot_pos = is_robot_r_pos || is_robot_g_pos;

            if (is_player_pos && is_robot_pos) {
                char_to_print = 'O';
                screenSetColor(RED, WHITE);
            } else if (is_player_pos) {
                char_to_print = 'O';
                screenSetColor(GREEN, BLACK);
            } else if (is_robot_r_pos) {
                char_to_print = 'R';
                screenSetColor(WHITE, BLACK);
            } else if (is_robot_g_pos) {
                char_to_print = 'G';
                screenSetColor(MAGENTA, BLACK);
            } else {
                char_to_print = map_char;

                if (char_to_print == '#') {
                    screenSetColor(BLUE, BLUE);
                } else if (char_to_print == 'S') {
                    screenSetColor(BLUE, all_solved ? GREEN : RED);
                } else if (char_to_print >= 'A' && char_to_print <= 'D') {
                    int index = char_to_print - 'A';
                    if (terminals[index] == -1) screenSetColor(RED, YELLOW);
                    else screenSetColor(BLUE, GREEN);
                } else if (char_to_print == 'T') {
                    screenSetColor(CYAN, BLACK);
                } else if (char_to_print == 'P') {
                    screenSetColor(CYAN, WHITE);
                } else if (char_to_print == 'W' || char_to_print == 'X' || char_to_print == 'Y' || char_to_print == 'Z') {
                    screenSetColor(BLUE, BROWN);
                } else if (char_to_print == 'w') {
                    screenSetColor(currentCamera == 0 ? RED : BLACK, currentCamera == 0 ? RED : BLACK);
                } else if (char_to_print == 'x') {
                    screenSetColor(currentCamera == 1 ? RED : BLACK, currentCamera == 1 ? RED : BLACK);
                } else if (char_to_print == 'y') {
                    screenSetColor(currentCamera == 2 ? RED : BLACK, currentCamera == 2 ? RED : BLACK);
                } else if (char_to_print == 'z') {
                    screenSetColor(currentCamera == 3 ? RED : BLACK, currentCamera == 3 ? RED : BLACK);
                } else if (char_to_print == 'o') {
                    screenSetColor(BLACK, WHITE);
                } else if (char_to_print == 'F') {
                    screenSetColor(WHITE, RED);
                } else {
                    screenSetColor(BLACK, BLACK);
                    char_to_print = ' ';
                }
            }

            putchar(char_to_print);

            if (map_char == 'S' && all_solved && is_player_pos) {
                player->win = 1;
                *running = 0;
            }
        }
    }


    screenGotoxy(offsetX + COLUMN + 4, offsetY + LINE - 8);
    printf("Score: %.2f", player->score);
    screenGotoxy(offsetX + COLUMN + 4, offsetY + LINE - 7);
    printf("Senha: %s", sentences[*current_sentence_index].sentence);

    char terminal_labels[] = {'A', 'B', 'C', 'D'};
    for (int i = 0; i < NUM_TERMINALS; i++) {
        screenGotoxy(offsetX + COLUMN + 4, offsetY + LINE - 6 + i);
        if (terminals[i] == sentences[*current_sentence_index].terminals[i]) {
            screenSetColor(BLACK, GREEN);
            printf("Terminal %c: OK ", terminal_labels[i]);
        } else {
            screenSetColor(BLACK, BROWN);
            printf("Terminal %c: -- ", terminal_labels[i]);
        }
    }

    screenGotoxy(offsetX + COLUMN + 4, offsetY + LINE - 6 + NUM_TERMINALS);
    if (all_solved) {
        screenSetColor(GREEN, GREEN);
        printf("EXIT OPENED! ");
    } else {
        screenSetColor(BLUE, BLUE);
        printf("EXIT LOCKED. ");
    }

    checkTimerStageThree(player, running);

    int boxX = offsetX + COLUMN + 4;
    int boxY = offsetY + 2;

    screenSetColor(BLACK, BLUE);

    screenGotoxy(boxX, boxY);
    printf("+------------------------------+");

    screenGotoxy(boxX, boxY + 1);
    printf("| GUIA DO MAPA                |");

    screenGotoxy(boxX, boxY + 2);
    printf("| (R) Robô Vermelho -> Reinicia |");

    screenGotoxy(boxX, boxY + 3);
    printf("| (G) Robô Verde   -> Reinicia |");

    screenGotoxy(boxX, boxY + 4);
    printf("| Cameras -> -7 se te virem    |");

    screenGotoxy(boxX, boxY + 5);
    printf("| Dica: fique imóvel e protegido|");

    screenGotoxy(boxX, boxY + 6);
    printf("| Terminais A/B/C/D -> 0 ou 1  |");

    screenGotoxy(boxX, boxY + 7);
    printf("| (T) -> Teleporte             |");

    screenGotoxy(boxX, boxY + 8);
    printf("| (S) -> Saída do estágio       |");

    screenGotoxy(boxX, boxY + 9);
    printf("+------------------------------+");

    screenSetColor(BLACK, BLUE);


    screenUpdate();
}

void setupStageThree(
    Player *player,
    char **allocated_map,
    int terminals[],
    char stage_map[LINE][COLUMN + 1]
) {
    player->x = 1;
    player->y = 1;

    for (int i = 0; i < LINE; i++) strcpy(allocated_map[i], stage_map[i]);
    player->win = 0;

    for (int i = 0; i < NUM_TERMINALS; i++) {
        terminals[i] = -1;
    }
}

void resetIndexStageThree(int *current_index, int total) {
    *current_index = (*current_index + 1) % total;
    screenUpdate();
}

char movePlayer_stageThree(
    MoveDirection direction,
    char **map,
    int *running,
    Player *player,
    char *lastChar
) {
    int x = player->x;
    int y = player->y;

    if (direction == UP) y--;
    else if (direction == DOWN) y++;
    else if (direction == LEFT) x--;
    else if (direction == RIGHT) x++;

    if (x < 0 || x >= COLUMN || y < 0 || y >= LINE) return '#';

    char next = stage_three_map_template[y][x];

    if (next == '#') {
        return next;
    }

    player->x = x;
    player->y = y;

    return next;
}

void openTerminalStageThree(int terminals[], char requiredTerminal,
    StageThreeSentence sentences[], int *current_sentence_index, Player *player) {

    if (requiredTerminal < 'A' || requiredTerminal > 'D') return;

    int index = requiredTerminal - 'A';

    int value = open_terminal_modelStageThree(requiredTerminal);

    if (value == sentences[*current_sentence_index].terminals[index]) {
        terminals[index] = value;
        player->score += 10;
    } else {
        resetIndexStageThree(current_sentence_index, NUM_SENTENCES);
        for (int i = 0; i < NUM_TERMINALS; i++) terminals[i] = -1;
        screenSetColor(RED, RED);
    }
}

void checkTimerStageThree(Player *player, int *running) {
    int elapsed = getTimeDiff();
    screenGotoxy(MAXX - 15, MAXY - 12);
    printf("Tempo: %d s", elapsed / 1000);
}

int open_terminal_modelStageThree(char terminal) {
    int terminal_value = -1;

    keyboardDestroy();

    screenClear();
    screenGotoxy(1, 1);
    screenSetColor(BLACK, BLUE);
    printf("Digite o valor do terminal %c: ", terminal);
    fflush(stdout);

    int ch;
    do {
        ch = getchar();
    } while (ch != '0' && ch != '1' && ch != EOF);

    if (ch == EOF) ch = '0';
    terminal_value = ch - '0';

    printf("%d\n", terminal_value);
    fflush(stdout);

    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}

    keyboardInit();

    return terminal_value;
}
