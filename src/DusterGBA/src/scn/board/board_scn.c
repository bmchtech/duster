#include "board_scn.h"

TSurface bg0_srf;
int bg0_srf_cbb = 0;
int bg0_srf_sbb = 31;
int bg1_tte_cbb = 2;
int bg1_tte_sbb = 28;
VPos board_offset;
BOOL board_ui_dirty = TRUE;
BOOL sidebar_dirty = TRUE;
int cursor_last_moved_frame = 0;
VPos16 cursor_pos;
BOOL cursor_shown = TRUE;
BOOL cursor_click = FALSE;
VPos16 cursor_click_pos;
PawnMoveTweenInfo pawn_move_tween;
PawnFlashTweenInfo pawn_flash_tween;
CC_HashTable* pawn2sprite;
SpritePawnPair sprite_pawn_pairs[128];
VPos16 cache_range_buf[CACHE_RANGE_BUF_LEN];
int cache_range_buf_filled = 0;
BOOL request_step = FALSE;
BOOL pawn_move_range_dirty = TRUE;
BoardScenePage board_scene_page = BOARDSCN_BOARD;
BOOL pausemenu_dirty = TRUE;
int board_scroll_x = 0;
int board_scroll_y = 0;
int sidebar_page = 0;
int movequeue_length = 0;
QueuedMove movequeue_queue[MOVEQUEUE_MAX_SIZE];
int movequeue_progress = -1;
int movequeue_delay_timer = 0;
int ai_played_move = -1;
int ai_wait_timer = AI_WAIT_TIME;

void boardscn_start() {
    // init
    dusk_init_graphics_mode0();
    dusk_sprites_init();
    dusk_sprites_configure(FALSE);

    // init rng
    sqran(frame_count ^ 0xBEEF1234);

    // main bg
    REG_DISPCNT |= DCNT_BG0;
    REG_BG0CNT = BG_CBB(bg0_srf_cbb) | BG_SBB(bg0_srf_sbb) | BG_PRIO(0);

    // set up bg0 as a drawing surface
    srf_init(&bg0_srf, SRF_CHR4C, tile_mem[bg0_srf_cbb], 240, 160, 4, pal_bg_mem);
    schr4c_prep_map(&bg0_srf, se_mem[bg0_srf_sbb], 0); // set whole map to 0

    // set up bg1 with tte
    REG_DISPCNT |= DCNT_BG1;
    tte_init_chr4c(1, BG_CBB(bg1_tte_cbb) | BG_SBB(bg1_tte_sbb), 0, 0x0201, CLR_GRAY, NULL, NULL);
    REG_BG1CNT |= BG_PRIO(2);
    tte_init_con();

    // set bg palette
    pal_bg_mem[0] = RES_PAL[2]; // bg color
    pal_bg_mem[1] = RES_PAL[0]; // draw col 1
    pal_bg_mem[2] = RES_PAL[3]; // draw col 2

    // pawn spritesheet
    SpriteAtlas atlas = dusk_load_atlas("a_pawn");
    dusk_sprites_upload_atlas(&atlas);

    // initialize game
    game_clear_state();
    game_load_cold_data();
    game_init();

    // load gamemap
    u32 test1_gmp_len;
    char gamemap_file_name[48];
    sprintf(gamemap_file_name, "%s.gmp.bin", selected_map_file);
    mgba_printf(MGBA_LOG_ERROR, "loading map file %s\n", gamemap_file_name);
    const void* test1_gmp = dusk_load_raw(gamemap_file_name, &test1_gmp_len);
    mgba_printf(MGBA_LOG_ERROR, "loading gamemap from tmx data[%d]", test1_gmp_len);
    BOOL load_success = game_load_gamemap((void*)test1_gmp, test1_gmp_len);

    if (!load_success) {
        mgba_printf(MGBA_LOG_ERROR, "gamemap load failed");
    }

    // set vars for drawing
    board_offset = (VPos){.x = 8, .y = 8};
    cursor_pos = (VPos16){.x = 0, .y = 0};

    // clear tweens
    pawn_move_tween.pawn_gid = -1;
    pawn_flash_tween.pawn_gid = -1;

    // init data structures
    CC_HashTableConf pawn2sprite_conf;
    cc_hashtable_conf_init(&pawn2sprite_conf);
    pawn2sprite_conf.hash = GENERAL_HASH;
    pawn2sprite_conf.key_length = sizeof(pawn_gid_t);
    cc_hashtable_new_conf(&pawn2sprite_conf, &pawn2sprite);

    // play start game sfx
    boardscn_sfx_play_startchime();
}

void set_ui_dirty() {
    board_ui_dirty = TRUE;
    sidebar_dirty = TRUE;
}

void boardscn_input() {
    // input
    int y_move = key_tri_vert();
    int x_move = key_tri_horz();

    u32 arrows_touched = key_transit(KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN);

    if (cursor_shown && (arrows_touched || cursor_last_moved_frame < (frame_count - 6)) &&
        (x_move != 0 || y_move != 0)) {

        // store cursor last move frame
        cursor_last_moved_frame = frame_count;

        on_try_move_cursor(x_move, y_move);
    }

    if (key_hit(KEY_SELECT)) {
        cursor_shown = !cursor_shown; // toggle cursor
        cursor_click = FALSE;

        set_ui_dirty();
    }

    if (key_hit(KEY_START)) {
        // pause menu
        if (board_scene_page == BOARDSCN_BOARD)
            board_scene_page = BOARDSCN_PAUSEMENU;
        else if (board_scene_page == BOARDSCN_PAUSEMENU)
            board_scene_page = BOARDSCN_BOARD;

        pausemenu_dirty = TRUE;
        set_ui_dirty();
    }

    if (key_hit(KEY_L)) {
        sidebar_page = (sidebar_page + 1) % NUM_SIDEBAR_PAGES;
        set_ui_dirty();
    }

    if (cursor_shown && key_hit(KEY_A)) {
        // click input
        on_cursor_try_click(cursor_pos);
    }
}

void update_ai_moveplay() {
    // ensure move not already played
    if (ai_played_move >= game_state.turns) {
        return;
    }

    // wait for timer
    if (ai_wait_timer > 0) {
        ai_wait_timer--;
        return;
    }

    // mark this turn as played, and reset timer
    ai_played_move = game_state.turns;
    ai_wait_timer = AI_WAIT_TIME;

    // now check whose move it is
    int whose_move = game_util_whose_turn();

    // if it is the ai's turn, ask the ai to plan moves
    int human_player_team = -1;
    if (whose_move != human_player_team) {
        // initialize the move queue
        memset(movequeue_queue, 0, sizeof(movequeue_queue));

        // call the planner to plan moves for this team
        int num_moves_planned = 0;
        if (whose_move == 0) {
            num_moves_planned = game_gs_ai_plan_moves_variant_1(whose_move, movequeue_queue, MOVEQUEUE_MAX_SIZE);
        }
        if (whose_move == 1) {
            num_moves_planned = game_gs_ai_plan_moves_variant_2(whose_move, movequeue_queue, MOVEQUEUE_MAX_SIZE);
        }

        // log planned moves
        mgba_printf(MGBA_LOG_ERROR, "planning moves returned %d", num_moves_planned);
        // set variables for move queue
        movequeue_length = num_moves_planned;
        movequeue_progress = -1; // indicates ready movequeue
    }
}

void boardscn_update() {
    dusk_frame();

    // logic
    if (request_step) {
        request_step = FALSE;
        game_logic_step();
    }

    // input

    boardscn_input();

    // draw

    switch (board_scene_page) {
    case BOARDSCN_BOARD:
        draw_sidebar();
        draw_board();

        // tween updates are last
        update_pawn_tweens();

        // update ai move play
        update_ai_moveplay();

        // step queued
        update_queued_moves();

        // update sprites
        dusk_sprites_update();

        break;
    case BOARDSCN_PAUSEMENU:
        update_pause_ui();
        draw_pause_ui();

        break;
    }
}

void boardscn_end() {
    // clean ds
    cc_hashtable_destroy(pawn2sprite);
}

Scene board_scene = {
    .start = boardscn_start,
    .update = boardscn_update,
    .end = boardscn_end,
};