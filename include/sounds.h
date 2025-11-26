#ifndef SOUNDS_H
#define SOUNDS_H

#include <stdint.h>
#include "miniaudio.h"

void walk_sound(ma_engine *engine);
void open_terminal_sound(ma_engine *engine);
void correct_value_sound(ma_engine *engine);
void wrong_value_sound(ma_engine *engine);
void pass_stage_sound(ma_engine *engine);
void caught_sound(ma_engine *engine);

#endif