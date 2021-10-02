#include "board_scn.h"

void foreach_pawn2sprite_key(const void* key) {
    // try getting the value and printing
    int* val_out;
    cc_hashtable_get(pawn2sprite, (void*)key, (void*)&val_out);
    int* key_out = (int*)key;

    mgba_printf(MGBA_LOG_ERROR, "pawn2sprite k: %d, v: %d", *key_out, *val_out);
}

void update_pawn_tween() {
    // log mappings

    // CC_Array* pawn2sprite_keys;
    // cc_hashtable_get_keys(pawn2sprite, &pawn2sprite_keys);
    // cc_array_destroy(pawn2sprite_keys);
    // mgba_printf(MGBA_LOG_ERROR, "pawn2sprite size: %d", cc_array_size(pawn2sprite_keys));

    cc_hashtable_foreach_key(pawn2sprite, foreach_pawn2sprite_key);

    // // try getting
    // int* get_test_out;
    // int get_test_key = 32;
    // enum cc_stat get_test_stat = cc_hashtable_get(pawn2sprite, &get_test_key, (void*) &get_test_out);
    // if (get_test_stat == CC_OK) {
    //     mgba_printf(MGBA_LOG_ERROR, "pawn2sprite get test: %d = %d", get_test_key, *get_test_out);
    // } else {
    //     mgba_printf(MGBA_LOG_ERROR, "pawn2sprite get test fail: key: %d", get_test_key);
    // }

    if (pawn_tween.pawn_gid < 0)
        return;

    // now we tween
    int* pawn_sprite_ix_out;
    if (cc_hashtable_get(pawn2sprite, &pawn_tween.pawn_gid, (void*)&pawn_sprite_ix_out) != CC_OK) {
        mgba_printf(MGBA_LOG_ERROR, "failed to get sprite index for pawn gid: %d", pawn_tween.pawn_gid);
        return;
    }

    if (frame_count >= pawn_tween.end_frame) {
        // done
        // set real pos to end
        int pawn_old_pos = board_find_pawn_tile(pawn_tween.pawn_gid);
        board_move_pawn(pawn_tween.pawn_gid, pawn_old_pos, BOARD_POS(pawn_tween.end_pos.x, pawn_tween.end_pos.y));

        // clear tween info
        memset(&pawn_tween, 0, sizeof(PawnTweenInfo));
        pawn_tween.pawn_gid = -1;

        return;
    }

    // get the assigned sprite
    int pawn_sprite_ix = *pawn_sprite_ix_out;
    Sprite* pawn_sprite = &sprites[pawn_sprite_ix];

    // calculate the between vpos
    int tween_len = pawn_tween.end_frame - pawn_tween.start_frame;
    int frame_prog = frame_count - pawn_tween.start_frame;
    
    int dx = pawn_tween.end_pos.x - pawn_tween.start_pos.x;

    int x_step_rate = tween_len / dx; // frames per x pixel
    int curr_x_step = frame_prog / x_step_rate;

    // mgba_printf(MGBA_LOG_ERROR, "sr: %d, st: %d", x_step_rate, curr_x_step);
    int x_prog = pawn_tween.start_pos.x + (curr_x_step * 1);

    VPos16 pix_pos = board_vpos_to_pix_pos(pawn_tween.start_pos.x + x_prog, pawn_tween.end_pos.y);
    pawn_sprite->x = pix_pos.x;
    pawn_sprite->y = pix_pos.y;
}