#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "player.h"
#include "screen.h"
#include "keyboard.h"

#define LINE 19
#define COLUMN 40
#define NUM_TERMINALS 4
#define NUM_CAMERAS 4
#define NUM_VARS_ABC 3
#define TRUTH_ROWS_ABC 8

// Control "time" without real timer
#define CAMERA_SWITCH_INTERVAL 4000   // how many loops to switch camera
#define ROBOT_MOVE_INTERVAL    800    // how many loops to move robots

typedef struct StageThreeSentence {
    char sentence[200];                            // logical formula
    int inputs[TRUTH_ROWS_ABC][NUM_VARS_ABC];      // truth table inputs (A,B,C)
    int outputs[TRUTH_ROWS_ABC];                   // truth table expected outputs
} StageThreeSentence;

// --------- Map template ---------

char stage_three_map_template[LINE][COLUMN + 1] = {
    "########################################",
    ".O.............................A........",
    ".#################.#########..#.######..",
    "..............xxxx......................",
    ".#######.######X#########.############..",
    "..............xxxx..##.......B.........",
    "##############.##########.####.########",
    "#..................###.........y........",
    "#####.##.######################Y#######",
    "C...........................yyyyyyyyy..#",
    ".#####.#################.############.##",
    ".#####.....................Tz..........",
    "########w###################z########.#",
    ".....wwwwwww############Zzzzzzzzzz...#.",
    ".#######wW##################z#######..",
    ".#####wwwwwww................#########",
    ".......###########.###################",
    "#####.P................................",
    "####################S##################"
};

// --------- Sentences for A, B, C, D ---------

StageThreeSentence stage_three_sentences[NUM_TERMINALS] = {
    {
        "(A -> B) -> C",
        {
            {0, 0, 0},
            {0, 0, 1},
            {0, 1, 0},
            {0, 1, 1},
            {1, 0, 0},
            {1, 0, 1},
            {1, 1, 0},
            {1, 1, 1}
        },
        {1, 1, 1, 1, 1, 1, 0, 1}
    },
    {
        "A ^ (B -> C)",
        {
            {0, 0, 0},
            {0, 0, 1},
            {0, 1, 0},
            {0, 1, 1},
            {1, 0, 0},
            {1, 0, 1},
            {1, 1, 0},
            {1, 1, 1}
        },
        {0, 0, 0, 0, 1, 1, 0, 1}
    },
    {
        "(A v B) -> C",
        {
            {0, 0, 0},
            {0, 0, 1},
            {0, 1, 0},
            {0, 1, 1},
            {1, 0, 0},
            {1, 0, 1},
            {1, 1, 0},
            {1, 1, 1}
        },
        {1, 1, 0, 1, 0, 1, 0, 1}
    },
    {
        "A -> (B ^ ¬C)",
        {
            {0, 0, 0},
            {0, 0, 1},
            {0, 1, 0},
            {0, 1, 1},
            {1, 0, 0},
            {1, 0, 1},
            {1, 1, 0},
            {1, 1, 1}
        },
        {1, 1, 1, 1, 0, 0, 1, 0}
    }
};

// --------- Prototypes ---------

void setupStageThree(
    Player *player,
    char **allocated_map,
    int terminal_states[],
    char stage_map[LINE][COLUMN + 1]
);

void printMapStageThree(
    char **allocated_map,
    Player *player,
    int terminal_states[],
    StageThreeSentence sentences[],
    int *current_sentence_index,
    int current_camera_index,
    int robot_red_y,
    int robot_green_y,
    int *is_running
);

char movePlayerStageThree(
    MoveDirection direction,
    char **allocated_map,
    int *is_running,
    Player *player
);

void openTerminalStageThree(
    int terminal_states[],
    char terminal_position,
    StageThreeSentence sentences[],
    int *current_sentence_index,
    Player *player
);

int openTerminalTruthTableModelStageThree(
    StageThreeSentence sentences[],
    char terminal_position
);

void resetIndexStageThree(int *current_index, int total);

// --------- Main stage function ---------

int stage_three(char **allocated_map, Player *player) {
    const int ROBOT_RED_FIXED_X   = 29;
    const int ROBOT_RED_START_Y   = 1;
    const int ROBOT_RED_END_Y     = 3;

    const int ROBOT_GREEN_FIXED_X = 5;
    const int ROBOT_GREEN_START_Y = 8;
    const int ROBOT_GREEN_END_Y   = 11;

    const int TELEPORT_EXIT_X = 6;
    const int TELEPORT_EXIT_Y = 17;

    static int robot_red_y           = 0;
    static int robot_red_x           = 0;
    static int robot_red_direction   = 1; 

    static int robot_green_y         = 0;
    static int robot_green_x         = 0;
    static int robot_green_direction = 1;

    static int robots_initialized    = 0;

    player->x = 1;
    player->y = 1;

    int current_sentence_index = 0;
    int is_running             = 1;
    int current_camera_index   = 0;         
    int terminal_states[NUM_TERMINALS] = {-1, -1, -1, -1}; 

    char last_required_position = '.';
    int key_pressed = 0;
    int need_redraw = 0;

    long long loop_counter = 0;
    long long next_camera_switch_tick = CAMERA_SWITCH_INTERVAL;
    long long next_robot_move_tick    = ROBOT_MOVE_INTERVAL;

    keyboardInit();
    screenInit(0);

    setupStageThree(player, allocated_map, terminal_states, stage_three_map_template);


    if (!robots_initialized) {
        robot_red_y  = ROBOT_RED_START_Y;
        robot_red_x  = ROBOT_RED_FIXED_X;

        robot_green_y = ROBOT_GREEN_START_Y;
        robot_green_x = ROBOT_GREEN_FIXED_X;

        robots_initialized = 1;
    }

    setupStageThree(player, allocated_map, terminal_states, stage_three_map_template);

    while (1) {
        printMapStageThree(
            allocated_map,
            player,
            terminal_states,
            stage_three_sentences,
            &current_sentence_index,
            current_camera_index,
            robot_red_y,
            robot_green_y,
            &is_running
        );

        while (is_running) {
            loop_counter++;

            if (loop_counter >= next_robot_move_tick) {
                int next_red_y = robot_red_y + robot_red_direction;
                if (next_red_y < ROBOT_RED_START_Y ||
                    next_red_y > ROBOT_RED_END_Y ||
                    stage_three_map_template[next_red_y][robot_red_x] == '#') {
                    robot_red_direction *= -1;
                }
                robot_red_y += robot_red_direction;

                int next_green_y = robot_green_y + robot_green_direction;
                if (next_green_y < ROBOT_GREEN_START_Y ||
                    next_green_y > ROBOT_GREEN_END_Y ||
                    stage_three_map_template[next_green_y][robot_green_x] == '#') {
                    robot_green_direction *= -1;
                }
                robot_green_y += robot_green_direction;

                next_robot_move_tick = loop_counter + ROBOT_MOVE_INTERVAL;
                need_redraw = 1;
            }

            if (loop_counter >= next_camera_switch_tick) {
                resetIndexStageThree(&current_camera_index, NUM_CAMERAS);
                next_camera_switch_tick = loop_counter + CAMERA_SWITCH_INTERVAL;
                need_redraw = 1;
            }

            if (keyhit()) {
                key_pressed = readch();

                if (key_pressed == 'w') {
                    last_required_position = movePlayerStageThree(UP, allocated_map, &is_running, player);
                    need_redraw = 1;
                } else if (key_pressed == 's') {
                    last_required_position = movePlayerStageThree(DOWN, allocated_map, &is_running, player);
                    need_redraw = 1;
                } else if (key_pressed == 'a') {
                    last_required_position = movePlayerStageThree(LEFT, allocated_map, &is_running, player);
                    need_redraw = 1;
                } else if (key_pressed == 'd') {
                    last_required_position = movePlayerStageThree(RIGHT, allocated_map, &is_running, player);
                    need_redraw = 1;
                } else if (key_pressed == 'l') {
                    is_running = 0;
                    player->win = 0;
                    need_redraw = 1;

                }

                openTerminalStageThree(
                    terminal_states,
                    last_required_position,
                    stage_three_sentences,
                    &current_sentence_index,
                    player
                );

                if (last_required_position == 'T') {
                    player->x = TELEPORT_EXIT_X;
                    player->y = TELEPORT_EXIT_Y;
                    last_required_position = stage_three_map_template[player->y][player->x];
                }
            }

            char player_map_char = stage_three_map_template[player->y][player->x];

            int player_hit_red_robot   = (player->x == robot_red_x   && player->y == robot_red_y);
            int player_hit_green_robot = (player->x == robot_green_x && player->y == robot_green_y);

            if (player_hit_red_robot || player_hit_green_robot) {
                player->score -= 20;
                if (player->score < 0) player->score = 0;

                player->x = 1;
                player->y = 1;
            }

            int player_hit_projection_camera =
                (player_map_char == 'w' && current_camera_index == 0) ||
                (player_map_char == 'x' && current_camera_index == 1) ||
                (player_map_char == 'y' && current_camera_index == 2) ||
                (player_map_char == 'z' && current_camera_index == 3);

            int player_hit_fixed_camera =
                (player_map_char == 'W' ||
                 player_map_char == 'X' ||
                 player_map_char == 'Y' ||
                 player_map_char == 'Z');

            if (player_hit_projection_camera || player_hit_fixed_camera) {
                player->score -= 7;
                if (player->score < 0) player->score = 0;

                player->x = 1;
                player->y = 1;
            }

            if (need_redraw) {
                printMapStageThree(
                    allocated_map,
                    player,
                    terminal_states,
                    stage_three_sentences,
                    &current_sentence_index,
                    current_camera_index,
                    robot_red_y,
                    robot_green_y,
                    &is_running
                );
                need_redraw = 0; 
            }
        }

        if (!is_running) break;
    }

    return 0;
}


void setupStageThree(
    Player *player,
    char **allocated_map,
    int terminal_states[],
    char stage_map[LINE][COLUMN + 1]
) {
    player->x = 1;
    player->y = 1;
    player->win = 0;
    player->score = 0;

    for (int i = 0; i < LINE; i++) {
        strcpy(allocated_map[i], stage_map[i]);
    }

    for (int i = 0; i < NUM_TERMINALS; i++) {
        terminal_states[i] = -1;
    }
}


void resetIndexStageThree(int *current_index, int total) {
    *current_index = (*current_index + 1) % total;
}


char movePlayerStageThree(
    MoveDirection direction,
    char **allocated_map,
    int *is_running,
    Player *player
) {
    (void)allocated_map;
    (void)is_running;

    int next_x = player->x;
    int next_y = player->y;

    if (direction == UP) next_y--;
    else if (direction == DOWN) next_y++;
    else if (direction == LEFT) next_x--;
    else if (direction == RIGHT) next_x++;

    if (next_x < 0 || next_x >= COLUMN || next_y < 0 || next_y >= LINE) {
        return '#';
    }

    char next_map_char = stage_three_map_template[next_y][next_x];

    if (next_map_char == '#') {
        return next_map_char;
    }

    player->x = next_x;
    player->y = next_y;

    return next_map_char;
}


void printMapStageThree(
    char **allocated_map,
    Player *player,
    int terminal_states[],
    StageThreeSentence sentences[],
    int *current_sentence_index,
    int current_camera_index,
    int robot_red_y,
    int robot_green_y,
    int *is_running
) {
    (void)allocated_map;
    (void)is_running;

    screenClear();

    const int ROBOT_RED_FIXED_X   = 29;
    const int ROBOT_GREEN_FIXED_X = 5;

    int offsetX = (MAXX - COLUMN - 10) / 2;
    int offsetY = (MAXY - LINE) / 2;

    int all_terminals_solved = 1;
    for (int i = 0; i < NUM_TERMINALS; i++) {
        if (terminal_states[i] != 1) {
            all_terminals_solved = 0;
            break;
        }
    }

    for (int map_y = 0; map_y < LINE; map_y++) {
        screenGotoxy(offsetX + 1, offsetY + map_y + 1);

        for (int map_x = 0; map_x < COLUMN; map_x++) {
            char base_map_char = stage_three_map_template[map_y][map_x];
            char char_to_print = base_map_char;

            int is_player_position      = (map_x == player->x && map_y == player->y);
            int is_robot_red_position   = (map_x == ROBOT_RED_FIXED_X   && map_y == robot_red_y);
            int is_robot_green_position = (map_x == ROBOT_GREEN_FIXED_X && map_y == robot_green_y);
            int is_any_robot_position   = is_robot_red_position || is_robot_green_position;

            if (is_player_position && is_any_robot_position) {
                char_to_print = 'O';
                screenSetColor(RED, WHITE);
            } else if (is_player_position) {
                char_to_print = 'O';
                screenSetColor(GREEN, BLACK);
            } else if (is_robot_red_position) {
                char_to_print = 'R';
                screenSetColor(WHITE, BLACK);
            } else if (is_robot_green_position) {
                char_to_print = 'G';
                screenSetColor(MAGENTA, BLACK);
            } else {
                char_to_print = base_map_char;

                if (char_to_print == '#') {
                    screenSetColor(BLUE, BLUE);
                } else if (char_to_print == 'S') {
                    screenSetColor(BLUE, all_terminals_solved ? GREEN : RED);
                } else if (char_to_print >= 'A' && char_to_print <= 'D') {
                    int terminal_index = char_to_print - 'A';
                    if (terminal_states[terminal_index] == 1)
                        screenSetColor(BLUE, GREEN);
                    else
                        screenSetColor(RED, YELLOW);
                } else if (char_to_print == 'T') {
                    screenSetColor(CYAN, BLACK);
                } else if (char_to_print == 'P') {
                    screenSetColor(CYAN, WHITE);
                } else if (char_to_print == 'W' ||
                           char_to_print == 'X' ||
                           char_to_print == 'Y' ||
                           char_to_print == 'Z') {
                    screenSetColor(BLUE, BROWN);
                } else if (char_to_print == 'w') {
                    screenSetColor(
                        current_camera_index == 0 ? RED : BLACK,
                        current_camera_index == 0 ? RED : BLACK
                    );
                } else if (char_to_print == 'x') {
                    screenSetColor(
                        current_camera_index == 1 ? RED : BLACK,
                        current_camera_index == 1 ? RED : BLACK
                    );
                } else if (char_to_print == 'y') {
                    screenSetColor(
                        current_camera_index == 2 ? RED : BLACK,
                        current_camera_index == 2 ? RED : BLACK
                    );
                } else if (char_to_print == 'z') {
                    screenSetColor(
                        current_camera_index == 3 ? RED : BLACK,
                        current_camera_index == 3 ? RED : BLACK
                    );
                } else {
                    screenSetColor(BLACK, BLACK);
                    char_to_print = ' ';
                }
            }

            putchar(char_to_print);

            if (base_map_char == 'S' && all_terminals_solved && is_player_position) {
                player->win = 1;
            }
        }
    }


    char terminal_labels[] = {'A', 'B', 'C', 'D'};
    for (int i = 0; i < NUM_TERMINALS; i++) {
        screenGotoxy(offsetX + COLUMN + 4, offsetY + LINE - 6 + i);
        if (terminal_states[i] == 1) {
            screenSetColor(BLACK, GREEN);
            printf("Terminal %c: OK ", terminal_labels[i]);
        } else {
            screenSetColor(BLACK, BROWN);
            printf("Terminal %c: -- ", terminal_labels[i]);
        }
    }

    screenGotoxy(offsetX + COLUMN + 4, offsetY + LINE - 6 + NUM_TERMINALS);
    if (all_terminals_solved) {
        screenSetColor(GREEN, GREEN);
        printf("EXIT OPENED! ");
    } else {
        screenSetColor(BLUE, BLUE);
        printf("EXIT LOCKED. ");
    }

    int boxX = offsetX + COLUMN + 4;
    int boxY = offsetY + 2;

    screenSetColor(BLACK, BLUE);

    screenGotoxy(boxX, boxY);
    printf("+------------------------------+");
    screenGotoxy(boxX, boxY + 1);
    printf("| MAP GUIDE                   |");
    screenGotoxy(boxX, boxY + 2);
    printf("| R: Red Robot  (hit = -20)   |");
    screenGotoxy(boxX, boxY + 3);
    printf("| G: Green Robot(hit = -20)   |");
    screenGotoxy(boxX, boxY + 4);
    printf("| Cameras: -7 pts if seen     |");
    screenGotoxy(boxX, boxY + 5);
    printf("| Terminals A/B/C/D -> 0 or 1 |");
    screenGotoxy(boxX, boxY + 6);
    printf("| T: Teleport                 |");
    screenGotoxy(boxX, boxY + 7);
    printf("| S: Stage exit               |");
    screenGotoxy(boxX, boxY + 8);
    printf("+------------------------------+");

    screenUpdate();
}


void openTerminalStageThree(
    int terminal_states[],
    char terminal_position,
    StageThreeSentence sentences[],
    int *current_sentence_index,
    Player *player
) {
    if (terminal_position < 'A' || terminal_position > 'D') return;

    int terminal_index = terminal_position - 'A';

    int is_correct = openTerminalTruthTableModelStageThree(sentences, terminal_position);

    if (is_correct) {
        terminal_states[terminal_index] = 1;
        player->score += 10;
    } else {
        player->score -= 5;
        if (player->score < 0) player->score = 0;
        (void)current_sentence_index; 
    }
}


int openTerminalTruthTableModelStageThree(
    StageThreeSentence sentences[],
    char terminal_position
) {
    if (terminal_position < 'A' || terminal_position > 'D') return 0;

    int sentence_index = terminal_position - 'A';
    int user_outputs[TRUTH_ROWS_ABC];

    keyboardDestroy();
    screenClear();
    screenGotoxy(1, 1);
    screenSetColor(BLACK, BLUE);

    printf("Terminal %c\n", terminal_position);
    printf("| A | B | C |   %s  |\n", sentences[sentence_index].sentence);
    printf("----------------------\n");

    for (int i = 0; i < TRUTH_ROWS_ABC; i++) {
        int value_a = sentences[sentence_index].inputs[i][0];
        int value_b = sentences[sentence_index].inputs[i][1];
        int value_c = sentences[sentence_index].inputs[i][2];

        printf("| %d | %d |   %d   | ", value_a, value_b, value_c);
        fflush(stdout);

        int input_char;
        do {
            input_char = getchar();
        } while (input_char != '0' && input_char != '1' && input_char != EOF);

        if (input_char == EOF) input_char = '0';

        user_outputs[i] = input_char - '0';
        fflush(stdout);

        int tmp;
        while ((tmp = getchar()) != '\n' && tmp != EOF) {}
    }

    int is_correct = 1;
    for (int i = 0; i < TRUTH_ROWS_ABC; i++) {
        if (user_outputs[i] != sentences[sentence_index].outputs[i]) {
            is_correct = 0;
            break;
        }
    }

    printf("\n\nPress ENTER to continue...");
    int tmp;
    while ((tmp = getchar()) != '\n' && tmp != EOF) {}

    keyboardInit();
    return is_correct;
}
