#include "board_scn.h"

Pawn* get_cursor_pawn() {
    if (!cursor_shown)
        return NULL;
    return board_get_pawn(BOARD_POS(cursor_pos.x, cursor_pos.y));
}

pawn_gid_t get_clicked_pawn_gid() {
    if (!cursor_click)
        return -1;
    return board_get_tile(BOARD_POS(cursor_click_pos.x, cursor_click_pos.y))->pawn_gid;
}

Pawn* get_clicked_pawn() {
    if (!cursor_click)
        return NULL;
    return board_get_pawn(BOARD_POS(cursor_click_pos.x, cursor_click_pos.y));
}

void on_cursor_click_move(VPos16 dest_pos) {
    // get the already selected pawn
    int sel_pawn_gid = get_clicked_pawn_gid();
    ClassData* sel_pawn_cd = pawn_get_classdata(sel_pawn_gid);
    VPos16 sel_pawn_pos = cursor_click_pos;

    // now check the dest tile
    int dest_tid = BOARD_POS(dest_pos.x, dest_pos.y);
    BoardTile* dest_tile = board_get_tile(dest_tid);

    // check if there's a pawn in the dest tile
    if (dest_tile->pawn_gid >= 0) {
        // there is a pawn there
        pawn_gid_t dest_pawn_gid = dest_tile->pawn_gid;

        int ir = sel_pawn_cd->interact_range;

        // are we within interact range (ir)?
        BOOL within_ir = board_dist(sel_pawn_pos.x, sel_pawn_pos.y, dest_pos.x, dest_pos.y) <= ir;

        // tid of intermediate tile
        int interact_itmdt_tid = -1;

        if (!within_ir) {
            // try to find to the closest tile within ir
            // use that tile as the intermediate

            // initialize to starting tile
            int closest_neighbor_tid = -1;
            int closest_neighbor_dist = -1;

            // check all neighbors
            for (int nx = -ir; nx <= ir; nx++) {
                for (int ny = -ir; ny <= ir; ny++) {
                    // get neighbor tile info
                    VPos16 nb_pos = (VPos16){.x = dest_pos.x + nx, .y = dest_pos.y + ny};
                    int nb_tid = POS_TO_TID(nb_pos);

                    // ensure that from this tile, we're within ir
                    if (board_dist(nb_pos.x, nb_pos.y, dest_pos.x, dest_pos.y) > ir)
                        continue; // out of range

                    // make sure this tile is walkable
                    if (!board_util_is_walkable(nb_pos.x, nb_pos.y))
                        continue;

                    // make sure nobody else is standing there!
                    if (board_get_pawn(BOARD_POS(nb_pos.x, nb_pos.y)))
                        continue;

                    // test distance from start pos
                    int nb_test_dist = board_dist(cursor_click_pos.x, cursor_click_pos.y, nb_pos.x, nb_pos.y);

                    if (closest_neighbor_tid < 0 || nb_test_dist < closest_neighbor_dist) {
                        closest_neighbor_dist = nb_test_dist;
                        closest_neighbor_tid = nb_tid;
                    }
                }
            }

            if (closest_neighbor_tid > 0) {
                // the intermediate is the most convenient neighbor
                interact_itmdt_tid = closest_neighbor_tid;
            }
        } else {
            // we are already within ir
            interact_itmdt_tid = POS_TO_TID(sel_pawn_pos);
        }

        if (interact_itmdt_tid > 0) {
            // we have a valid intermediate
            VPos16 interact_itmdt_pos = board_util_tile_id_to_pos(interact_itmdt_tid);

            // move our pawn to the intermediate
            animate_pawn_move(sel_pawn_gid, cursor_click_pos, interact_itmdt_pos);
            // flash the dest pawn
            animate_pawn_flash(dest_pawn_gid, sel_pawn_gid, FALSE);

            // interact with the pawn
            mgba_printf(MGBA_LOG_ERROR, "interact (me: %d) with pawn (%d)", sel_pawn_gid, dest_tile->pawn_gid);

            // request_step = TRUE; // request step
        } else {
            // we can't reach this pawn, give up
            mgba_printf(MGBA_LOG_ERROR, "we couldn't reach this pawn");
        }
    } else {
        // request a move anim
        animate_pawn_move(sel_pawn_gid, cursor_click_pos, dest_pos);
    }

    // now unclick and set dirty
    cursor_click = FALSE;
    set_ui_dirty();

    return; // done
}

void on_cursor_try_click(VPos16 try_click_pos) {
    if (cursor_click) {
        // a pawn is already selected

        // get the already selected pawn
        int sel_pawn_gid = get_clicked_pawn_gid();

        BOOL try_click_is_valid_move = FALSE;
        // then check if the click is within the range
        for (int i = 0; i < cache_range_buf_filled; i++) {
            // for each tile that's in range
            VPos16 withinrange_pos = cache_range_buf[i];
            if (try_click_pos.x == withinrange_pos.x && try_click_pos.y == withinrange_pos.y) {
                // this click target is within range

                // ensure that the move is valid
                BOOL is_move_valid = pawn_util_is_valid_move(sel_pawn_gid, cursor_click_pos, try_click_pos);

                if (!is_move_valid)
                    break;

                try_click_is_valid_move = TRUE;
            }
        }

        if (try_click_is_valid_move) {
            on_cursor_click_move(try_click_pos);
            return;
        }

        // if we got here, then the click wasn't within range
        // unclick
        cursor_click = FALSE;
        set_ui_dirty();
    } else if (get_cursor_pawn()) {
        // nothing is currently selected, but our cursor is over a pawn
        // set that pawn as clicked
        cursor_click = TRUE;
        cursor_click_pos = try_click_pos;
        pawn_move_range_dirty = TRUE;
        set_ui_dirty();
    }
}