#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "player.h"
#include "screen.h"
#include "timer.h"
#include "keyboard.h"

#define LINE 19
#define COLUMN 40

typedef struct {
    char sentence[200];
    int terminals[3];
} StageTwoSentence;

char stage_two_map[LINE][COLUMN + 1] = {
    "########################################",
    "#........................o.............#",            
    "####.###########################.#######",
    "##.........xxxxxxxxx......######.....###",
    "##.#####.######X#########.##########.###",
    "##.#####...xxxxxxx.##....^............B#",
    "##.############.##########.####y########",
    "#..................###..^....##y########",
    "#o#######.#####################yY#######",
    "A.........................yyyyyyyyy...#",
    "######.#################.############^##",
    "######...#######......................##",
    "########w###################z########.##",
    "###..wwwwwww############Zzzzzzzzzz###.##",
    "########wW##################z#######C.##",
    "######wwwwwww................###########",
    "########^#########.#######o#############",
    "######...............................###",
    "####################S###################"
};

StageTwoSentence stage_two_sentences[3] = {
    {"(A ∧ ¬B) ∧ ¬C", {1,0,0}},
    {"(A ∧ ¬B) ∧ ¬C", {1,0,0}},
    {"(A ∧ ¬B) ∧ ¬C", {1,0,0}}
};

void printMapStageTwo(
    char **map,
    Player *player,
    int terminals[],
    int *camera,
    StageTwoSentence stage_two_sentences[],
    int *current_sentence_index,
    int currentCamera
);

void setupStageTwo(
    Player *player,
    char **allocated_map,
    int terminals[],
    int *camera,
    char stage_map[LINE][COLUMN + 1]
);

void openTerminalStageTwo(int terminals[], char terminal, StageTwoSentence stage_two_sentences[], int *current_sentence_index, Player *player);
int open_terminal_modelStageTwo(char terminal);
void resetIndexStageTwo(int *item, int total);

char movePlayer_stageTwo(
    MoveDirection direction,
    char **map,
    int *running,
    Player *player,
    char *lastChar,
    int terminals[],
    int *current_sentence_index,
    int *penalty
) {
    int x = player->x;
    int y = player->y;


    if (direction == UP) y--;
    else if (direction == DOWN) y++;
    else if (direction == LEFT) x--;
    else if (direction == RIGHT) x++;

    char next = stage_two_map[y][x];


    if (next == '#') {
        return next;
    }

    map[player->y][player->x] = stage_two_map[player->y][player->x];

    *lastChar = next;

    if (next == '^') {
        *penalty -= 5;

        stage_two_map[y][x] = '.';
        map[y][x] = '.';

        player->x = x;
        player->y = y;
        map[y][x] = 'O';

        return next;
    }

    if (next == 'o') {
        map[player->y][player->x] = stage_two_map[player->y][player->x];

        player->x = 1;
        player->y = 1;

        map[player->y][player->x] = 'O';
        return next;
    }

    player->x = x;
    player->y = y;
    map[y][x] = 'O';

    
    if (next == 'S') {
      int isOpen =  terminals[0] == stage_two_sentences[*current_sentence_index].terminals[0] &&
                    terminals[1] == stage_two_sentences[*current_sentence_index].terminals[1] &&
                    terminals[2] == stage_two_sentences[*current_sentence_index].terminals[2];
    
      if (isOpen) {
        *running = 0;
      }
    }

    return next;
}



int stage_two(char **allocated_map, Player *player, int *penalty) {

    player->x = 1;
    player->y = 1;

    int current_sentence_index = 0;
    int running = 1;
    int camera = 0;
    int currentCamera = 0;
    int terminals[3] = {-1, -1, -1};

    char requiredPosition = '.';
    char lastChar = '.';
    int ch = 0;
    int lastCameraSwitch = 0;
    

    keyboardInit();
    screenInit(0);

    
    setupStageTwo(player, allocated_map, terminals, &camera, stage_two_map);

    allocated_map[player->y][player->x] = 'O';  

    int stageTime = 60000; 
    timerInit(stageTime);  
    lastCameraSwitch = getTimeDiff();  


    // loop principal
    while (1) {

        printMapStageTwo(allocated_map, player, terminals, &camera, stage_two_sentences,
                 &current_sentence_index, currentCamera);



        while (running) {
            if (keyhit()) {
                ch = readch();

                    //movimento do jogador
                if (ch == 'w') requiredPosition = movePlayer_stageTwo(UP, allocated_map, &running, player, &lastChar, terminals, &current_sentence_index, penalty);
                else if (ch == 's') requiredPosition = movePlayer_stageTwo(DOWN, allocated_map, &running, player, &lastChar, terminals, &current_sentence_index, penalty);
                else if (ch == 'a') requiredPosition = movePlayer_stageTwo(LEFT, allocated_map, &running, player, &lastChar, terminals, &current_sentence_index, penalty);
                else if (ch == 'd') requiredPosition = movePlayer_stageTwo(RIGHT, allocated_map, &running, player, &lastChar, terminals, &current_sentence_index, penalty);
                else if (ch == 'l') { running = 0; }
               
               openTerminalStageTwo(terminals, requiredPosition, stage_two_sentences, &current_sentence_index, player);

                printMapStageTwo(allocated_map, player, terminals, &camera,
                         stage_two_sentences, &current_sentence_index,
                         currentCamera);
            }


           // tempo atual desde o último timerInit(stageTime)
            int currentTime = getTimeDiff();

            if (currentTime - lastCameraSwitch >= 3000) {
                resetIndexStageTwo(&currentCamera, 4);

                printMapStageTwo(allocated_map, player, terminals,
                                &camera, stage_two_sentences,
                                &current_sentence_index,
                                currentCamera);

                lastCameraSwitch = currentTime; // marca a hora da troca
            }

            // detectar câmeras
            int caughtByW = (stage_two_map[player->y][player->x] == 'w' && currentCamera == 0);
            int caughtByX = (stage_two_map[player->y][player->x] == 'x' && currentCamera == 1);
            int caughtByY = (stage_two_map[player->y][player->x] == 'y' && currentCamera == 2);
            int caughtByZ = (stage_two_map[player->y][player->x] == 'z' && currentCamera == 3);

            if (caughtByW || caughtByX || caughtByY || caughtByZ) {
                *penalty -= 7;
                resetIndexStageTwo(&currentCamera, 4);
                resetIndexStageTwo(&current_sentence_index, 3);
                resetIndexStageTwo(terminals, 3);
            }
        }

        if (!running) break;
    }

    return 0;
}
void openTerminalStageTwo(int terminals[], char requiredTerminal,
StageTwoSentence sentences[], int *current_sentence_index, Player *player) {

int index = requiredTerminal - 'A';  // A=0, B=1, C=2

if (index >= 0 && index < 3) {
    int value = open_terminal_modelStageTwo(requiredTerminal);

    if (value == sentences[*current_sentence_index].terminals[index]) {
        terminals[index] = value;
    } else {
        resetIndexStageTwo(current_sentence_index, 3);
        terminals[0] = -1;
        terminals[1] = -1;
        terminals[2] = -1;
        screenSetColor(RED, RED);
        
    }
}

}



int open_terminal_modelStageTwo(char terminal) {
    int terminal_value = -1;

    keyboardDestroy();

    screenClear();
    screenGotoxy(1,1);
    screenSetColor(BLACK, BLUE);
    printf("Digite o valor do terminal %c ", terminal);
    fflush(stdout);

    char ch;
    do {
        ch = getchar(); 
    } while (ch != '0' && ch != '1');

    terminal_value = ch - '0';

    printf("\n%d\n", terminal_value);
    fflush(stdout);

    // Espera um pouco para o jogador ver
    getchar(); // lê o Enter

    // Volta para o modo raw do jogo
    keyboardInit();

    return terminal_value;
}



void printMapStageTwo(
    char **map,
    Player *player,
    int terminals[],
    int *camera,
    StageTwoSentence stage_two_sentences[],
    int *current_sentence_index,
    int currentCamera
) {
    screenClear();

    int offsetX = (MAXX - COLUMN - 10) / 2;
    int offsetY = (MAXY - LINE) / 2;

    for (int y = 0; y < LINE; y++) {
        screenGotoxy(offsetX + 1, offsetY + y + 1);

        for (int x = 0; x < COLUMN; x++) {
            char ch = map[y][x];


            if (ch == '#') {
                screenSetColor(BLUE, BLUE);
            } 
            else if (ch == 'O') { 
                screenSetColor(GREEN, BLACK);
            }
            else if (ch == 'S') { 
                if (terminals[0] == stage_two_sentences[*current_sentence_index].terminals[0] &&
                    terminals[1] == stage_two_sentences[*current_sentence_index].terminals[1] &&
                    terminals[2] == stage_two_sentences[*current_sentence_index].terminals[2]) {
                    screenSetColor(BLUE, GREEN);
                } else {
                    screenSetColor(BLUE, RED);
                }
            }
            else if (ch == 'A') {
                screenSetColor(terminals[0] == -1 ? RED : BLUE,
                               terminals[0] == -1 ? YELLOW : GREEN);
            }
            else if (ch == 'B') {
                screenSetColor(terminals[1] == -1 ? RED : BLUE,
                               terminals[1] == -1 ? YELLOW : GREEN);
            }
            else if (ch == 'C') {
                screenSetColor(terminals[2] == -1 ? RED : BLUE,
                               terminals[2] == -1 ? YELLOW : GREEN);
            }
            else if (ch == '^') { 
                screenSetColor(RED, BLACK);
            }
            else if (ch == 'o') { 
                screenSetColor(BLACK, WHITE);
            }

          
            else if (ch == 'W' || ch == 'X' || ch == 'Y' || ch == 'Z') {
                screenSetColor(BLUE, BROWN);
            }

            else if (ch == 'w') {
                screenSetColor(currentCamera == 0 ? RED : BLACK,
                               currentCamera == 0 ? RED : BLACK);
            }
            else if (ch == 'x') {
                screenSetColor(currentCamera == 1 ? RED : BLACK,
                               currentCamera == 1 ? RED : BLACK);
            }
            else if (ch == 'y') {
                screenSetColor(currentCamera == 2 ? RED : BLACK,
                               currentCamera == 2 ? RED : BLACK);
            }
            else if (ch == 'z') {
                screenSetColor(currentCamera == 3 ? RED : BLACK,
                               currentCamera == 3 ? RED : BLACK);
            }

           
            else {
                screenSetColor(BLACK, BLACK);
            }

            printf("%c", ch);
        }
    }

   
    screenSetColor(BLACK, BLUE);

    screenGotoxy(MAXX - 15, MAXY - 18);
    printf("Senha: %s", stage_two_sentences[*current_sentence_index].sentence);

   
    screenGotoxy(MAXX - 15, MAXY - 16);
    if (terminals[0] == stage_two_sentences[*current_sentence_index].terminals[0]) {
        screenSetColor(BLACK, GREEN);
        printf("Terminal A: OK ");
    } else {
        screenSetColor(BLACK, BROWN);
        printf("Terminal A: -- ");
    }

   
    screenGotoxy(MAXX - 15, MAXY - 15);
    if (terminals[1] == stage_two_sentences[*current_sentence_index].terminals[1]) {
        screenSetColor(BLACK, GREEN);
        printf("Terminal B: OK ");
    } else {
        screenSetColor(BLACK, BROWN);
        printf("Terminal B: -- ");
    }

   
    screenGotoxy(MAXX - 15, MAXY - 14);
    if (terminals[2] == stage_two_sentences[*current_sentence_index].terminals[2]) {
        screenSetColor(BLACK, GREEN);
        printf("Terminal C: OK ");
    } else {
        screenSetColor(BLACK, BROWN);
        printf("Terminal C: -- ");
    }

    if (terminals[0] == stage_two_sentences[*current_sentence_index].terminals[0] &&
        terminals[1] == stage_two_sentences[*current_sentence_index].terminals[1] &&
        terminals[2] == stage_two_sentences[*current_sentence_index].terminals[2]) {
        screenSetColor(GREEN, GREEN);
    } else {
        screenSetColor(BLUE, BLUE);
    }

    screenUpdate();
}
void setupStageTwo(
  Player *player,
  char **allocated_map,
  int terminals[],
  int *camera,
  char stage_map[LINE][COLUMN + 1]
) {
    player->x = 1;
    player->y = 1;

    for (int i = 0; i < LINE; i++) strcpy(allocated_map[i], stage_map[i]);

    player->win = 0;

    terminals[0] = -1;
    terminals[1] = -1;
    terminals[2] = -1;
}

void resetIndexStageTwo(int *current_sentence_index, int total_sentences) {
    *current_sentence_index = (*current_sentence_index + 1) % total_sentences;
    screenUpdate();
}


