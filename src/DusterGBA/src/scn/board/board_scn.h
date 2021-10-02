#pragma once

#include <string.h>
#include "dusk.h"
#include "tonc.h"
#include "res.h"
#include "vpos.h"
#include "contrib/mgba.h"
#include "game/game.h"

#include "cc_array.h"
#include "cc_hashtable.h"

typedef struct {
    int pawn_gid;
    VPos start_pos;
    VPos end_pos;
    int start_frame;
    int end_frame;
} PawnTweenInfo;

typedef struct {
    int pawn_gid;
    int sprite;
} SpritePawnPair;

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
extern CC_HashTable* pawn2sprite;
extern SpritePawnPair sprite_pawn_pairs[128];

VPos16 board_vpos_to_pix_pos(int tx, int ty);
void draw_board_outline();
void draw_board_cursor();
void draw_board();
void draw_sidebar();

Pawn* get_cursor_pawn();
Pawn* get_clicked_pawn();

void animate_pawn_move(s16 pawn_gid, VPos start_pos, VPos end_pos);
void update_pawn_tween();