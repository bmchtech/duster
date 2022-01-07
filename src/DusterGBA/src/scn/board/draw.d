module scn.board.draw;

import core.stdc.string;
import core.stdc.stdio;
import dusk;
import tonc;
import libgba.maxmod;
import res;
import dusk.contrib.mgba;
import typ.vpos;
import scn.board;
import game;

// the largest square able to be visible

VPos16 board_vpos_to_pix_pos(int tx, int ty) {
    VPos16 ret;
    ret.x = board_offset.x + ((tx - board_scroll_x) << 3);
    ret.y = board_offset.y + ((ty - board_scroll_y) << 3);
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
    int x1 = (board_offset.x) + ((cursor_pos.x - board_scroll_x) * 8);
    int x2 = x1 + 7;
    int y1 = (board_offset.y) + ((cursor_pos.y - board_scroll_y) * 8);
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

void draw_canmove_indicator(int tx, int ty) {
    int x1 = (board_offset.x) + (tx * 8);
    int y1 = (board_offset.y) + (ty * 8);

    schr4c_hline(&bg0_srf, x1 + 1, y1 + 7, x1 + 6, 2);
}

// typedef enum {
//     BLOCKED_PATTERN_SOLID,
//     BLOCKED_PATTERN_STRIPE,
// } BlockedPattern;
enum BlockedPattern {
    BLOCKED_PATTERN_SOLID,
    BLOCKED_PATTERN_STRIPE,
}

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
    memset32(tile_mem[bg0_srf_cbb], 0, 4096); // clear cbb
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
        ClassData* class_data = &game_data.class_data[clicked_pawn.unit_class];

        // draw footsteps for all tiles in range

        // compute range buf if needed
        if (pawn_move_range_dirty) {
            pawn_move_range_dirty = FALSE;

            cache_range_buf_filled = board_util_calc_rangebuf(pawn_pos.x, pawn_pos.y, class_data.move, cache_range_buf,
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
                    draw_square1(fs_pos.x - board_scroll_x, fs_pos.y - board_scroll_y, 2);
                } else {
                    draw_footstep(fs_pos.x - board_scroll_x, fs_pos.y - board_scroll_y);
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
    int whose_turn = game_util_whose_turn();

    // go through all tiles
    // brx and bry mean "board real x"
    for (int bry = bwy; bry < bwy + BOARD_SCROLL_WINDOW; bry++) {
        for (int brx = bwx; brx < bwx + BOARD_SCROLL_WINDOW; brx++) {
            // get "draw cooardinate" x and y, which subtracts the window offset
            int bdx = brx - bwx;
            int bdy = bry - bwy;

            BoardTile* tile = &game_state.board.tiles[BOARD_POS(brx, bry)];
            pawn_gid_t pawn_gid = tile.pawn_gid;
            if (pawn_gid >= 0) {
                // this is a pawn
                // look up the pawn
                Pawn* pawn = game_get_pawn_by_gid(pawn_gid);

                int team_ix = PAWN_WHICH_TEAM(pawn_gid);

                int pawn_sprite_id = pawn_sprite_ix;

                SpritePawnPair* pair = &sprite_pawn_pairs[pawn_sprite_id];
                // *pair = (SpritePawnPair){.pawn_gid = pawn_gid, .sprite = pawn_sprite_id};
                *pair = SpritePawnPair(pawn_gid, pawn_sprite_id);

                cc_hashtable_add(pawn2sprite, &pair.pawn_gid, &pair.sprite);

                // mgba_printf(MGBA_LOG_ERROR, "set 2sprite k: %d, v: %d", pair.pawn_gid, pair.sprite);

                // assign a sprite to drawing this pawn
                // dusk_sprites_make(pawn_sprite_id, 8, 8,
                //                   (Sprite){
                //                       .x = board_offset.x + (bdx << 3),
                //                       .y = board_offset.y + (bdy << 3),
                //                       .base_tid = pawn.unit_class + (team_ix * NUM_UNIT_CLASSES),
                //                   });
                dusk_sprites_make(pawn_sprite_id, 8, 8,
                    Sprite(
                        board_offset.x + (bdx << 3),
                        board_offset.y + (bdy << 3),
                        pawn.unit_class + (team_ix * NUM_UNIT_CLASSES)));

                pawn_sprite_ix++;

                // if it's this pawn's turn, and it's able to move
                if (whose_turn == team_ix && !pawn_util_moved_this_turn(pawn)) {
                    // draw a "ready" indicator
                    draw_canmove_indicator(bdx, bdy);
                }
            }
            if (tile.terrain > 0) {
                switch (tile.terrain) {
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
    //         Pawn* pawn = &team.pawns[j];
    //     }
    // }
}

void draw_sidebar() {
    if (!sidebar_dirty)
        return;

    sidebar_dirty = FALSE;

    draw_clear_text_surface();

    // turn indicator
    int turn = game_util_whose_turn();
    tte_printf("#{P:8,140}#{ci:1}turn: %s", game_state.teams[turn].name);

    // sidebar pawn info
    int page = sidebar_page % NUM_SIDEBAR_PAGES;
    Pawn* click_pawn = get_clicked_pawn();
    Pawn* hover_pawn = get_cursor_pawn();
    int hover_pawn_gid = game_state.board.tiles[BOARD_POS(cursor_pos.x, cursor_pos.y)].pawn_gid;
    int hover_tid = POS_TO_TID(cursor_pos);

    if (click_pawn) {
        // show click pawn info
        ClassData* class_data = &game_data.class_data[click_pawn.unit_class];
        UnitData* unit_data = &click_pawn.unit_data;

        switch (page) {
        case 0:
            tte_printf("#{P:142,06}#{ci:1}cl: %s L%d", class_data.name, unit_data.level);
            tte_printf("#{P:142,14}#{ci:1}hp: %d", unit_data.hitpoints);
            tte_printf("#{P:142,22}#{ci:1}st: %d|%d|%d|%d", unit_data.stats.atk, unit_data.stats.def,
                unit_data.stats.hp, unit_data.stats.spd);
            break;
        default:
            break;
        }
    }

    if (hover_pawn && (hover_pawn != click_pawn)) {
        // show hover pawn info
        ClassData* class_data = &game_data.class_data[hover_pawn.unit_class];
        UnitData* unit_data = &hover_pawn.unit_data;

        switch (page) {
        case 0:
            tte_printf("#{P:142,96}#{ci:1}look");
            tte_printf("#{P:142,104}#{ci:1}cl: %s L%d", class_data.name, unit_data.level);
            tte_printf("#{P:142,112}#{ci:1}hp: %d", unit_data.hitpoints);
            tte_printf("#{P:142,120}#{ci:1}st: %d|%d|%d|%d", unit_data.stats.atk, unit_data.stats.def,
                unit_data.stats.hp, unit_data.stats.spd);
            break;
        case 1:
            if (click_pawn && hover_pawn) {
                // attack simulation damage prediction
                HostileUnitDuel duel = game_logic_calc_hostile_damage(click_pawn, hover_pawn);
                tte_printf("#{P:142,96}#{ci:1}dmg: %d", duel.main_dmg);
                tte_printf("#{P:142,104}#{ci:1}ctr: %d", duel.counter_dmg);
            }
            break;
        default:
            break;
        }
    }
}
