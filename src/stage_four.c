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

void openTerminalFour(int terminals[], char terminal, SentenceModel sentences[], int *current_sentence_index, Player *player);
int open_terminal_model_four(char terminal);
void resetIndexFour(int *item, int total);
char movePlayerFour(MoveDirection direction, char **map, int *running, Player *player, char *lastChar);

char stage_four_map[LINE][COLUMN + 1] = {
    "########################################",
    "#@..............#######.........o......#",
    "###.X##################.X###############",
    "#...#...^....zzzz...o...#.............##",
    "#.###.#######.#z#######.#############.##",
    "#.#...#..^###.#zzzz.....#.#.........#..#",
    "#w#.###.###C#.#z#######.#.#.#####.#.####",
    "#www#...#...#.#z#..^..#.#.#.#.....#...##",
    "#w#.#.###.###.#.#.###.#.#...#.##W.######",
    "#oX.#.#...#x#.#.#.#.#.#...#.#.#.....#.##",
    "#.#.#.#.###x#.#.#...#...###.#.#######..#",
    "#.#.#.#..xxxxx..###.#.###.#.....^....#.#",
    "#.#.#.#.Y.###x###.#.#.#.#.#.########...#",
    "#.#.#...#.#..x#B#...#.#.#.#..........#.#",
    "#.#.#.#.#.#.###.#.#.#.#.#.#.X######.####",
    "#.#.#.#.#.#.#...#.#y..#.#^#...o#....zzz#",
    "#.#.#.#.#.#.#.###.#####.############z###",
    "#.#.#.#S#.#.#...yyyyyy#^..........zzz.A#",
    "########################################"};

SentenceModel sentencesFour[4] = {
    {"(A ∧ B) → C", {1, 1, 0}},
    {"¬A ∨ (B ∧ C)", {0, 1, 1}},
    {"A ⊕ C", {1, 0, 1}}};

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

                openTerminalFour(terminals, requiredPos, sentencesFour, &current_sentence_index, player);
                printMapFour(allocated_map, player, terminals, &camera, sentencesFour, &current_sentence_index, currentCamera);
            }

            if (timerTimeOver() == 1)
            {
                resetIndexFour(&currentCamera, 4);
                printMapFour(allocated_map, player, terminals, &camera, sentencesFour, &current_sentence_index, currentCamera);
            }

            int caughtByW = (stage_four_map[player->y][player->x] == 'w' && currentCamera == 0);
            int caughtByX = (stage_four_map[player->y][player->x] == 'x' && currentCamera == 1);
            int caughtByY = (stage_four_map[player->y][player->x] == 'y' && currentCamera == 2);
            int caughtByZ = (stage_four_map[player->y][player->x] == 'z' && currentCamera == 3);
            if (caughtByW || caughtByX || caughtByY || caughtByZ)
            {
                resetIndexFour(&currentCamera, 4);
                resetIndexFour(&current_sentence_index, 2);
                printMapFour(allocated_map, player, terminals, &camera, sentencesFour, &current_sentence_index, currentCamera);
            }
        }

        if (running == 0)
            break;
    }

    return 0;
}

void openTerminalFour(int terminals[], char requiredTerminal, SentenceModel sentences[], int *current_sentence_index, Player *player)
{
    int index = requiredTerminal - 'A';

    if (index >= 0 && index < 3)
    {
        int value = open_terminal_model_four(requiredTerminal);
        if (value == sentences[*current_sentence_index].terminals[index])
        {
            terminals[index] = value;
            player->score += 10;
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

            if (ch == '@')
            {
                screenSetColor(GREEN, BLACK);
            }
            else if (ch == '#')
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
                screenSetColor(terminals[0] == -1 ? RED : BLUE, terminals[0] == -1 ? YELLOW : GREEN);
            }
            else if (ch == 'B')
            {
                screenSetColor(terminals[1] == -1 ? RED : BLUE, terminals[1] == -1 ? YELLOW : GREEN);
            }
            else if (ch == 'C')
            {
                screenSetColor(terminals[2] == -1 ? RED : BLUE, terminals[2] == -1 ? YELLOW : GREEN);
            }
            else if (ch == '^')
            {
                screenSetColor(RED, BLACK);
            }
            else if (ch == 'o')
            {
                screenSetColor(BLACK, WHITE);
            }

            else if (ch == 'W' || ch == 'X' || ch == 'Y' || ch == 'Z')
            {
                screenSetColor(BLUE, BROWN);
            }
            else if (ch == 'w')
            {
                screenSetColor(currentCamera == 0 ? RED : BLACK, currentCamera == 0 ? RED : BLACK);
            }
            else if (ch == 'x')
            {
                screenSetColor(currentCamera == 1 ? RED : BLACK, currentCamera == 1 ? RED : BLACK);
            }
            else if (ch == 'y')
            {
                screenSetColor(currentCamera == 2 ? RED : BLACK, currentCamera == 2 ? RED : BLACK);
            }
            else if (ch == 'z')
            {
                screenSetColor(currentCamera == 3 ? RED : BLACK, currentCamera == 3 ? RED : BLACK);
            }
            else
            {
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

    screenSetColor(BLACK, BLUE);

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

    int boxX = COLUMN + 20;
    int boxY = 13;

    screenSetColor(BLACK, BLUE);

    screenGotoxy(boxX, boxY);
    printf("+------------------------------+");
    screenGotoxy(boxX, boxY + 1);
    printf("| GUIA DO MAPA                |");
    screenGotoxy(boxX, boxY + 2);
    printf("| (o) Buraco  -> Reinicia     |");
    screenGotoxy(boxX, boxY + 3);
    printf("| (^) Espinho -> -5 pontos    |");
    screenGotoxy(boxX, boxY + 4);
    printf("| Cameras -> -7 se te ver     |");
    screenGotoxy(boxX, boxY + 5);
    printf("| Dica: fique imóvel e a      |");
    screenGotoxy(boxX, boxY + 6);
    printf("| magia da Pixie te protege   |");
    screenGotoxy(boxX, boxY + 7);
    printf("| A/B/C -> Digite 0 ou 1      |");
    screenGotoxy(boxX, boxY + 8);
    printf("| (S)  ->      Saida          |");
    screenGotoxy(boxX, boxY + 9);
    printf("+------------------------------+");

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

    if (map[y][x] == '.' || map[y][x] == 'S' || map[y][x] == 'x' || map[y][x] == 'w' || map[y][x] == 'y' || map[y][x] == 'z')
    {
        if (next == '^')
        {
            player->score -= 5;
        }

        if (next == 'o')
        {
            map[player->y][player->x] = stage_four_map[player->y][player->x];
            player->x = 1;
            player->y = 1;
            map[player->y][player->x] = '@';
            return next;
        }

        if (map[y][x] == 'S')
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

