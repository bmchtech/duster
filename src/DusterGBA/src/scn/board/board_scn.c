#include "board_scn.h"

TSurface bg0_srf;
int bg0_srf_cbb = 0;
int bg0_srf_sbb = 31;
int bg1_tte_cbb = 2;
int bg1_tte_sbb = 28;
VPos board_offset;
BOOL board_ui_dirty = TRUE;
BOOL sidebar_dirty = TRUE;
int game_turn = 0;
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

int cursor_last_moved_frame = 0;

void boardscn_start() {
    // init
    dusk_init_graphics_mode0();
    dusk_sprites_init();
    dusk_sprites_configure(FALSE);

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

    // test loading data
    // u32 d_class_len;
    // u8* d_class = (u8*) dusk_load_raw("d_class.bin", &d_class_len);
    // dusk_load_raw("d_class.bin", &d_class_len);

    // pawn spritesheet
    SpriteAtlas atlas = dusk_load_atlas("a_pawn");
    dusk_sprites_upload_atlas(&atlas);

    game_clear_state(); // reset game state

    // set up new game
    game_load_cold_data();
    game_init();
    game_init_board(16);
    game_init_team(0, "player");
    game_init_team(1, "enmy");

    Team* team0 = &game_state.teams[0];
    team_set_pawn_t(team0, 0, 0); // first unit is soldier
    team_set_pawn_t(team0, 1, 1); // second unit is horse
    team_set_pawn_t(team0, 2, 3); // third unit is mage

    Team* team1 = &game_state.teams[1];
    team_set_pawn_t(team1, 0, 1);
    team_set_pawn_t(team1, 1, 2);

    board_set_pawn(BOARD_POS(0, 0), PAWN_GID(0, 0)); // pawn #0
    board_set_pawn(BOARD_POS(3, 2), PAWN_GID(0, 1)); // pawn #1
    board_set_pawn(BOARD_POS(2, 4), PAWN_GID(0, 2)); // pawn #2

    board_set_pawn(BOARD_POS(2, 9), PAWN_GID(1, 0));
    board_set_pawn(BOARD_POS(3, 8), PAWN_GID(1, 1));

    board_set_terrain(BOARD_POS(1, 3), TERRAIN_BLOCKED);
    board_set_terrain(BOARD_POS(2, 3), TERRAIN_BLOCKED);
    board_set_terrain(BOARD_POS(3, 3), TERRAIN_BLOCKED);

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

    // set test tween
    // animate_pawn_move(PAWN_GID(0, 0), (VPos){.x = 0, .y = 0}, (VPos){.x = 10, .y = 10});
    // animate_pawn_flash(PAWN_GID(0, 0));
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
        // move cursor
        cursor_pos.x += x_move;
        cursor_pos.y += y_move;

        // ensure cursor position is clamped
        if (cursor_pos.x < 0)
            cursor_pos.x = game_state.board_size - 1;
        if (cursor_pos.x >= game_state.board_size)
            cursor_pos.x = 0;
        if (cursor_pos.y < 0)
            cursor_pos.y = game_state.board_size - 1;
        if (cursor_pos.y >= game_state.board_size)
            cursor_pos.y = 0;

        cursor_last_moved_frame = frame_count;

        // cursor_click = FALSE;

        // need to redraw bg
        set_ui_dirty();
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

    if (cursor_shown && key_hit(KEY_A)) {
        // click input
        on_cursor_try_click(cursor_pos);
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