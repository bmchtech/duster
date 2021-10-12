#include "board_scn.h"

// the largest square able to be visible

VPos16 board_vpos_to_pix_pos(int tx, int ty) {
    VPos16 ret;
    ret.x = board_offset.x + (tx << 3);
    ret.y = board_offset.y + (ty << 3);
    return ret;
}

// void srfbg_draw_rect() {

// }

void draw_board_outline() {
    // draw the board outline
    int x1 = (board_offset.x);
    int x2 = (board_offset.x) + (BOARD_SCROLL_WINDOW * 8);
    int y1 = (board_offset.y);
    int y2 = (board_offset.y) + (BOARD_SCROLL_WINDOW * 8);

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

void draw_square1(int tx, int ty, int square_sz) {
    int sq1 = 0 + square_sz;
    int sq2 = 7 - square_sz;
    int x1 = (board_offset.x) + (tx * 8);
    int x2 = x1 + sq2;
    int y1 = (board_offset.y) + (ty * 8);
    int y2 = y1 + sq2;

    schr4c_hline(&bg0_srf, x1 + sq1, y1 + sq1, x2, 2);
    schr4c_vline(&bg0_srf, x1 + sq1, y1 + sq1, y2, 2);
    schr4c_hline(&bg0_srf, x1 + sq1, y2, x2, 2);
    schr4c_vline(&bg0_srf, x2, y1 + sq1, y2, 2);
}

typedef enum {
    BLOCKED_PATTERN_SOLID,
    BLOCKED_PATTERN_STRIPE,
} BlockedPattern;

void draw_blocked_tile(int tx, int ty, BlockedPattern pattern) {
    int x1 = (board_offset.x) + (tx * 8);
    int x2 = x1 + 7;
    int y1 = (board_offset.y) + (ty * 8);
    int y2 = y1 + 8;

    switch (pattern) {
    case BLOCKED_PATTERN_SOLID:
        // filled
        // schr4c_rect(&bg0_srf, x1, y1, x2, y2, 1);
    case BLOCKED_PATTERN_STRIPE:
        // stripes
        for (int i = 0; i < (y2 - y1); i += 2) {
            schr4c_hline(&bg0_srf, x1, y1 + i, x2, 1);
        }
    default:
        break;
    }
}

void draw_clear_ui_surface() {
    // clear whole bg ui surface
    memset32(tile_mem[bg0_srf_cbb], 0, 4096);          // clear cbb
    schr4c_prep_map(&bg0_srf, se_mem[bg0_srf_sbb], 0); // set whole map to 0
}

void draw_clear_text_surface() {
    tte_erase_screen(); // clear tte bg
}

void draw_clicked_pawn_graphics() {
    // check if pawn selected
    Pawn* clicked_pawn = get_clicked_pawn();
    if (cursor_click && clicked_pawn) {
        VPos16 pawn_pos = cursor_click_pos;
        ClassData* class_data = &game_data.class_data[clicked_pawn->unit_class];

        // draw footsteps for all tiles in range

        // compute range buf if needed
        if (pawn_move_range_dirty) {
            pawn_move_range_dirty = FALSE;

            cache_range_buf_filled = board_util_calc_rangebuf(pawn_pos.x, pawn_pos.y, class_data->move, cache_range_buf,
                                                              CACHE_RANGE_BUF_LEN);
            if (cache_range_buf_filled <= 0) {
                // calc range failed
                mgba_printf(MGBA_LOG_ERROR, "calculate range buf for pawn %d failed (code %d)!", get_clicked_pawn_gid(),
                            cache_range_buf_filled);
                // unclick
                cursor_click = FALSE;
                set_ui_dirty();
            }
        }

        // draw from cached buf
        if (cache_range_buf_filled > 0) {
            // range buf is filled
            for (int i = 0; i < cache_range_buf_filled; i++) {
                VPos16 fs_pos = cache_range_buf[i];
                if (board_get_pawn(BOARD_POS(fs_pos.x, fs_pos.y))) {
                    // there is a pawn
                    draw_square1(fs_pos.x, fs_pos.y, 2);
                } else {
                    draw_footstep(fs_pos.x, fs_pos.y);
                }
            }
        }
    }
}

void draw_board() {
    if (board_ui_dirty) {
        board_ui_dirty = false;

        draw_clear_ui_surface();

        // draw outline
        draw_board_outline();

        // draw cursor
        if (cursor_shown)
            draw_board_cursor();

        draw_clicked_pawn_graphics();
    }

    // start assigning sprites from sprite M, and every time a new pawn is found increment the counter
    int pawn_sprite_ix = 1;

    // hide all sprites from M to NUM_SPRITES
    for (int i = pawn_sprite_ix; i < NUM_SPRITES; i++) {
        sprites[i].flags &= ~DUSKSPRITE_FLAGS_VISIBLE; // set not visible
    }

    cc_hashtable_remove_all(pawn2sprite);

    // get board window pos
    int bwy = board_scroll_y;
    int bwx = board_scroll_x;

    // go through all tiles
    // brx and bry mean "board real x"
    for (int bry = bwy; bry < bwy + BOARD_SCROLL_WINDOW; bry++) {
        for (int brx = bwx; brx < bwx + BOARD_SCROLL_WINDOW; brx++) {
            // get "draw cooardinate" x and y, which subtracts the window offset
            int bdx = brx - bwx;
            int bdy = bry - bwy;

            BoardTile* tile = &game_state.board.tiles[BOARD_POS(brx, bry)];
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
                                      .x = board_offset.x + (bdx << 3),
                                      .y = board_offset.y + (bdy << 3),
                                      .base_tid = pawn->unit_class + (team_ix * NUM_UNIT_CLASSES),
                                  });

                pawn_sprite_ix++;
            }
            if (tile->terrain > 0) {
                switch (tile->terrain) {
                case TERRAIN_BLOCKED:
                    draw_blocked_tile(bdx, bdy, BLOCKED_PATTERN_STRIPE);
                default:
                    break;
                }
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

    draw_clear_text_surface();

    // turn indicator
    tte_printf("#{P:8,140}#{ci:1}turn: %s", game_state.teams[game_turn].name);

    // currently hovered pawn
    Pawn* clicked_pawn = get_clicked_pawn();
    pawn_gid_t clicked_pawn_gid = get_clicked_pawn_gid();

    if (cursor_click && clicked_pawn) {
        // show pawn info
        ClassData* class_data = &game_data.class_data[clicked_pawn->unit_class];
        UnitData* unit_data = &clicked_pawn->unit_data;

        int pawn_team_ix = clicked_pawn_gid / TEAM_MAX_PAWNS;

        tte_printf("#{P:142,6}#{ci:1}class: %s", class_data->name);
        tte_printf("#{P:142,14}#{ci:1}hp: %d", unit_data->hitpoints);
        tte_printf("#{P:142,22}#{ci:1}stats: %d | %d | %d", unit_data->stats.atk, unit_data->stats.def,
                   unit_data->stats.hp);
    } else {
        int hover_tid = POS_TO_TID(cursor_pos);
        tte_printf("#{P:142,6}#{ci:1}tid: %d", hover_tid);
        BoardTile* tile = board_get_tile(hover_tid);
        tte_printf("#{P:142,14}#{ci:1}pawn: %d", tile->pawn_gid);
    }
}