#define MINIAUDIO_IMPLEMENTATION
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "miniaudio.h"
#include "screen.h"
#include "timer.h"
#include "stage_one.h"
#include "stage_two.h"
#include "stage_three.h"
#include "stage_four.h"
#include "utils.h"

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
  int penalty = 0;

  char **allocated_map;
  allocated_map = (char **) calloc(LINE, sizeof(char *));
  for (int i = 0; i < LINE; i++) {
      allocated_map[i] = (char *) calloc(COLUMN + 1, sizeof(char));
  }

  ma_sound_start(&menu_music);
  showInitialMenu(&running, &stop);

  if (stop == 0) return 0;

  inputUsername(username);
  ma_sound_stop(&menu_music);

  timerInit(0);
  time_t inicio = time(NULL);

  Player player;
  player.x = 1;
  player.y = 1;

  ma_sound_start(&stage_noise);

  dialogue_zero();
  stage_one(allocated_map, &player);
  dialogue_one();

  stage_two(allocated_map, &player, &penalty);
  dialogue_two();

  stage_three(allocated_map, &player, &penalty);
  dialogue_three();

  stage_four(allocated_map, &player, &penalty);
  
  time_t fim = time(NULL); // Finaliza o cronometro
  int tempo_jogo = (int) difftime(fim, inicio);

  tempo_jogo += penalty;
  if (tempo_jogo < 0) tempo_jogo = 0;

  dialogue_final();
  ma_sound_stop(&stage_noise);

  screenClear();
  screenGotoxy(1,1);
  screenSetColor(BLACK, BLUE);
  printf("Parabéns %s, sua pontuação foi: %d\n\n", username, tempo_jogo);

  printf("Obrigado por jogar Point of No Return!\n");
  //EXIT

  free_map_memory(allocated_map);
  return 0;
}

void free_map_memory(char **map) {
    for (int i = 0; i < LINE; i++) free(map[i]);
    free(map);
}
