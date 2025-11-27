#include <unistd.h>
#include <string.h>
#include "miniaudio.h"
#include "sounds.h"

#define PATH_MAX 4096

void walk_sound(ma_engine *engine) {
  char srcPath[PATH_MAX];
  getcwd(srcPath, sizeof(srcPath));
  strcat(srcPath, "/src/assets/walk.wav");
  ma_engine_play_sound(engine, srcPath, NULL);
}

void open_terminal_sound(ma_engine *engine) {
  char srcPath[PATH_MAX];
  getcwd(srcPath, sizeof(srcPath));
  strcat(srcPath, "/src/assets/open_terminal.wav");
  ma_engine_play_sound(engine, srcPath, NULL);
}

void terminal_correct_value_sound(ma_engine *engine) {
  char srcPath[PATH_MAX];
  getcwd(srcPath, sizeof(srcPath));
  strcat(srcPath, "/src/assets/terminal_correct_value.wav");
  ma_engine_play_sound(engine, srcPath, NULL);
}

void terminal_wrong_value_sound(ma_engine *engine) {
  char srcPath[PATH_MAX];
  getcwd(srcPath, sizeof(srcPath));
  strcat(srcPath, "/src/assets/terminal_wrong_value.wav");
  ma_engine_play_sound(engine, srcPath, NULL);
}

void pass_stage_sound(ma_engine *engine) {
  char srcPath[PATH_MAX];
  getcwd(srcPath, sizeof(srcPath));
  strcat(srcPath, "/src/assets/pass_stage.wav");
  ma_engine_play_sound(engine, srcPath, NULL);
}

void caught_by_cam_sound(ma_engine *engine) {
  char srcPath[PATH_MAX];
  getcwd(srcPath, sizeof(srcPath));
  strcat(srcPath, "/src/assets/caught_by_cam.wav");
  ma_engine_play_sound(engine, srcPath, NULL);
}

void falling_sound(ma_engine *engine) {
  char srcPath[PATH_MAX];
  getcwd(srcPath, sizeof(srcPath));
  strcat(srcPath, "/src/assets/falling.wav");
  ma_engine_play_sound(engine, srcPath, NULL);
}

void thorns_sound(ma_engine *engine) {
  char srcPath[PATH_MAX];
  getcwd(srcPath, sizeof(srcPath));
  strcat(srcPath, "/src/assets/falling.wav");
  ma_engine_play_sound(engine, srcPath, NULL);
}