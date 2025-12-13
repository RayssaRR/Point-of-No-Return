#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"

#define MAX_PLAYERS 20
#define NAME_SIZE 100

typedef struct {
    char name[NAME_SIZE];
    int score;
} RankingEntry;

RankingEntry rankingList[MAX_PLAYERS];
int rankingCount = 0;

void loadRanking() {
    FILE *file = fopen("ranking.txt", "r");
    if (!file) {
        file = fopen("ranking.txt", "w");
        if (file) {
            fclose(file);
        }
        rankingCount = 0;
        return;
    }

    rankingCount = 0;
    while (rankingCount < MAX_PLAYERS &&
           fscanf(file, "%99s %d",
                  rankingList[rankingCount].name,
                  &rankingList[rankingCount].score) == 2) {
        rankingCount++;
    }

    fclose(file);
}

void saveRanking() {
    FILE *file = fopen("ranking.txt", "w");
    if (!file) return;

    for (int i = 0; i < rankingCount; i++) {
        fprintf(file, "%s %d\n", rankingList[i].name, rankingList[i].score);
    }

    fclose(file);
}

void addScore(const char *name, int score) {
    loadRanking();

    for (int i = 0; i < rankingCount; i++) {
        if (strcmp(rankingList[i].name, name) == 0) {
            if (score > rankingList[i].score)
                rankingList[i].score = score;
            saveRanking();
            return;
        }
    }

    if (rankingCount < MAX_PLAYERS) {
        strcpy(rankingList[rankingCount].name, name);
        rankingList[rankingCount].score = score;
        rankingCount++;
    }

    for (int i = 0; i < rankingCount - 1; i++)
        for (int j = i + 1; j < rankingCount; j++)
            if (rankingList[j].score > rankingList[i].score) {
                RankingEntry temp = rankingList[i];
                rankingList[i] = rankingList[j];
                rankingList[j] = temp;
            }
    saveRanking();
}


void showRankingScreen() {
    loadRanking();
    screenClear();
    screenGotoxy(1,1);

    printf("\n===== RANKING =====\n");
    if (rankingCount == 0) {
        printf("Nenhum dado de ranking ainda!\n");
        return;
    }

    for (int i = 0; i < rankingCount; i++) {
        printf("%d. %s - %d pontos\n", i + 1,
               rankingList[i].name,
               rankingList[i].score);
    }
}
