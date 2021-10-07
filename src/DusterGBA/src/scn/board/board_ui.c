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

void on_cursor_try_click(VPos16 try_click_pos) {
    if (cursor_click) {
        // a pawn is already selected

        // get that pawn
        // Pawn* clicked_pawn = get_clicked_pawn();
        // get pawn gid
        int clicked_pawn_gid = get_clicked_pawn_gid();
        ClassData* clicked_pawn_class_data = pawn_get_classdata(clicked_pawn_gid);

        // then check if the click is within the range
        for (int i = 0; i < cache_range_buf_filled; i++) {
            VPos16 scan_pos = cache_range_buf[i];
            if (try_click_pos.x == scan_pos.x && try_click_pos.y == scan_pos.y) {
                // this click is within range
                // ensure valid
                BOOL is_move_valid = pawn_util_is_valid_move(clicked_pawn_gid, cursor_click_pos, try_click_pos);

                if (!is_move_valid)
                    break;

                // now check the dest tile
                VPos16 dest_pos = try_click_pos;
                int dest_tid = BOARD_POS(dest_pos.x, dest_pos.y);

                BoardTile* dest_tile = board_get_tile(dest_tid);
                if (dest_tile->pawn_gid >= 0) {
                    // there is a pawn there
                    pawn_gid_t dest_pawn_gid = dest_tile->pawn_gid;
                    // Pawn* dest_pawn = board_get_pawn(dest_tid);

                    // try to move to the closest tile within interact range ir
                    int ir = clicked_pawn_class_data->interact_range;

                    int closest_neighbor_tid = -1;
                    int closest_neighbor_dist = -1;

                    for (int nx = -ir; nx <= ir; nx++) {
                        for (int ny = -ir; ny <= ir; ny++) {
                            VPos16 nb_pos = (VPos16){.x = dest_pos.x + nx, .y = dest_pos.y + ny};
                            int nb_tid = POS_TO_TID(nb_pos);

                            int nb_dist_to_target = board_dist(nb_pos.x, nb_pos.y, dest_pos.x, dest_pos.y);
                            if (nb_dist_to_target > ir)
                                // out of range
                                continue;

                            if (!board_util_is_walkable(nb_pos.x, nb_pos.y))
                                continue;

                            mgba_printf(MGBA_LOG_ERROR, "checking (%d, %d)", nb_pos.x, nb_pos.y);
                            int nb_test_dist = board_dist(cursor_click_pos.x, cursor_click_pos.y, nb_pos.x, nb_pos.y);

                            if (closest_neighbor_tid < 0 || nb_test_dist < closest_neighbor_dist) {
                                closest_neighbor_dist = nb_test_dist;
                                closest_neighbor_tid = nb_tid;
                            }
                        }
                    }

                    if (closest_neighbor_tid > 0) {
                        // we have a valid intermediate
                        VPos16 interact_itmdt_pos = board_util_tile_id_to_pos(closest_neighbor_tid);
                        animate_pawn_move(clicked_pawn_gid, cursor_click_pos, interact_itmdt_pos);
                        animate_pawn_flash(dest_pawn_gid);

                        // // interact with the pawn
                        // mgba_printf(MGBA_LOG_ERROR, "interact (me: %d) with pawn (%d)", clicked_pawn_gid,
                        //             dest_tile->pawn_gid);

                        // request_step = TRUE; // request step
                    } else {
                        // we can't reach this pawn, give up
                        mgba_printf(MGBA_LOG_ERROR, "we couldn't reach this pawn");
                    }
                } else {
                    // request a move anim
                    animate_pawn_move(clicked_pawn_gid, cursor_click_pos, dest_pos);
                }

                // now unclick and set dirty
                cursor_click = FALSE;
                set_ui_dirty();

                return; // done
            }
        }

        // if we got here, then the click wasn't within range
        // unclick
        cursor_click = FALSE;
        set_ui_dirty();
    } else if (get_cursor_pawn()) {
        // check if pawn there
        // there is pawn
        // set the new click pos
        cursor_click = TRUE;
        cursor_click_pos = try_click_pos;
        pawn_move_range_dirty = TRUE;
        set_ui_dirty();
    }
}