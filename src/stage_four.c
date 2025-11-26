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
    int terminals[NUM_TERMINALS]; // A, B, C, D
} StageFourSentence;

char stage_Four_map_template[LINE][COLUMN + 1] = {
    "########################################",
    "#O.............................A.......#", 
    "####.######################...##.#######",
    "##..x......r..r..x...........r.r...D####", 
    "##.#####.######X#########.#######r...###",
    "##.#####...x.r...r..##..F....B....r.####", 
    "##.############.##########.####y########",
    "#...............r..###.......##y########",
    "#o###r##.#####################yY########", 
    "C..r....w...................yyyyyyyyy..#", 
    "######.#################.############.##", 
    "######...#######w..........P..........##",
    "########w###################z########.##",
    "###..wwwwwww############Zzzzzzzzzz###.##",
    "########wW##################z#######.###",
    "######wwwwwww................###########",
    "##################.#####################",
    "#####.T...............................##", 
    "####################S###################"
};

StageFourSentence stage_Four_sentences[NUM_SENTENCES] = {
    {"(A -> ¬C) ^ (B <-> D)", {1, 0, 1, 0}}, 
    {"(A <-> B) -> (C -> D)", {1, 1, 0, 0}}, 
    {"(A ^ (B -> C)) -> D", {0, 1, 1, 0}}
};

void printMapStageFour(
    char **map,
    Player *player,
    int terminals[],
    StageFourSentence sentences[],
    int *current_sentence_index,
    int currentCamera,
    int robot_r_y,
    int robot_g_y, 
    int *running
);
void setupStageFour(
    Player *player,
    char **allocated_map,
    int terminals[],
    char stage_map[LINE][COLUMN + 1]
);
void openTerminalStageFour(
    int terminals[],
    char terminal,
    StageFourSentence sentences[],
    int *current_sentence_index,
    Player *player
);
int open_terminal_modelStageFour(char terminal);
void resetIndexStageFour(int *item, int total);
char movePlayer_stageFour(
    MoveDirection direction,
    char **map,
    int *running,
    Player *player,
    char *lastChar,
    int terminals[],
    int *current_sentence_index
);


int stage_four(char **allocated_map, Player *player, int *penalty) {
    const int ROBOT_R_FIXED_X = 29; 
    const int ROBOT_R_START_Y = 3; 
    const int ROBOT_R_END_Y = 5;   
    
    const int ROBOT_G_FIXED_X = 5;
    const int ROBOT_G_START_Y = 8; 
    const int ROBOT_G_END_Y = 11;  
    
    const int TELEPORT_EXIT_X = 27;
    const int TELEPORT_EXIT_Y = 11;

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

    setupStageFour(player, allocated_map, terminals, stage_Four_map_template);

    int stageTime = 120000;
    timerInit(stageTime);
    lastCameraSwitch = getTimeDiff(); 
    lastRobotMove = getTimeDiff();


    while (1) {
        printMapStageFour(allocated_map, player, terminals, stage_Four_sentences,
                           &current_sentence_index, currentCamera, robot_r_y, robot_g_y, &running);

        while (running) {
            if (keyhit()) {
                ch = readch();

                if (ch == 'w') requiredPosition = movePlayer_stageFour(UP, allocated_map, &running, player, &lastChar, terminals, &current_sentence_index);
                else if (ch == 's') requiredPosition = movePlayer_stageFour(DOWN, allocated_map, &running, player, &lastChar, terminals, &current_sentence_index);
                else if (ch == 'a') requiredPosition = movePlayer_stageFour(LEFT, allocated_map, &running, player, &lastChar, terminals, &current_sentence_index);
                else if (ch == 'd') requiredPosition = movePlayer_stageFour(RIGHT, allocated_map, &running, player, &lastChar, terminals, &current_sentence_index);
                else if (ch == 'l') { running = 0; player->win = 0; }

                openTerminalStageFour(terminals, requiredPosition, stage_Four_sentences, &current_sentence_index, player);

                if (requiredPosition == 'T') { 
                     player->x = TELEPORT_EXIT_X;
                     player->y = TELEPORT_EXIT_Y;
                     
                     requiredPosition = stage_Four_map_template[player->y][player->x]; 
                }

                printMapStageFour(allocated_map, player, terminals, stage_Four_sentences,
                                   &current_sentence_index, currentCamera, robot_r_y, robot_g_y, &running);
            }

            int currentTime = getTimeDiff();

            if (currentTime - lastCameraSwitch >= 3000) {
                resetIndexStageFour(&currentCamera, NUM_CAMERAS); 
                lastCameraSwitch = currentTime;
                printMapStageFour(allocated_map, player, terminals, stage_Four_sentences,
                                   &current_sentence_index, currentCamera, robot_r_y, robot_g_y, &running);
            }

            if (currentTime - lastRobotMove >= 500) {
                
                int next_r_y = robot_r_y + robot_r_direction;
                
                if (next_r_y < ROBOT_R_START_Y || next_r_y > ROBOT_R_END_Y || stage_Four_map_template[next_r_y][robot_r_x] == '#') {
                    robot_r_direction *= -1; 
                }
                robot_r_y += robot_r_direction; 

                int next_g_y = robot_g_y + robot_g_direction;

                if (next_g_y < ROBOT_G_START_Y || next_g_y > ROBOT_G_END_Y || stage_Four_map_template[next_g_y][robot_g_x] == '#') {
                    robot_g_direction *= -1; 
                }
                robot_g_y += robot_g_direction; 

                lastRobotMove = currentTime;

                printMapStageFour(allocated_map, player, terminals, stage_Four_sentences,&current_sentence_index, currentCamera, robot_r_y, robot_g_y, &running);
            }


            char player_pos_char = stage_Four_map_template[player->y][player->x];

            if ((player->x == robot_r_x && player->y == robot_r_y) || (player->x == robot_g_x && player->y == robot_g_y)) {
                *penalty -= 20;
                player->x = 1; 
                player->y = 1; 
                resetIndexStageFour(&current_sentence_index, NUM_SENTENCES);
                for(int i = 0; i < NUM_TERMINALS; i++) terminals[i] = -1;
            }
                        
            int caughtByProjection = (player_pos_char == 'w' && currentCamera == 0) ||(player_pos_char == 'x' && currentCamera == 1) ||(player_pos_char == 'y' && currentCamera == 2) || (player_pos_char == 'z' && currentCamera == 3);
            
            int caughtByFixedUnit = (player_pos_char == 'W' || player_pos_char == 'X' || player_pos_char == 'Y' || player_pos_char == 'Z');

            if (caughtByProjection || caughtByFixedUnit) {
                *penalty -= 7;
                
                player->x = 1;
                player->y = 1;

                resetIndexStageFour(&currentCamera, NUM_CAMERAS);
                resetIndexStageFour(&current_sentence_index, NUM_SENTENCES);
                for(int i = 0; i < NUM_TERMINALS; i++) terminals[i] = -1;
            }

            if (player_pos_char == 'o') {
                player->x = 1;
                player->y = 1;
            }

            if (player_pos_char == 'F') {
                player->x = 1; 
                player->y = 1; 
                resetIndexStageFour(&current_sentence_index, NUM_SENTENCES);
                for(int i = 0; i < NUM_TERMINALS; i++) terminals[i] = -1;
            }
        }

        if (!running) break;
    }

    return 0;
}


void printMapStageFour(
    char **map,
    Player *player,
    int terminals[],
    StageFourSentence sentences[],
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
    for(int i = 0; i < NUM_TERMINALS; i++) {
        if (terminals[i] != sentences[*current_sentence_index].terminals[i]) {
            all_solved = 0;
            break;
        }
    }

    for (int y = 0; y < LINE; y++) {
        screenGotoxy(offsetX + 1, offsetY + y + 1);

        for (int x = 0; x < COLUMN; x++) {
            
            char map_char = stage_Four_map_template[y][x];
            char char_to_print = map_char;
            
            int is_player_pos = (x == player->x && y == player->y);
            int is_robot_r_pos = (x == ROBOT_R_FIXED_X && y == robot_r_y);
            int is_robot_g_pos = (x == ROBOT_G_FIXED_X && y == robot_g_y);
            
            int is_robot_pos = is_robot_r_pos || is_robot_g_pos;
            

            if (is_player_pos && is_robot_pos) {
                char_to_print = 'O'; 
                screenSetColor(RED, WHITE);
            } 
            else if (is_player_pos) {
                char_to_print = 'O';
                screenSetColor(GREEN, BLACK); 
            } 
            else if (is_robot_r_pos) {
                char_to_print = 'R'; 
                screenSetColor(WHITE, BLACK);
            }
            else if (is_robot_g_pos) {
                char_to_print = 'G'; 
                screenSetColor(MAGENTA, BLACK); 
            } 
            else {
                char_to_print = map_char;
                
                if (char_to_print == '#') {
                    screenSetColor(BLUE, BLUE);
                } else if (char_to_print == 'S') {
                    screenSetColor(BLUE, all_solved ? GREEN : RED);
                } else if (char_to_print >= 'A' && char_to_print <= 'D') { 
                    int index = char_to_print - 'A';
                    screenSetColor(terminals[index] == -1 ? RED : BLUE, terminals[index] == -1 ? YELLOW : GREEN);
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
                } 
                else if (char_to_print == 'o') {
                    screenSetColor(BLACK, WHITE); 
                } else if (char_to_print == 'F') {
                     screenSetColor(WHITE, RED); 
                } else {
                     screenSetColor(BLACK, BLACK);
                     char_to_print = ' '; 
                }
            }
            
            printf("%c", char_to_print);
            
            if (map_char == 'S' && all_solved && is_player_pos) {
               player->win = 1; 
               *running = 0; 
            }
        }
    }

    screenSetColor(BLACK, BLUE);
    screenGotoxy(MAXX - 15, MAXY - 18);
    printf("Senha: %s", sentences[*current_sentence_index].sentence);


    char terminal_labels[] = {'A', 'B', 'C', 'D'};
    for (int i = 0; i < NUM_TERMINALS; i++) {
        screenGotoxy(MAXX - 15, MAXY - 16 + i);
        if (terminals[i] == sentences[*current_sentence_index].terminals[i]) {
            screenSetColor(BLACK, GREEN);
            printf("Terminal %c: OK ", terminal_labels[i]);
        } else {
            screenSetColor(BLACK, BROWN);
            printf("Terminal %c: -- ", terminal_labels[i]);
        }
    }

    screenGotoxy(MAXX - 15, MAXY - 16 + NUM_TERMINALS);
    if (all_solved) {
        screenSetColor(GREEN, GREEN);
        printf("EXIT OPENED! ");
    } else {
        screenSetColor(BLUE, BLUE);
        printf("EXIT LOCKED. ");
    }

    screenUpdate();
}


void setupStageFour(
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

void resetIndexStageFour(int *current_index, int total) {
    *current_index = (*current_index + 1) % total;
    screenUpdate();
}

char movePlayer_stageFour(
    MoveDirection direction,
    char **map,
    int *running,
    Player *player,
    char *lastChar,
    int terminals[],
    int *current_sentence_index
) {
    int x = player->x;
    int y = player->y;

    if (direction == UP) y--;
    else if (direction == DOWN) y++;
    else if (direction == LEFT) x--;
    else if (direction == RIGHT) x++;

    char next = stage_Four_map_template[y][x];

    if (next == '#') {
        return next;
    }

    if (next == 'S') {
      int isOpen =  terminals[0] == stage_Four_sentences[*current_sentence_index].terminals[0] &&
                    terminals[1] == stage_Four_sentences[*current_sentence_index].terminals[1] &&
                    terminals[2] == stage_Four_sentences[*current_sentence_index].terminals[2] &&
                    terminals[3] == stage_Four_sentences[*current_sentence_index].terminals[3];
    
      if (isOpen) *running = 0;
    }
    
    player->x = x;
    player->y = y;

    return next;
}

void openTerminalStageFour(int terminals[], char requiredTerminal,
StageFourSentence sentences[], int *current_sentence_index, Player *player) {

    int index = requiredTerminal - 'A';

    if (index >= 0 && index < NUM_TERMINALS) {
        int value = open_terminal_modelStageFour(requiredTerminal);

        if (value == sentences[*current_sentence_index].terminals[index]) {
            terminals[index] = value;
        } else {
            resetIndexStageFour(current_sentence_index, NUM_SENTENCES);
            for(int i = 0; i < NUM_TERMINALS; i++) terminals[i] = -1;
            screenSetColor(RED, RED);
        }
    }
}

int open_terminal_modelStageFour(char terminal) {
    int terminal_value = -1;

    keyboardDestroy();

    screenClear();
    screenGotoxy(1,1);
    screenSetColor(BLACK, BLUE);
    printf("Digite o valor do terminal %c (0 ou 1): ", terminal);
    fflush(stdout);

    char ch;
    do {
        ch = getchar();
    } while (ch != '0' && ch != '1');

    terminal_value = ch - '0';

    printf("%d\n", terminal_value);
    fflush(stdout);

    getchar(); 

    keyboardInit();

    return terminal_value;
}
