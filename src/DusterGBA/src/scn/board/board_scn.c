#include "board_scn.h"

TSurface bg0_srf;
int bg0_srf_cbb = 0;
int bg0_srf_sbb = 31;
VPos board_offset;
bool bg_ui_dirty = true;
int game_turn = 0;
VPos16 cursor_pos;
bool cursor_down = true;
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
    tte_init_chr4c(1, BG_CBB(2) | BG_SBB(29), 0, 0x0201, CLR_GRAY, NULL, NULL);
    REG_BG1CNT |= BG_PRIO(2);
    tte_init_con();

    // set bg palette
    pal_bg_mem[0] = RES_PAL[2]; // bg color
    pal_bg_mem[1] = RES_PAL[0]; // draw col 1
    pal_bg_mem[2] = RES_PAL[3]; // draw col 2

    mgba_printf(MGBA_LOG_INFO, "bean");

    // pawn spritesheet
    SpriteAtlas atlas = dusk_load_atlas("a_pawn");
    dusk_sprites_upload_atlas(&atlas);

    game_clear_state(); // reset game state

    // set up new game
    game_init_board(16);
    game_init_team(0, "player");

    Team* team = &game_state.teams[0];
    team_set_pawn(team, 0, 0); // first unit is soldier
    team_set_pawn(team, 1, 1); // second unit is horse

    board_set_pawn(BOARD_POS(0, 0), PAWN_GID(0, 0)); // pawn #0
    board_set_pawn(BOARD_POS(3, 2), PAWN_GID(0, 1)); // pawn #1

    // set vars for drawing
    board_offset = (VPos){.x = 8, .y = 8};
    cursor_pos = (VPos16){.x = 0, .y = 0};

    tte_printf("#{P:8,140}#{ci:1}turn: %s", game_state.teams[game_turn].name);
}

void boardscn_input() {
    // input
    int y_move = key_tri_vert();
    int x_move = key_tri_horz();

    u32 arrows_touched = key_transit(KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN);

    if ((arrows_touched || cursor_last_moved_frame < (frame_count - 6)) && (x_move != 0 || y_move != 0)) {
        // move cursor
        cursor_pos.x += x_move;
        cursor_pos.y += y_move;

        // clamp
        if (cursor_pos.x < 0)
            cursor_pos.x = game_state.board_size - 1;
        if (cursor_pos.x >= game_state.board_size)
            cursor_pos.x = 0;
        if (cursor_pos.y < 0)
            cursor_pos.y = game_state.board_size - 1;
        if (cursor_pos.y >= game_state.board_size)
            cursor_pos.y = 0;

        cursor_last_moved_frame = frame_count;

        if (cursor_down) {
            // need to redraw bg
            bg_ui_dirty = true;
        }
    }

    if (key_hit(KEY_SELECT)) {
        cursor_down = !cursor_down; // toggle cursor
        bg_ui_dirty = true;
    }
}

void boardscn_update() {
    dusk_frame();

    boardscn_input();
    draw_board();

    // update sprites
    dusk_sprites_update();
}

void boardscn_end() {}

Scene board_scene = {
    .start = boardscn_start,
    .update = boardscn_update,
    .end = boardscn_end,
};