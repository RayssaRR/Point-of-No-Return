#include <stdio.h>
#include <string.h>
#include <time.h>
#include "miniaudio.h"
#include "sounds.h"
#include "screen.h"
#include "timer.h"
#include "keyboard.h"
#include "utils.h"

typedef struct
{
  char sentence[200];
  int terminals[3];
} SentenceModel;

void printStageOneIntro();
void printMap(
    char **map,
    Player *player,
    int terminals[],
    int *camera,
    SentenceModel sentences[],
    int *current_sentence_index,
    int currentCamera);
void setupStage(
    Player *player,
    char **allocated_map,
    int terminals[],
    int *camera,
    char stage_map[LINE][COLUMN + 1]);
void openTerminal(int terminals[], char terminal, SentenceModel sentences[], int *current_sentence_index, Player *player);
int open_terminal_model(char terminal);
void resetIndex(int *item, int total);
char movePlayer(
    MoveDirection direction,
    char **map,
    int *running,
    Player *player,
    char *lastChar,
    int terminals[],
    int *current_sentence_index);

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
  // O callback deve ser definido, mas não precisamos de lógica aqui para a reprodução simples.
  // O ma_data_source_read_pcm_frames cuidará da leitura dos dados.
  (void)pDevice;
  (void)pOutput;
  (void)pInput;
  (void)frameCount;
}

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
    "####################S###################"};

SentenceModel sentences[3] = {
    {"A ∧ B ∧ C", {1, 1, 1}},
    {"A ∧ B ∧ ¬(¬C)", {1, 1, 0}},
    {"A ∧ ¬(¬B) ∧ ¬C", {1, 0, 1}}};

ma_engine engine;

int stage_one(char **allocated_map, Player *player)
{
  ma_result result;

  result = ma_engine_init(NULL, &engine);
  if (result != MA_SUCCESS)
  {
    printf("Erro ao inicializar o motor de áudio (miniaudio).\n");
    return -1;
  }

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

  while (1)
  {
    setupStage(player, allocated_map, terminals, &camera, stage_one_map);
    printMap(allocated_map, player, terminals, &camera, sentences, &current_sentence_index, currentCamera);
    timerInit(3000);

    while (running)
    {
      if (keyhit())
      {
        ch = readch();

        if (ch == 'w')
        {
          requiredPosition = movePlayer(UP, allocated_map, &running, player, &lastChar, terminals, &current_sentence_index);
        }
        else if (ch == 's')
        {
          requiredPosition = movePlayer(DOWN, allocated_map, &running, player, &lastChar, terminals, &current_sentence_index);
        }
        else if (ch == 'a')
        {
          requiredPosition = movePlayer(LEFT, allocated_map, &running, player, &lastChar, terminals, &current_sentence_index);
        }
        else if (ch == 'd')
        {
          requiredPosition = movePlayer(RIGHT, allocated_map, &running, player, &lastChar, terminals, &current_sentence_index);
        }
        else if (ch == 'l')
        {
          running = 0;
        }

        openTerminal(terminals, requiredPosition, sentences, &current_sentence_index, player);
        printMap(allocated_map, player, terminals, &camera, sentences, &current_sentence_index, currentCamera);
      }

      if (timerTimeOver() == 1)
      {
        resetIndex(&currentCamera, 4);
        printMap(allocated_map, player, terminals, &camera, sentences, &current_sentence_index, currentCamera);
      }

      int caughtByW = (stage_one_map[player->y][player->x] == 'w' && currentCamera == 0);
      int caughtByX = (stage_one_map[player->y][player->x] == 'x' && currentCamera == 1);
      int caughtByY = (stage_one_map[player->y][player->x] == 'y' && currentCamera == 2);
      int caughtByZ = (stage_one_map[player->y][player->x] == 'z' && currentCamera == 3);
      if (caughtByW || caughtByX || caughtByY || caughtByZ)
      {
        player->score -= 5;
        caught_by_cam_sound(&engine);
        resetIndex(&currentCamera, 4);
        resetIndex(&current_sentence_index, 2);
        printMap(allocated_map, player, terminals, &camera, sentences, &current_sentence_index, currentCamera);
      }
    }

    if (running == 0)
      break;
  }

  return 0;
}

void openTerminal(int terminals[], char requiredTerminal, SentenceModel sentences[], int *current_sentence_index, Player *player)
{
  int index = requiredTerminal - 'A';

  if (index >= 0 && index < 3)
  {
    int value = open_terminal_model(requiredTerminal);
    if (value == sentences[*current_sentence_index].terminals[index])
    {
      terminal_correct_value_sound(&engine);
      terminals[index] = value;
      player->score += 10;
    }
    else
    {
      terminal_wrong_value_sound(&engine);
      resetIndex(current_sentence_index, 2);
      terminals[0] = -1;
      terminals[1] = -1;
      terminals[2] = -1;
      screenSetColor(RED, RED);
      player->score -= 5;
    }

    if (terminals[0] == sentences[*current_sentence_index].terminals[0] &&
        terminals[1] == sentences[*current_sentence_index].terminals[1] &&
        terminals[2] == sentences[*current_sentence_index].terminals[2])
    {
      door_unlocked_sound(&engine);
    }
  }
}

int open_terminal_model(char terminal)
{
  open_terminal_sound(&engine);
  int terminal_value = -1;

  keyboardDestroy();

  screenClear();
  screenGotoxy(1, 1);
  screenSetColor(BLACK, BLUE);
  printf("Digite o valor do terminal %c ", terminal);
  fflush(stdout);

  char ch;
  do
  {
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

void printMap(
    char **map,
    Player *player,
    int terminals[],
    int *camera,
    SentenceModel sentences[],
    int *current_sentence_index,
    int currentCamera)
{
  screenClear();

  int offsetX = (MAXX - COLUMN - 10) / 2;
  int offsetY = (MAXY - LINE) / 2;

  for (int y = 0; y < LINE; y++)
  {
    screenGotoxy(offsetX + 1, offsetY + y + 1);
    for (int x = 0; x < COLUMN; x++)
    {
      char ch = map[y][x];
      if (ch == '#')
      {
        screenSetColor(BLUE, BLUE);
      }
      else if (ch == 'O')
      {
        screenSetColor(GREEN, BLACK);
      }
      else if (ch == 'S')
      {
        if (terminals[0] == sentences[*current_sentence_index].terminals[0] &&
            terminals[1] == sentences[*current_sentence_index].terminals[1] &&
            terminals[2] == sentences[*current_sentence_index].terminals[2])
        {
          screenSetColor(BLUE, GREEN);
        }
        else
        {
          screenSetColor(BLUE, RED);
        }
      }
      else if (ch == 'A')
      {
        if (terminals[0] == -1)
        {
          screenSetColor(RED, YELLOW);
        }
        else
        {
          screenSetColor(BLUE, GREEN);
        }
      }
      else if (ch == 'B')
      {
        if (terminals[1] == -1)
        {
          screenSetColor(RED, YELLOW);
        }
        else
        {
          screenSetColor(BLUE, GREEN);
        }
      }
      else if (ch == 'C')
      {
        if (terminals[2] == -1)
        {
          screenSetColor(RED, YELLOW);
        }
        else
        {
          screenSetColor(BLUE, GREEN);
        }
      }
      else if (ch == 'W' || ch == 'X' || ch == 'Y' || ch == 'Z')
      {
        screenSetColor(BLUE, BROWN);
      }
      else if (ch == 'w')
      {
        if (currentCamera == 0)
        {
          screenSetColor(RED, RED);
        }
        else
        {
          screenSetColor(BLACK, BLACK);
        }
      }
      else if (ch == 'x')
      {
        if (currentCamera == 1)
        {
          screenSetColor(RED, RED);
        }
        else
        {
          screenSetColor(BLACK, BLACK);
        }
      }
      else if (ch == 'y')
      {
        if (currentCamera == 2)
        {
          screenSetColor(RED, RED);
        }
        else
        {
          screenSetColor(BLACK, BLACK);
        }
      }
      else if (ch == 'z')
      {
        if (currentCamera == 3)
        {
          screenSetColor(RED, RED);
        }
        else
        {
          screenSetColor(BLACK, BLACK);
        }
      }
      else
      {
        screenSetColor(BLACK, BLACK);
      }
      printf("%c", ch);
    }
  }

  screenSetColor(BLACK, BLUE);
  screenGotoxy(MAXX - 15, MAXY - 19);
  printf("Pontos: %d", player->score);

  screenGotoxy(MAXX - 15, MAXY - 18);
  printf("Senha: %s", sentences[*current_sentence_index].sentence);

  screenGotoxy(MAXX - 15, MAXY - 16);
  if (terminals[0] == sentences[*current_sentence_index].terminals[0])
  {
    screenSetColor(BLACK, GREEN);
    printf("Terminal A: OK ");
  }
  else
  {
    screenSetColor(BLACK, BROWN);
    printf("Terminal A: -- ");
  }

  screenGotoxy(MAXX - 15, MAXY - 15);
  if (terminals[1] == sentences[*current_sentence_index].terminals[1])
  {
    screenSetColor(BLACK, GREEN);
    printf("Terminal B: OK ");
  }
  else
  {
    screenSetColor(BLACK, BROWN);
    printf("Terminal B: -- ");
  }

  screenGotoxy(MAXX - 15, MAXY - 14);
  if (terminals[2] == sentences[*current_sentence_index].terminals[2])
  {
    screenSetColor(BLACK, GREEN);
    printf("Terminal C: OK ");
  }
  else
  {
    screenSetColor(BLACK, BROWN);
    printf("Terminal C: -- ");
  }

  if (terminals[0] == sentences[*current_sentence_index].terminals[0] &&
      terminals[1] == sentences[*current_sentence_index].terminals[1] &&
      terminals[2] == sentences[*current_sentence_index].terminals[2])
  {
    screenSetColor(GREEN, GREEN);
  }
  else
  {
    screenSetColor(BLUE, BLUE);
  }
  screenUpdate();
}

void setupStage(
    Player *player,
    char **allocated_map,
    int terminals[],
    int *camera,
    char stage_map[LINE][COLUMN + 1])
{
  player->x = 1;
  player->y = 1;

  for (int i = 0; i < LINE; i++)
    strcpy(allocated_map[i], stage_map[i]);

  terminals[0] = -1;
  terminals[1] = -1;
  terminals[2] = -1;
}

void resetIndex(int *current_sentence_index, int total_sentences)
{
  *current_sentence_index = (*current_sentence_index + 1) % total_sentences;
  screenUpdate();
}

char movePlayer(
    MoveDirection direction,
    char **map,
    int *running,
    Player *player,
    char *lastChar,
    int terminals[],
    int *current_sentence_index)
{

  int x = player->x;
  int y = player->y;

  if (direction == UP)
  {
    y = player->y - 1;
    x = player->x;
  }
  else if (direction == DOWN)
  {
    y = player->y + 1;
    x = player->x;
  }
  else if (direction == LEFT)
  {
    y = player->y;
    x = player->x - 1;
  }
  else if (direction == RIGHT)
  {
    y = player->y;
    x = player->x + 1;
  }

  if (map[y][x] == '.' || map[y][x] == 'S' || map[y][x] == 'x' || map[y][x] == 'w' || map[y][x] == 'y' || map[y][x] == 'z')
  {
    walk_sound(&engine);
    int isOpen = terminals[0] == sentences[*current_sentence_index].terminals[0] &&
                 terminals[1] == sentences[*current_sentence_index].terminals[1] &&
                 terminals[2] == sentences[*current_sentence_index].terminals[2];

    if (map[y][x] == 'S') {
      if (isOpen) {
        pass_stage_sound(&engine);
        *running = 0;
      } else {
        door_locked_sound(&engine);
        return map[player->y][player->x];
      }
    }

    *lastChar = stage_one_map[player->y][player->x] == 'O' ? '.' : stage_one_map[player->y][player->x];

    map[player->y][player->x] = *lastChar;

    player->x = x;
    player->y = y;
    map[player->y][player->x] = 'O';
  }

  return map[y][x];
}