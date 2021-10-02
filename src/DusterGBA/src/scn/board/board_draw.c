#include "board_scn.h"

void draw_board_outline() {
    // draw the board outline
    int x1 = (board_offset.x);
    int x2 = (board_offset.x) + (game_state.board_size * 8);
    int y1 = (board_offset.y);
    int y2 = (board_offset.y) + (game_state.board_size * 8);

    schr4c_hline(&bg0_srf, x1 - 1, y1 - 1, x2, 1);
    schr4c_vline(&bg0_srf, x1 - 1, y1 - 1, y2, 1);
    schr4c_hline(&bg0_srf, x1 - 1, y2, x2, 1);
    schr4c_vline(&bg0_srf, x2, y1 - 1, y2, 1);
}

void draw_board_cursor() {
    int x1 = (board_offset.x) + (cursor_pos.x * 8);
    int x2 = x1 + 7;
    int y1 = (board_offset.y) + (cursor_pos.y * 8);
    int y2 = y1 + 7;

    if (cursor_click) {
        // cursor block
        schr4c_rect(&bg0_srf, x1, y1, x2 + 1, y2 + 1, 2);
    } else {
        // cursor outline
        schr4c_hline(&bg0_srf, x1, y1, x2, 2);
        schr4c_vline(&bg0_srf, x1, y1, y2, 2);
        schr4c_hline(&bg0_srf, x1, y2, x2, 2);
        schr4c_vline(&bg0_srf, x2, y1, y2, 2);
    }
}

void draw_board() {
    if (board_ui_dirty) {
        // clear whole bg ui surface
        memset32(tile_mem[bg0_srf_cbb], 0, 4096);          // clear cbb
        schr4c_prep_map(&bg0_srf, se_mem[bg0_srf_sbb], 0); // set whole map to 0

        // draw outline
        draw_board_outline();

        // draw cursor
        if (cursor_down)
            draw_board_cursor();

        // no longer dirty
        board_ui_dirty = false;
    }

    // start assigning sprites from sprite M, and every time a new pawn is found increment the counter
    int pawn_sprite_ix = 1;

    // draw the pawns
    for (int by = 0; by < game_state.board_size; by++) {
        for (int bx = 0; bx < game_state.board_size; bx++) {
            BoardTile* tile = &game_state.board.tiles[BOARD_POS(bx, by)];
            if (tile->pawn_gid >= 0) {
                // this is a pawn

                // look up the pawn
                Pawn* pawn = game_get_pawn_by_gid(tile->pawn_gid);

                // assign a sprite to drawing this pawn
                dusk_sprites_make(pawn_sprite_ix++, 8, 8,
                                  (Sprite){
                                      .x = board_offset.x + (bx << 3),
                                      .y = board_offset.y + (by << 3),
                                      .base_tid = pawn->unit_class,
                                  });
            }
        }
    }

    // // for each team
    // for (int i = 0; i < NUM_TEAMS; i++) {
    //     Team* team = &game_state.teams[i];
    //     // draw pawns
    //     for (int j = 0; j < TEAM_MAX_PAWNS; j++) {
    //         Pawn* pawn = &team->pawns[j];
    //     }
    // }
}

void draw_sidebar() {
    if (!sidebar_dirty)
        return;

    sidebar_dirty = FALSE;

    tte_erase_screen(); // clear tte bg

    // turn indicator
    tte_printf("#{P:8,140}#{ci:1}turn: %s", game_state.teams[game_turn].name);

    // currently hovered pawn
    Pawn* cursor_pawn = get_cursor_pawn();

    if (cursor_pawn) {
        int cursor_pawn_class = cursor_pawn->unit_class;
        tte_printf("#{P:180,8}#{ci:1}pawn: %d", cursor_pawn_class);
    }
}