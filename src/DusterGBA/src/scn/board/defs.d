module scn.board.defs;

import core.stdc.string;
import dusk;
import tonc;
import libgba.maxmod;
import res;
import dusk.contrib.mgba;

import game;
import scn.board;

struct PawnMoveTweenInfo {
    int pawn_gid;
    VPos16 start_pos;
    VPos16 end_pos;
    int start_frame;
    int end_frame;
}

struct PawnFlashTweenInfo {
    int pawn_gid;
    int initiator_gid;
    int start_frame;
    int end_frame;
    bool flash_color;
}

struct SpritePawnPair {
    pawn_gid_t pawn_gid;
    int sprite;
}

enum BoardScenePage {
    BOARDSCN_BOARD,
    BOARDSCN_PAUSEMENU,
}

TSurface bg0_srf;
int bg0_srf_cbb;
int bg0_srf_sbb;
int bg1_tte_cbb;
int bg1_tte_sbb;
VPos board_offset;
bool board_ui_dirty;
bool sidebar_dirty;
VPos16 cursor_pos;
bool cursor_shown;
bool cursor_click;
VPos16 cursor_click_pos;
int cursor_last_moved_frame;
PawnMoveTweenInfo pawn_move_tween;
PawnFlashTweenInfo pawn_flash_tween;
// CC_HashTable* pawn2sprite;
SpritePawnPair[128] sprite_pawn_pairs;
enum CACHE_RANGE_BUF_LEN = 128;
VPos16[CACHE_RANGE_BUF_LEN] cache_range_buf;
int cache_range_buf_filled;
bool request_step;
bool pawn_move_range_dirty;
BoardScenePage board_scene_page;
bool pausemenu_dirty;
enum BOARD_SCROLL_WINDOW = 16;
int board_scroll_x;
int board_scroll_y;
int pause_cursor_selection;
enum NUM_SIDEBAR_PAGES = 2;
int sidebar_page;
int movequeue_length;
QueuedMove[TEAM_MAX_PAWNS + 1] movequeue_queue;
int movequeue_progress;
int movequeue_delay_timer;
int ai_played_move;
enum AI_WAIT_TIME = 30;
int ai_wait_timer;

// extern functions
void sfx_play_startchime();
void sfx_play_scroll();
void sfx_play_click();
void sfx_play_interact();
void sfx_play_move();
void sfx_play_cant();