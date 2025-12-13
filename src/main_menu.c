#include <stdio.h>
#include "screen.h"
#include "ranking.h"

void showMainMenu(int *option)
{
    screenClear();

    int startX = 2;
    int startY = 1;

    screenSetColor(WHITE, BLUE);

    screenGotoxy(startX, startY);
    printf("╔══════════════════════════════╗");
    screenGotoxy(startX, startY + 1);
    printf("║      POINT OF NO RETURN      ║");
    screenGotoxy(startX, startY + 2);
    printf("╠══════════════════════════════╣");
    screenGotoxy(startX, startY + 3);
    printf("║  1 ▶ Jogar                   ║");
    screenGotoxy(startX, startY + 4);
    printf("║  2 ▶ Ver Ranking             ║");
    screenGotoxy(startX, startY + 5);
    printf("║  0 ▶ Sair                    ║");
    screenGotoxy(startX, startY + 6);
    printf("╚══════════════════════════════╩");

    screenSetColor(WHITE, BLUE);
    screenGotoxy(startX , startY + 8);
    printf("Escolha uma opcao: ");

    scanf("%d", option);
}

