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
    VPos16 start_pos;
    VPos16 end_pos;
    int start_frame;
    int end_frame;
} PawnMoveTweenInfo;

typedef struct {
    int pawn_gid;
    int initiator_gid;
    int start_frame;
    int end_frame;
    BOOL flash_color;
} PawnFlashTweenInfo;

typedef struct {
    pawn_gid_t pawn_gid;
    int sprite;
} SpritePawnPair;

typedef enum {
    BOARDSCN_BOARD,
    BOARDSCN_PAUSEMENU,
} BoardScenePage;

extern TSurface bg0_srf;
extern int bg0_srf_cbb;
extern int bg0_srf_sbb;
extern int bg1_tte_cbb;
extern int bg1_tte_sbb;
extern VPos board_offset;
extern BOOL board_ui_dirty;
extern BOOL sidebar_dirty;
extern VPos16 cursor_pos;
extern BOOL cursor_shown;
extern BOOL cursor_click;
extern VPos16 cursor_click_pos;
extern PawnMoveTweenInfo pawn_move_tween;
extern PawnFlashTweenInfo pawn_flash_tween;
extern CC_HashTable* pawn2sprite;
extern SpritePawnPair sprite_pawn_pairs[128];
#define CACHE_RANGE_BUF_LEN 128
extern VPos16 cache_range_buf[CACHE_RANGE_BUF_LEN];
extern int cache_range_buf_filled;
extern BOOL request_step;
extern BOOL pawn_move_range_dirty;
extern BoardScenePage board_scene_page;
extern BOOL pausemenu_dirty;
#define BOARD_SCROLL_WINDOW 16
extern int board_scroll_x;
extern int board_scroll_y;
extern int pause_cursor_selection;
#define NUM_SIDEBAR_PAGES 2
extern int sidebar_page;
extern int movequeue_length;
extern QueuedMove movequeue_queue[TEAM_MAX_PAWNS + 1];
extern int movequeue_progress;
extern int movequeue_delay_timer;
extern int ai_played_move;
#define AI_WAIT_TIME 30
extern int ai_wait_timer;

void set_ui_dirty();

VPos16 board_vpos_to_pix_pos(int tx, int ty);
void draw_clear_text_surface();
void draw_clear_ui_surface();
void draw_board_outline();
void draw_board_cursor();
void draw_board();
void draw_sidebar();

Pawn* get_cursor_pawn();
pawn_gid_t get_clicked_pawn_gid();
Pawn* get_clicked_pawn();
void on_cursor_try_click(VPos16 click_pos);
void on_try_move_cursor(int mx, int my);

void animate_pawn_move(pawn_gid_t pawn_gid, VPos16 start_pos, VPos16 end_pos);
void animate_pawn_flash(pawn_gid_t pawn_gid, pawn_gid_t initiator_gid, BOOL flash_color);
void update_pawn_tweens();
int step_running_queued_moves(QueuedMove* moves, int length, int progress);
void update_queued_moves();

void update_pause_ui();
void draw_pause_ui();

void boardscn_sfx_play_startchime();
void boardscn_sfx_play_scroll();
void boardscn_sfx_play_click();
void boardscn_sfx_play_interact();
void boardscn_sfx_play_move();
void boardscn_sfx_play_cant();