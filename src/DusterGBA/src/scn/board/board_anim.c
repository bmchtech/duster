#include "board_scn.h"

void animate_pawn_move(pawn_gid_t pawn_gid, VPos16 start_pos, VPos16 end_pos) {
    pawn_move_tween.start_pos = start_pos;
    pawn_move_tween.end_pos = end_pos;
    pawn_move_tween.pawn_gid = pawn_gid;
    pawn_move_tween.start_frame = frame_count;
    pawn_move_tween.end_frame = frame_count + 6;
}

void animate_pawn_flash(pawn_gid_t pawn_gid) {
    pawn_flash_tween.pawn_gid = pawn_gid;
    pawn_flash_tween.start_frame = frame_count;
    pawn_flash_tween.end_frame = frame_count + 6;
}

void update_pawn_move_tween() {
    if (pawn_move_tween.pawn_gid < 0)
        return;

    // now we tween
    int* pawn_sprite_ix_out;
    if (cc_hashtable_get(pawn2sprite, &pawn_move_tween.pawn_gid, (void*)&pawn_sprite_ix_out) != CC_OK) {
        mgba_printf(MGBA_LOG_ERROR, "failed to get sprite index for pawn gid: %d", pawn_move_tween.pawn_gid);
        return;
    }

    if (frame_count >= pawn_move_tween.end_frame) {
        // done
        // set real pos to end
        int pawn_old_pos = board_find_pawn_tile(pawn_move_tween.pawn_gid);
        board_move_pawn(pawn_move_tween.pawn_gid, pawn_old_pos,
                        BOARD_POS(pawn_move_tween.end_pos.x, pawn_move_tween.end_pos.y));

        request_step = TRUE; // step

        // clear tween info
        memset(&pawn_move_tween, 0, sizeof(PawnMoveTweenInfo));
        pawn_move_tween.pawn_gid = -1;

        return;
    }

    // get the assigned sprite
    int pawn_sprite_ix = *pawn_sprite_ix_out;
    Sprite* pawn_sprite = &sprites[pawn_sprite_ix];

    // calculate the between vpos
    int tween_len = pawn_move_tween.end_frame - pawn_move_tween.start_frame;
    int frame_prog = frame_count - pawn_move_tween.start_frame;

    VPos16 start_pix_pos = board_vpos_to_pix_pos(pawn_move_tween.start_pos.x, pawn_move_tween.start_pos.y);
    VPos16 end_pix_pos = board_vpos_to_pix_pos(pawn_move_tween.end_pos.x, pawn_move_tween.end_pos.y);

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

void update_pawn_flash_tween() {

}

void update_pawn_tweens() {
    update_pawn_move_tween();
    update_pawn_flash_tween();
}