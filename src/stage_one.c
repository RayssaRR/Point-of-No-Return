#include <stdio.h>
#include <string.h>
#include "screen.h"
#include "timer.h"
#include "keyboard.h"
#include "utils.h"

typedef struct {
  char sentence[200];
  int terminals[3];
} SentenceModel;

void printMap(
  char **map,
  Player *player,
  int terminals[],
  int *camera,
  SentenceModel sentences[],
  int *current_sentence_index,
  int currentCamera
);
void setupStage(
  Player *player,
  char **allocated_map,
  int terminals[],
  int *camera,
  char stage_map[LINE][COLUMN + 1]
);
void openTerminal(int terminals[], char terminal, SentenceModel sentences[], int *current_sentence_index);
int open_terminal_model(char terminal);
void resetIndex(int *item, int total);
char movePlayer(
  MoveDirection direction,
  char **map,
  int *running,
  Player *player,
  char *lastChar
);

char stage_one_map[LINE][COLUMN + 1] = {
    "########################################",
    "#O.....................................#",
    "####.###########################.#######",
    "##.........xxxxxxxxx......######.....###",
    "########.######X####################.###",
    "########...xxxxxxx.##.................B#",
    "###############.##########.####y########",
    "#..................###.......##y########",
    "#########.#####################yY#######",
    "A..........................yyyyyyyyy...#",
    "######.#################.###############",
    "######...#######......................##",
    "########w###################z########.##",
    "###..wwwwwww############Zzzzzzzzzz###.##",
    "########wW##################z#######C.##",
    "######wwwwwww................###########",
    "##################.#####################",
    "######...............................###",
    "####################S###################"
};

SentenceModel sentences[3] = {
  {"A ∧ B ∧ C", {1, 1, 1}},
  {"A ∧ B ∧ ¬ C", {1, 1, 0}},
  {"A ∧ ¬B ∧ C", {1, 0, 1}}
};

int stage_one(char **allocated_map, Player *player) {
  player->x = 1;
  player->y = 1;

  int current_sentence_index = 0;

  static int ch = 0;
  int currentCamera = 0;
  int running = 1;
  int camera = 0;
  int terminals[3] = {-1, -1, -1};
  char requiredPosition = '.';
  char lastChar = '.';

  keyboardInit();
  screenInit(0);

  while (1) {
    setupStage(player, allocated_map, terminals, &camera, stage_one_map);
    printMap(allocated_map, player, terminals, &camera, sentences, &current_sentence_index, currentCamera);
    timerInit(3000);

    while (running) {
      if (keyhit()) {
        ch = readch();

        if (ch == 'w') {
            requiredPosition = movePlayer(UP, allocated_map, &running, player, &lastChar);
        } else if (ch == 's') {
            requiredPosition = movePlayer(DOWN, allocated_map, &running, player, &lastChar);
        } else if (ch == 'a') {
            requiredPosition = movePlayer(LEFT, allocated_map, &running, player, &lastChar);
        } else if (ch == 'd') {
            requiredPosition = movePlayer(RIGHT, allocated_map, &running, player, &lastChar);
        } else if (ch == 'l') {
            running = 0;
            player->win = 0;
        }

        openTerminal(terminals, requiredPosition, sentences, &current_sentence_index);
        printMap(allocated_map, player, terminals, &camera, sentences, &current_sentence_index, currentCamera);

      }

      if (timerTimeOver() == 1) {
        resetIndex(&currentCamera, 4);
        printMap(allocated_map, player, terminals, &camera, sentences, &current_sentence_index, currentCamera);
      }

      int caughtByW = (stage_one_map[player->y][player->x] == 'w' && currentCamera == 0);
      int caughtByX = (stage_one_map[player->y][player->x] == 'x' && currentCamera == 1);
      int caughtByY = (stage_one_map[player->y][player->x] == 'y' && currentCamera == 2);
      int caughtByZ = (stage_one_map[player->y][player->x] == 'z' && currentCamera == 3);
      if (caughtByW || caughtByX || caughtByY || caughtByZ) {
        resetIndex(&currentCamera, 4);
        resetIndex(&current_sentence_index, 2);
        printMap(allocated_map, player, terminals, &camera, sentences, &current_sentence_index, currentCamera);
      } 
    }

    if (running == 0) break;
  }

  return 0;
}

void openTerminal(int terminals[], char requiredTerminal, SentenceModel sentences[], int *current_sentence_index) {
    int index = requiredTerminal - 'A';
    
    if (index >= 0 && index < 3) { 
        int value = open_terminal_model(requiredTerminal);
        if (value == sentences[*current_sentence_index].terminals[index]) {
            terminals[index] = value;
        } else {
          resetIndex(current_sentence_index, 2);
          terminals[0] = -1;
          terminals[1] = -1;
          terminals[2] = -1;
          screenSetColor(RED, RED);
        }
    }
}

int open_terminal_model(char terminal) {
  int terminal_value = -1;
  screenClear(); // Limpando a tela para garantir que não há texto residual

  int offsetX = 1; // Tentando centralizar a mensagem na tela
  int offsetY = 1;

  screenGotoxy(offsetX, offsetY);
  screenSetColor(BLACK, BLUE); // Move o cursor para a posição calculada
  printf("Digite o valor do terminal %c:", terminal);

  screenUpdate();  // Atualizando a tela para refletir as mudanças

  keyboardDestroy(); // Desativar captura de teclas temporariamente

  screenGotoxy(offsetX, offsetY + 1); // Move o cursor para a posição calculada
  scanf("%d", &terminal_value);

  keyboardInit(); // Reinicializa captura de teclas após a entrada do nome
  screenUpdate();  // Atualizando a tela para refletir as mudanças

  return terminal_value;
}

void printMap(
  char **map,
  Player *player,
  int terminals[],
  int *camera,
  SentenceModel sentences[],
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
            } else if (ch == 'O') {
                screenSetColor(GREEN, BLACK);
            } else if (ch == 'S') {
                if (terminals[0] == sentences[*current_sentence_index].terminals[0] &&
                    terminals[1] == sentences[*current_sentence_index].terminals[1] &&
                    terminals[2] == sentences[*current_sentence_index].terminals[2]) {
                  screenSetColor(BLUE, GREEN);
                } else {
                  screenSetColor(BLUE, RED);
                }
            } else if (ch == 'A') {
                if (terminals[0] == -1) {
                  screenSetColor(RED, YELLOW);
                } else {
                  screenSetColor(BLUE, GREEN);
                }
            } else if (ch == 'B') {
                if (terminals[1] == -1) {
                  screenSetColor(RED, YELLOW);
                } else {
                  screenSetColor(BLUE, GREEN);
                }
            } else if (ch == 'C') {
                if (terminals[2] == -1) {
                  screenSetColor(RED, YELLOW);
                } else {
                  screenSetColor(BLUE, GREEN);
                }
            } else if (ch == 'W' || ch == 'X' || ch == 'Y' || ch == 'Z') {
                screenSetColor(BLUE, BROWN);
            } else if (ch == 'w') {
              if (currentCamera == 0) {
                screenSetColor(RED, RED);
              } else {
                screenSetColor(BLACK, BLACK);
              }
            } else if (ch == 'x') {
              if (currentCamera == 1) {
                screenSetColor(RED, RED);
              } else {
                screenSetColor(BLACK, BLACK);
              }
            } else if (ch == 'y') {
              if (currentCamera == 2) {
                screenSetColor(RED, RED);
              } else {
                screenSetColor(BLACK, BLACK);
              }
            } else if (ch == 'z') {
              if (currentCamera == 3) {
                screenSetColor(RED, RED);
              } else {
                screenSetColor(BLACK, BLACK);
              }
            } else {
                screenSetColor(BLACK, BLACK);
            }
            printf("%c", ch);
        }
    }
    
    screenGotoxy(MAXX - 15, MAXY - 19);
    screenSetColor(BLACK, BLUE);
    printf("Score: %.2f", player->score);
    screenGotoxy(MAXX - 15, MAXY - 18);
    printf("Senha: %s", sentences[*current_sentence_index].sentence);
    screenGotoxy(MAXX - 15, MAXY - 17);

    screenGotoxy(MAXX - 15, MAXY - 16);
    if (terminals[0] == sentences[*current_sentence_index].terminals[0]) {
        screenSetColor(BLACK, GREEN);
        printf("Terminal A: OK ");
    } else {
        screenSetColor(BLACK, BROWN);
        printf("Terminal A: -- ");
    }

    screenGotoxy(MAXX - 15, MAXY - 15);
    if (terminals[1] == sentences[*current_sentence_index].terminals[1]) {
        screenSetColor(BLACK, GREEN);
        printf("Terminal B: OK ");
    } else {
        screenSetColor(BLACK, BROWN);
        printf("Terminal B: -- ");
    }

    screenGotoxy(MAXX - 15, MAXY - 14);
    if (terminals[2] == sentences[*current_sentence_index].terminals[2]) {
        screenSetColor(BLACK, GREEN);
        printf("Terminal C: OK ");
    } else {
        screenSetColor(BLACK, BROWN);
        printf("Terminal C: -- ");
    }

    if (terminals[0] == sentences[*current_sentence_index].terminals[0] &&
        terminals[1] == sentences[*current_sentence_index].terminals[1] &&
        terminals[2] == sentences[*current_sentence_index].terminals[2]) {
      screenSetColor(GREEN, GREEN);
    } else {
      screenSetColor(BLUE, BLUE);
    }
    screenUpdate();
}

void setupStage(
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

void resetIndex(int *current_sentence_index, int total_sentences) {
    *current_sentence_index = (*current_sentence_index + 1) % total_sentences;
    screenUpdate();
}

char movePlayer(
  MoveDirection direction,
  char **map,
  int *running,
  Player *player,
  char *lastChar
) {

  int x = player->x;
  int y = player->y;

  if (direction == UP) {
    y = player->y - 1;
    x = player->x;
  } else if (direction == DOWN) {
    y = player->y + 1;
    x = player->x;
  } else if (direction == LEFT) {
    y = player->y;
    x = player->x - 1;
  } else if (direction == RIGHT) {
    y = player->y;
    x = player->x + 1;
  }

  if (map[y][x] == '.' || map[y][x] == 'S' || map[y][x] == 'x' || map[y][x] == 'w' || map[y][x] == 'y' || map[y][x] == 'z') {
    if (map[y][x] == 'S') {
      player->win = 1;
      *running = 0;
    }

    *lastChar = stage_one_map[player->y][player->x] == 'O' ? '.' : stage_one_map[player->y][player->x];

    map[player->y][player->x] = *lastChar;

    player->x = x;
    player->y = y;
    map[player->y][player->x] = 'O';
  }

  return map[y][x];
}