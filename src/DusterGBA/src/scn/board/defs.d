module scn.board.defs;

import core.stdc.string;
import dusk;
import tonc;
import libgba.maxmod;
import res;
import dusk.contrib.mgba;
import libtind.ds.dict;

import game;
import scn.board;

struct PawnMoveTweenInfo {
    pawn_gid_t pawn_gid;
    VPos16 start_pos;
    VPos16 end_pos;
    int start_frame;
    int end_frame;
}

struct PawnFlashTweenInfo {
    pawn_gid_t pawn_gid;
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

enum CACHE_RANGE_BUF_LEN = 128;
enum BOARD_SCROLL_WINDOW = 16;
enum NUM_SIDEBAR_PAGES = 2;
enum AI_WAIT_TIME = 30;
enum BG0_SRF_CBB = 0;
enum BG0_SRF_SBB = 31;
enum BG1_TTE_CBB = 2;
enum BG1_TTE_SBB = 28;
enum HUMAN_PLAYER_TEAM = -1;

__gshared TSurface bg0_srf;
__gshared VPos board_offset;
__gshared bool board_ui_dirty = true;
__gshared bool sidebar_dirty = true;
__gshared VPos16 cursor_pos;
__gshared bool cursor_shown;
__gshared bool cursor_click;
__gshared VPos16 cursor_click_pos;
__gshared int cursor_last_moved_frame;
__gshared PawnMoveTweenInfo pawn_move_tween;
__gshared PawnFlashTweenInfo pawn_flash_tween;
__gshared Dict!(pawn_gid_t, int) pawn2sprite;
__gshared SpritePawnPair[128] sprite_pawn_pairs;
__gshared VPos16[CACHE_RANGE_BUF_LEN] cache_range_buf;
__gshared int cache_range_buf_filled;
__gshared bool request_step;
__gshared bool pawn_move_range_dirty;
__gshared BoardScenePage board_scene_page;
__gshared bool pausemenu_dirty;
__gshared int board_scroll_x;
__gshared int board_scroll_y;
__gshared int pause_cursor_selection = 0;
__gshared int sidebar_page;
__gshared int movequeue_length = 0;
__gshared QueuedMove[TEAM_MAX_PAWNS + 1] movequeue_queue;
__gshared int movequeue_progress = -1;
__gshared int movequeue_delay_timer = 0;
__gshared int ai_played_move = -1;
__gshared int ai_wait_timer;

// extern functions
extern (C) {
    void sfx_play_startchime();
    void sfx_play_scroll();
    void sfx_play_click();
    void sfx_play_interact();
    void sfx_play_move();
    void sfx_play_cant();
}
