#include "board_scn.h"

void foreach_pawn2sprite_key(const void* key) {
    // try getting the value and printing
    int* val_out;
    cc_hashtable_get(pawn2sprite, (void*)key, (void*)&val_out);
    int* key_out = (int*)key;

    mgba_printf(MGBA_LOG_ERROR, "pawn2sprite k: %d, v: %d", *key_out, *val_out);
}

void animate_pawn_move(s16 pawn_gid, VPos16 start_pos, VPos16 end_pos) {
    pawn_tween.start_pos = start_pos;
    pawn_tween.end_pos = end_pos;
    pawn_tween.pawn_gid = pawn_gid;
    pawn_tween.start_frame = frame_count;
    pawn_tween.end_frame = frame_count + 6;
}

void update_pawn_tween() {
    // log mappings

    // CC_Array* pawn2sprite_keys;
    // cc_hashtable_get_keys(pawn2sprite, &pawn2sprite_keys);
    // cc_array_destroy(pawn2sprite_keys);
    // mgba_printf(MGBA_LOG_ERROR, "pawn2sprite size: %d", cc_array_size(pawn2sprite_keys));

    // cc_hashtable_foreach_key(pawn2sprite, foreach_pawn2sprite_key);

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

    VPos16 start_pix_pos = board_vpos_to_pix_pos(pawn_tween.start_pos.x, pawn_tween.start_pos.y);
    VPos16 end_pix_pos = board_vpos_to_pix_pos(pawn_tween.end_pos.x, pawn_tween.end_pos.y);

    int dx = end_pix_pos.x - start_pix_pos.x;
    int dy = end_pix_pos.y - start_pix_pos.y;

    int x_step = dx / tween_len; // pix per frame
    int y_step = dy / tween_len; // pix per frame

    // mgba_printf(MGBA_LOG_ERROR, "sr: %d, st: %d", x_step_rate, curr_x_step);
    int x_prog = start_pix_pos.x + (frame_prog * x_step);
    int y_prog = start_pix_pos.y + (frame_prog * y_step);

    pawn_sprite->x = x_prog;
    pawn_sprite->y = y_prog;
}