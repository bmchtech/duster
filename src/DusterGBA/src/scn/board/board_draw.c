#include "board_scn.h"

VPos16 board_vpos_to_pix_pos(int tx, int ty) {
    VPos16 ret;
    ret.x = board_offset.x + (tx << 3);
    ret.y = board_offset.y + (ty << 3);
    return ret;
}

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

void draw_footstep(int tx, int ty) {
    int x1 = (board_offset.x) + (tx * 8);
    int y1 = (board_offset.y) + (ty * 8);

    schr4c_plot(&bg0_srf, x1 + 2, y1 + 2, 2);
    schr4c_plot(&bg0_srf, x1 + 2, y1 + 5, 2);
    schr4c_plot(&bg0_srf, x1 + 5, y1 + 5, 2);
    schr4c_plot(&bg0_srf, x1 + 5, y1 + 2, 2);
}

void draw_board() {
    if (board_ui_dirty) {
        // clear whole bg ui surface
        memset32(tile_mem[bg0_srf_cbb], 0, 4096);          // clear cbb
        schr4c_prep_map(&bg0_srf, se_mem[bg0_srf_sbb], 0); // set whole map to 0

        // draw outline
        draw_board_outline();

        // draw cursor
        if (cursor_shown)
            draw_board_cursor();

        // no longer dirty
        board_ui_dirty = false;
    }

    // start assigning sprites from sprite M, and every time a new pawn is found increment the counter
    int pawn_sprite_ix = 1;

    cc_hashtable_remove_all(pawn2sprite);

    // draw the pawns
    for (int by = 0; by < game_state.board_size; by++) {
        for (int bx = 0; bx < game_state.board_size; bx++) {
            BoardTile* tile = &game_state.board.tiles[BOARD_POS(bx, by)];
            if (tile->pawn_gid >= 0) {
                // this is a pawn

                // look up the pawn
                Pawn* pawn = game_get_pawn_by_gid(tile->pawn_gid);

                int team_ix = tile->pawn_gid / TEAM_MAX_PAWNS;

                int pawn_sprite_id = pawn_sprite_ix;

                SpritePawnPair* pair = &sprite_pawn_pairs[pawn_sprite_id];
                *pair = (SpritePawnPair){.pawn_gid = tile->pawn_gid, .sprite = pawn_sprite_id};

                cc_hashtable_add(pawn2sprite, &pair->pawn_gid, &pair->sprite);

                // mgba_printf(MGBA_LOG_ERROR, "set 2sprite k: %d, v: %d", pair->pawn_gid, pair->sprite);

                // assign a sprite to drawing this pawn
                dusk_sprites_make(pawn_sprite_id, 8, 8,
                                  (Sprite){
                                      .x = board_offset.x + (bx << 3),
                                      .y = board_offset.y + (by << 3),
                                      .base_tid = pawn->unit_class + (team_ix * NUM_UNIT_CLASSES),
                                  });

                pawn_sprite_ix++;
            }
        }
    }

    // check if pawn selected
    Pawn* clicked_pawn = get_clicked_pawn();
    if (cursor_click && clicked_pawn) {
        VPos16 pawn_pos = cursor_click_pos;
        ClassData* class_data = &game_data.class_data[clicked_pawn->unit_class];

        // draw footsteps for all tiles in range
        const int range_buf_len = 128;
        VPos range_buf[range_buf_len];
        int range_buf_filled =
            board_util_calc_rangebuf(pawn_pos.x, pawn_pos.y, class_data->move, range_buf, range_buf_len);

        for (int i = 0; i < range_buf_filled; i++) {
            VPos fs_pos = range_buf[i];
            draw_footstep(fs_pos.x, fs_pos.y);
        }

        // for (int i = -class_data->move; i <= class_data->move; i++) {
        //     for (int j = -class_data->move; j <= class_data->move; j++) {
        //         if (i == 0 && j == 0)
        //             continue;

        //         int tx = pawn_pos.x + i;
        //         int ty = pawn_pos.y + j;
        //         if (!is_on_board(tx, ty))
        //             continue;

        //         if (board_dist(pawn_pos.x, pawn_pos.y, tx, ty) > class_data->move)
        //             continue;

        //         draw_footstep(tx, ty);
        //     }
        // }
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
    Pawn* clicked_pawn = get_clicked_pawn();

    if (cursor_click && clicked_pawn) {
        // show pawn info
        ClassData* class_data = &game_data.class_data[clicked_pawn->unit_class];
        tte_printf("#{P:142,6}#{ci:1}class: %s", class_data->name);
        tte_printf("#{P:142,14}#{ci:1}move: %d", class_data->move);
    }
}