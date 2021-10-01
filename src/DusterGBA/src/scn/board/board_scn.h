#pragma once

#include <string.h>
#include "dusk.h"
#include <tonc.h>
#include "res.h"
#include "contrib/mgba.h"
#include "game/game.h"

extern TSurface bg0_srf;
extern int bg0_srf_cbb;
extern int bg0_srf_sbb;
extern VPos board_offset;
extern bool bg_ui_dirty;
extern int game_turn;
extern VPos16 cursor_pos;
extern bool cursor_down;

void draw_board_outline();
void draw_board_cursor();
void draw_board();