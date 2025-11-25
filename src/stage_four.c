#include <stdio.h>
#include <string.h>
#include "screen.h"
#include "timer.h"
#include "keyboard.h"
#include "utils.h"

typedef struct
{
    char sentence[200];
    int terminals[3];
} SentenceModel;

void printMapFour(
    char **map,
    Player *player,
    int terminals[],
    int *camera,
    SentenceModel sentences[],
    int *current_sentence_index,
    int currentCamera);

void setupStageFour(
    Player *player,
    char **allocated_map,
    int terminals[],
    int *camera,
    char stage_map[LINE][COLUMN + 1]);

void openTerminalFour(int terminals[], char terminal, SentenceModel sentences[], int *current_sentence_index);
int open_terminal_model_four(char terminal);
void resetIndexFour(int *item, int total);
char movePlayerFour(MoveDirection direction, char **map, int *running, Player *player, char *lastChar);

char stage_four_map[LINE][COLUMN + 1] = {
    "########################################",
    "#@..............#######...............##",
    "###.###################.################",
    "#...#...................#.............##",
    "#.###.#######.#.#######.#############.##",
    "#.#...#.....#.#.........#.#.........#.##",
    "#.#.###.###C#.#.#######.#.#.#####.#.####",
    "#...#...#.###.#.#.....#.#.#.#.....#...##",
    "#.#.#.###.#.#.#.#.###.#.#.#.#.###.######",
    "#.#.#.#...#.#.#.#.#.#.#...#.#.#.....#.##",
    "#.#.#.#.###.#.#.#...#...###.#.#######.##",
    "#.#.#.#.........###.#.###.#..........#.#",
    "#.#.#.#.#.###.###.#.#.#.#.#.#########.##",
    "#.#.#...#.#...#B#.#.#.#.#.#..........#.#",
    "#.#.#.#.#.#.###.#.#.#.#.#.#.#######.##.#",
    "#.#.#.#.#.#.#...#.#.#.#.#.#.......#....#",
    "#.#.#.#.#.#.#.#####.#.#.############.###",
    "#.#.#.#S#.#.#.........#...............A#",
    "########################################"};

SentenceModel sentencesFour[4] = {
    {"(A ∧ B) → C", {1, 1, 0}},
    {"¬A ∨ (B ∧ C)", {0, 1, 1}},
    {"A ⊕ C", {1, 0, 1}},
    {"(A ∧ ¬B) ∨ (B ∧ ¬C)", {1, 0, 0}}};

int stage_four(char **allocated_map, Player *player)
{
    player->x = 1;
    player->y = 1;

    int current_sentence_index = 0;

    static int ch = 0;
    int currentCamera = 0;
    int running = 1;
    int camera = 0;
    int terminals[3] = {-1, -1, -1};
    char requiredPos = '.';
    char lastChar = '.';

    keyboardInit();
    screenInit(0);

    while (1)
    {

        setupStageFour(player, allocated_map, terminals, &camera, stage_four_map);
        printMapFour(allocated_map, player, terminals, &camera, sentencesFour, &current_sentence_index, currentCamera);
        timerInit(3000);

        while (running)
        {
            if (keyhit())
            {
                ch = readch();

                if (ch == 'w')
                    requiredPos = movePlayerFour(UP, allocated_map, &running, player, &lastChar);
                else if (ch == 's')
                    requiredPos = movePlayerFour(DOWN, allocated_map, &running, player, &lastChar);
                else if (ch == 'a')
                    requiredPos = movePlayerFour(LEFT, allocated_map, &running, player, &lastChar);
                else if (ch == 'd')
                    requiredPos = movePlayerFour(RIGHT, allocated_map, &running, player, &lastChar);
                else if (ch == 'l')
                {
                    running = 0;
                    player->win = 0;
                }

                openTerminalFour(terminals, requiredPos, sentencesFour, &current_sentence_index);
                printMapFour(allocated_map, player, terminals, &camera, sentencesFour, &current_sentence_index, currentCamera);
            }

            if (timerTimeOver() == 1)
            {
                resetIndexFour(&currentCamera, 4);
                printMapFour(allocated_map, player, terminals, &camera, sentencesFour, &current_sentence_index, currentCamera);
            }
        }

        if (running == 0)
            break;
    }

    return 0;
}

void openTerminalFour(int terminals[], char requiredTerminal, SentenceModel sentences[], int *current_sentence_index)
{
    int index = requiredTerminal - 'A';

    if (index >= 0 && index < 3)
    {
        int value = open_terminal_model_four(requiredTerminal);
        if (value == sentences[*current_sentence_index].terminals[index])
        {
            terminals[index] = value;
        }
        else
        {
            resetIndexFour(current_sentence_index, 2);
            terminals[0] = terminals[1] = terminals[2] = -1;
            screenSetColor(RED, RED);
        }
    }
}

int open_terminal_model_four(char terminal)
{
    int v;
    screenClear();
    screenGotoxy(1, 1);
    screenSetColor(BLACK, BLUE);
    printf("Digite o valor do terminal %c:", terminal);
    screenUpdate();

    keyboardDestroy();
    screenGotoxy(1, 2);
    scanf("%d", &v);

    keyboardInit();
    return v;
}

void printMapFour(
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
        screenGotoxy(offsetX, offsetY + y + 1);

        for (int x = 0; x < COLUMN; x++)
        {
            char ch = map[y][x];

            switch (ch)
            {
            case '#':
                screenSetColor(BLUE, BLUE);
                break;
            case '@':
                screenSetColor(GREEN, BLACK);
                break;
            case 'A':
            case 'B':
            case 'C':
                screenSetColor(WHITE, BLACK);
                break;
            case 'S':
                screenSetColor(BLACK, GREEN);
                break;
            default:
                screenSetColor(BLACK, BLACK);
            }

            printf("%c", ch);
        }
    }

    screenGotoxy(MAXX - 15, MAXY - 19);
    screenSetColor(BLACK, BLUE);
    printf("Score: %.2f", player->score);

    screenGotoxy(MAXX - 15, MAXY - 18);
    printf("Sentença: %s", sentences[*current_sentence_index].sentence);

    screenUpdate();
}

void setupStageFour(
    Player *player,
    char **allocated_map,
    int terminals[],
    int *camera,
    char stage_map[LINE][COLUMN + 1])
{
    player->win = 0;

    for (int i = 0; i < LINE; i++)
        strcpy(allocated_map[i], stage_map[i]);

    for (int i = 0; i < 3; i++)
        terminals[i] = -1;
}

void resetIndexFour(int *idx, int total)
{
    *idx = (*idx + 1) % total;
}

char movePlayerFour(MoveDirection direction, char **map, int *running, Player *player, char *lastChar)
{
    int x = player->x;
    int y = player->y;

    if (direction == UP)
        y--;
    else if (direction == DOWN)
        y++;
    else if (direction == LEFT)
        x--;
    else if (direction == RIGHT)
        x++;

    char next = map[y][x];

    if (next == '.' || next == '@' || next == 'A' || next == 'B' || next == 'C' || next == 'S')
    {

        if (next == 'S')
        {
            player->win = 1;
            *running = 0;
        }

        *lastChar = '.';
        map[player->y][player->x] = *lastChar;

        player->x = x;
        player->y = y;

        map[y][x] = '@';
    }

    return next;
}