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
    // check if pawn there
    if (get_cursor_pawn()) {
        // there is pawn
        // set the new click pos
        cursor_click = TRUE;
        cursor_click_pos = try_click_pos;
        set_ui_dirty();
    } else if (cursor_click) {
        // a pawn is already selected

        // get that pawn
        // Pawn* clicked_pawn = get_clicked_pawn();
        // get pawn gid
        int clicked_pawn_gid = board_get_tile(BOARD_POS(cursor_click_pos.x, cursor_click_pos.y))->pawn_gid;

        // then check if the click is within the range
        for (int i = 0; i < cache_range_buf_filled; i++) {
            VPos scan_pos = cache_range_buf[i];
            if (try_click_pos.x == scan_pos.x && try_click_pos.y == scan_pos.y) {
                // this click is within range
                // request a move anim
                animate_pawn_move(clicked_pawn_gid, vpos_from_vpos16(cursor_click_pos),
                                  vpos_from_vpos16(try_click_pos));

                // now unclick and set dirty
                cursor_click = FALSE;
                set_ui_dirty();
            }
        }
    }
}