#pragma once

#include <string.h>
#include "dusk.h"
#include "tonc.h"
#include "res.h"
#include "vpos.h"
#include "contrib/mgba.h"
#include "game/game.h"


typedef struct {
    int pawn_gid;
    VPos start_pos;
    VPos end_pos;
    VPos step;
    int num_steps;
    int end_frame;
} PawnTweenInfo;

extern TSurface bg0_srf;
extern int bg0_srf_cbb;
extern int bg0_srf_sbb;
extern int bg1_tte_cbb;
extern int bg1_tte_sbb;
extern VPos board_offset;
extern BOOL board_ui_dirty;
extern BOOL sidebar_dirty;
extern int game_turn;
extern VPos16 cursor_pos;
extern BOOL cursor_shown;
extern BOOL cursor_click;
extern VPos16 cursor_click_pos;
extern PawnTweenInfo pawn_tween;

void draw_board_outline();
void draw_board_cursor();
void draw_board();
void draw_sidebar();

Pawn* get_cursor_pawn();
Pawn* get_clicked_pawn();

void update_pawn_tween();