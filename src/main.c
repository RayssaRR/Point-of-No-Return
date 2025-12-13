#define MINIAUDIO_IMPLEMENTATION
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "miniaudio.h"
#include "screen.h"
#include "timer.h"
#include "stage_one.h"
#include "stage_two.h"
#include "stage_three.h"
#include "utils.h"
#include "ranking.h"
#include "main_menu.h"


void free_map_memory(char **map);

int main() {
  ma_engine engine;
  ma_engine_init(NULL, &engine);

  char stage_noise_path[4096];
  getcwd(stage_noise_path, sizeof(stage_noise_path));
  strcat(stage_noise_path, "/src/assets/stage_noise.wav");

  char menu_music_path[4096];
  getcwd(menu_music_path, sizeof(menu_music_path));
  strcat(menu_music_path, "/src/assets/point_of_no_return_menu.wav");

  ma_sound stage_noise;
  if (ma_sound_init_from_file(&engine, stage_noise_path, 0, NULL, NULL, &stage_noise) != MA_SUCCESS) {
      printf("Erro ao carregar som\n");
      return -1;
  }

  ma_sound menu_music;
  if (ma_sound_init_from_file(&engine, menu_music_path, 0, NULL, NULL, &menu_music) != MA_SUCCESS) {
      printf("Erro ao carregar som\n");
      return -1;
  }

  ma_sound_set_looping(&stage_noise, MA_TRUE);
  ma_sound_set_looping(&menu_music, MA_TRUE);

  int running = 1;
  int stop = 1;
  char username[100];

  char **allocated_map;
  allocated_map = (char **) calloc(LINE, sizeof(char *));
  for (int i = 0; i < LINE; i++) {
      allocated_map[i] = (char *) calloc(COLUMN + 1, sizeof(char));
  }

int option = -1;

while (option != 0) {

    ma_sound_start(&menu_music);
    showMainMenu(&option);
    ma_sound_stop(&menu_music);

    if (option == 0) {
        return 0; 
    }

    if (option == 2) {
        showRankingScreen();
        printf("\nPressione ENTER para voltar ao menu...");
        getchar(); getchar();
        continue;
    }

    if (option == 1) {
        ma_sound_start(&menu_music);

        showInitialMenu(&running, &stop);
        if (stop == 0) return 0;

        inputUsername(username);
        ma_sound_stop(&menu_music);

        break; 
    }

    printf("Opcao invalida!\n");
}


  timerInit(0);

  Player player;
  player.score = 0;
  player.x = 1;
  player.y = 1;

  ma_sound_start(&stage_noise);

  dialogue_zero();
  stage_one(allocated_map, &player);
  dialogue_one();

  stage_two(allocated_map, &player);
  dialogue_two();

  stage_three(allocated_map, &player);

  dialogue_final();
  ma_sound_stop(&stage_noise);

  screenClear();
  screenGotoxy(1,1);
  screenSetColor(BLACK, BLUE);
  printf("Obrigado por jogar Point of No Return, %s!", username);
  printf("\nSua pontuação foi: %d pontos!\n", player.score);
  
  addScore(username, player.score);

  screenSetColor(BLACK, BLUE);
  printf("\n\nPontuação salva no ranking!\n");
  printf("Pressione ENTER para continuar...");
  getchar(); getchar();
  //EXIT

  free_map_memory(allocated_map);
  return 0;
}

void free_map_memory(char **map) {
    for (int i = 0; i < LINE; i++) free(map[i]);
    free(map);
}
