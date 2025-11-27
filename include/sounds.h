#ifndef SOUNDS_H
#define SOUNDS_H

#include <stdint.h>
#include "miniaudio.h"

void walk_sound(ma_engine *engine);
void open_terminal_sound(ma_engine *engine);
void terminal_correct_value_sound(ma_engine *engine);
void terminal_wrong_value_sound(ma_engine *engine);
void pass_stage_sound(ma_engine *engine);
void caught_by_cam_sound(ma_engine *engine);
void falling_sound(ma_engine *engine);
void thorns_sound(ma_engine *engine);

#endif