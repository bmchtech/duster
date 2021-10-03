#include "game.h"
#include <stdio.h>
#include <memory.h>
#include <tonc.h>
#include "cold_data.h"

BOOL board_util_is_on_board(int tx, int ty) {
    return ((tx >= 0) && tx < game_state.board_size) && ((ty >= 0) && ty < game_state.board_size);
}

VPos16 board_util_tile_id_to_pos(int tile_id) {
    VPos16 ret;
    ret.x = tile_id % MAX_BOARD_SIZE;
    ret.y = tile_id / MAX_BOARD_SIZE;
    return ret;
}

int board_util_calc_rangebuf(int start_tx, int start_ty, int range, VPos16* pos_buf, int pos_buf_len) {
    // clear rangebuf
    memset(pos_buf, 0, sizeof(VPos16) * pos_buf_len);

    int pos_buf_ix = 0;

    // scan in square
    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; j++) {
            if (i == 0 && j == 0)
                continue;

            int scan_tx = start_tx + i;
            int scan_ty = start_ty + j;
            if (!board_util_is_on_board(scan_tx, scan_ty))
                continue;

            if (board_dist(start_tx, start_ty, scan_tx, scan_ty) > range)
                continue;

            // // make sure no other pawn is there
            // if (board_get_pawn(BOARD_POS(scan_tx, scan_ty)))
            //     continue;

            // this is in range
            pos_buf[pos_buf_ix] = (VPos16){.x = scan_tx, .y = scan_ty};
            pos_buf_ix++;

            // if pos buf is full, leave
            if (pos_buf_ix >= pos_buf_len - 1)
                return pos_buf_ix;
        }
    }

    return pos_buf_ix;
}

BOOL pawn_util_is_valid_move(pawn_gid_t pawn_gid, VPos16 start_pos, VPos16 end_pos) {
    ClassData* class_data = pawn_get_classdata(pawn_gid);

    int pawn_max_move = class_data->move;

    // check dist from start to end tiles
    int start_end_dist = board_dist(start_pos.x, start_pos.y, end_pos.x, end_pos.y);

    // check if dist exceeds max move
    if (start_end_dist > pawn_max_move)
        return FALSE;

    return TRUE;
}

BOOL pawn_util_is_valid_interaction(pawn_gid_t pawn1_gid, VPos16 pawn1_pos, pawn_gid_t pawn2_gid, VPos16 pawn2_pos) {
    // ClassData* class_data = pawn_get_classdata(pawn_gid);

    // int pawn_max_move = class_data->move;

    // // check dist from start to end tiles
    // int start_end_dist = board_dist(start_pos.x, start_pos.y, end_pos.x, end_pos.y);

    // // check if dist exceeds max move
    // if (start_end_dist > pawn_max_move)
    //     return FALSE;

    return TRUE;
}