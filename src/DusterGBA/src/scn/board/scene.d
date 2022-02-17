module scn.board.scene;

import core.stdc.string;
import core.stdc.stdio;
import dusk;
import tonc;
import libgba.maxmod;
import res;
import dusk.contrib.mgba;
import typ.vpos;
import libtind.ds.vector;
import scn.board;
import game;

extern (C):

void boardscn_init_vars() {
    board_offset = VPos(8, 8);
    cursor_pos = VPos16(0, 0);
    board_ui_dirty = true;
    sidebar_dirty = true;
    cursor_shown = true;
    cursor_click = false;
    cursor_last_moved_frame = 0;
    request_step = false;
    pausemenu_dirty = true;
    board_scroll_x = 0;
    board_scroll_y = 0;
    sidebar_page = 0;
    movequeue_progress = -1;
    movequeue_delay_timer = 0;
    ai_played_move = -1;
    ai_wait_timer = AI_WAIT_TIME;

    cache_range_vec.clear();
    pawn2sprite.clear();
    movequeue_queue.clear();
}

void boardscn_start() {
    // init
    dusk_init_graphics_mode0();
    dusk_sprites_init();
    dusk_sprites_configure(false);

    // init rng
    sqran(frame_count ^ 0xBEEF1234);

    // main bg
    *REG_DISPCNT |= DCNT_BG0;
    *REG_BG0CNT = cast(u16)(BG_CBB(BG0_SRF_CBB) | BG_SBB(BG0_SRF_SBB) | BG_PRIO(0));

    // set up bg0 as a drawing surface
    srf_init(&bg0_srf, ESurfaceType.SRF_CHR4C, cast(void*) tile_mem[BG0_SRF_CBB], 240, 160, 4, pal_bg_mem);
    schr4c_prep_map(&bg0_srf, cast(ushort*) se_mem[BG0_SRF_SBB], cast(u16) 0); // set whole map to 0

    // set up bg1 with tte
    *REG_DISPCNT |= DCNT_BG1;
    *REG_BG1CNT |= BG_PRIO(2);
    tte_init_chr4c(1, cast(u16)(BG_CBB(BG1_TTE_CBB) | BG_SBB(BG1_TTE_SBB)), 0, 0x0201, CLR_GRAY, null, null);
    tte_init_con();

    // set bg palette
    pal_bg_mem[0] = RES_PAL[2]; // bg color
    pal_bg_mem[1] = RES_PAL[0]; // draw col 1
    pal_bg_mem[2] = RES_PAL[3]; // draw col 2

    // pawn spritesheet
    SpriteAtlas atlas = dusk_load_atlas(cast(char*) "a_pawn");
    dusk_sprites_upload_atlas(&atlas);

    // initialize game
    game_clear_state();
    game_load_cold_data();
    game_init();

    // load gamemap
    u32 test1_gmp_len;
    char[48] gamemap_file_name;
    sprintf(cast(char*) gamemap_file_name, "%s.gmp.bin", cast(char*) selected_map_file);
    mgba_printf(MGBALogLevel.INFO, "loading map file %s\n", cast(char*) gamemap_file_name);
    const void* test1_gmp = dusk_load_raw(cast(char*) gamemap_file_name, &test1_gmp_len);
    mgba_printf(MGBALogLevel.INFO, "loading gamemap from tmx data[%d]", test1_gmp_len);
    bool load_success = game_load_gamemap(cast(void*) test1_gmp, test1_gmp_len);

    if (!load_success) {
        mgba_printf(MGBALogLevel.ERROR, "gamemap load failed");
    }

    // set vars for drawing
    boardscn_init_vars();

    // clear tweens
    pawn_move_tween.pawn_gid = -1;
    pawn_flash_tween.pawn_gid = -1;

    // change page
    boardscn_change_page(BoardScenePage.BOARD);

    // play start game sfx
    sfx_play_startchime();
}

void set_ui_dirty() {
    board_ui_dirty = true;
    sidebar_dirty = true;
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
        cursor_click = false;

        set_ui_dirty();
    }

    if (key_hit(KEY_START)) {
        // // pause menu
        // if (board_scene_page == BoardScenePage.BOARD)
        //     boardscn_change_page(BoardScenePage.TURNOVERLAY);
        // else if (board_scene_page == BoardScenePage.TURNOVERLAY)
        //     boardscn_change_page(BoardScenePage.BOARD);
        if (board_scene_page == BoardScenePage.BOARD) {
            pausemenu_dirty = true;
            boardscn_change_page(BoardScenePage.PAUSEMENU);
        } else if (board_scene_page == BoardScenePage.PAUSEMENU) {
            boardscn_change_page(BoardScenePage.BOARD);
        }

        pausemenu_dirty = true;
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
    if (whose_move != HUMAN_PLAYER_TEAM) {
        import ai.players;

        // initialize the move queue
        movequeue_queue.clear();

        // call the planner to plan moves for this team
        int num_moves_planned = 0;
        if (whose_move == 0) {
            num_moves_planned = ai_plan_moves_variant_1(whose_move, &movequeue_queue);
        }
        if (whose_move == 1) {
            num_moves_planned = ai_plan_moves_variant_1(whose_move, &movequeue_queue);
        }

        // log planned moves
        mgba_printf(MGBALogLevel.ERROR, "ai (team %d) planned %d moves", whose_move, num_moves_planned);
        // set variables for move queue
        movequeue_progress = -1; // indicates ready movequeue
    }
}

void boardscn_update() {
    dusk_frame();

    // logic
    if (request_step) {
        request_step = false;
        game_logic_step();
    }

    // input

    boardscn_input();

    boardscn_draw_page(board_scene_page);
}

void boardscn_change_page(BoardScenePage next_page) {
    switch (next_page) {
    case BoardScenePage.BOARD:
        // show bg0-1 only
        *REG_DISPCNT &= ~(DCNT_BG2);
        *REG_DISPCNT |= DCNT_BG0 | DCNT_BG1;
        break;
    case BoardScenePage.PAUSEMENU:
        break;
    case BoardScenePage.TURNOVERLAY:
        // show bg2 only
        *REG_DISPCNT &= ~(DCNT_BG0 | DCNT_BG1);
        *REG_DISPCNT |= DCNT_BG2;
        *REG_BG2CNT |= BG_PRIO(3);
        break;
    default:
        break;
    }

    board_scene_page = next_page;
}

void boardscn_draw_page(BoardScenePage page) {
    switch (page) {
    case BoardScenePage.BOARD:
        draw_sidebar();
        draw_board();

        // update ai move play
        update_ai_moveplay();

        // step queued
        update_queued_moves();

        // tween updates are last
        update_pawn_tweens();

        // update sprites
        dusk_sprites_update();

        break;
    case BoardScenePage.PAUSEMENU:
        update_pause_ui();
        draw_pause_ui();
        break;
    case BoardScenePage.TURNOVERLAY:
        // update_pause_ui();
        // draw_pause_ui();

        GritImage bg_img = dusk_load_image(cast(char*) "turn_tp");
        dusk_background_upload_raw(&bg_img, BG2_GFX_CBB, BG2_GFX_SBB, 32);
        dusk_background_make(2, BG_REG_32x32, Background( /*x*/ 0, /*y*/ 0, /*cbb*/ BG2_GFX_CBB, /*sbb*/ BG2_GFX_SBB));

        break;
    default:
        break;
    }
}

void boardscn_end() {
    // clean ds
    pawn2sprite.clear();
}

__gshared Scene board_scene = Scene(&boardscn_start, &boardscn_end, &boardscn_update);
