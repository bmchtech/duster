#include "board_scn.h"

Pawn* get_cursor_pawn() {
    if (!cursor_shown)
        return NULL;
    return board_get_pawn(BOARD_POS(cursor_pos.x, cursor_pos.y));
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
        int clicked_pawn_gid = board_get_tile(BOARD_POS(cursor_click_pos.x, cursor_click_pos.y))->pawn_gid;

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

                BoardTile* dest_tile = board_get_tile(BOARD_POS(dest_pos.x, dest_pos.y));
                if (dest_tile->pawn_gid >= 0) {
                    // there is a pawn there
                    // interact with the pawn
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
        set_ui_dirty();
    }
}