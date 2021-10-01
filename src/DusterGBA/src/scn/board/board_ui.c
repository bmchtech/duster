#include "board_scn.h"

Pawn* get_cursor_pawn() {
    if (!cursor_down) return NULL;
    return board_get_pawn(BOARD_POS(cursor_pos.x, cursor_pos.y));
}