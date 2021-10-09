#include "board_scn.h"

void animate_pawn_move(pawn_gid_t pawn_gid, VPos16 start_pos, VPos16 end_pos) {
    pawn_move_tween.start_pos = start_pos;
    pawn_move_tween.end_pos = end_pos;
    pawn_move_tween.pawn_gid = pawn_gid;
    pawn_move_tween.start_frame = frame_count;
    pawn_move_tween.end_frame = frame_count + 6;
}

void animate_pawn_flash(pawn_gid_t pawn_gid, pawn_gid_t initiator_gid, BOOL flash_color) {
    pawn_flash_tween.pawn_gid = pawn_gid;
    pawn_flash_tween.initiator_gid = initiator_gid;
    pawn_flash_tween.start_frame = frame_count;
    pawn_flash_tween.flash_color = flash_color;
    pawn_flash_tween.end_frame = frame_count + 20;
}

int get_sprite_index_for_pawn(pawn_gid_t pawn_gid) {
    if (pawn_gid < 0)
        return -2;

    int* pawn_sprite_ix_out;
    if (cc_hashtable_get(pawn2sprite, &pawn_gid, (void*)&pawn_sprite_ix_out) != CC_OK) {
        mgba_printf(MGBA_LOG_ERROR, "failed to get sprite index for pawn gid: %d", pawn_gid);
        return -1;
    }

    return *pawn_sprite_ix_out;
}

void update_pawn_move_tween() {
    PawnMoveTweenInfo* tween = &pawn_move_tween;

    // make sure there is a running tween
    if (tween->pawn_gid < 0)
        return;

    // check if we are at end of anim
    if (frame_count >= tween->end_frame) {
        // done

        // propagate real actions

        // set real pos to end
        int pawn_old_pos = board_find_pawn_tile(tween->pawn_gid);
        board_move_pawn(tween->pawn_gid, pawn_old_pos, BOARD_POS(tween->end_pos.x, tween->end_pos.y));

        request_step = TRUE; // step

        // clear tween info
        memset(tween, 0, sizeof(PawnFlashTweenInfo));
        tween->pawn_gid = -1;

        return;
    }

    // continue the anim...

    // get the assigned sprite
    int pawn_sprite_ix = get_sprite_index_for_pawn(tween->pawn_gid);
    if (pawn_sprite_ix < 0)
        return; // FAIL
    Sprite* pawn_sprite = &sprites[pawn_sprite_ix];

    // get anim progress
    int tween_len = tween->end_frame - tween->start_frame; // total length of tween in frames
    int frame_prog = frame_count - tween->start_frame;     // how many frames have elapsed since the start frame

    // calculate the between vpos
    VPos16 start_pix_pos = board_vpos_to_pix_pos(tween->start_pos.x, tween->start_pos.y);
    VPos16 end_pix_pos = board_vpos_to_pix_pos(tween->end_pos.x, tween->end_pos.y);

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
    PawnFlashTweenInfo* tween = &pawn_flash_tween;

    // make sure there is a running tween
    if (tween->pawn_gid < 0)
        return;

    // check if we are at end of anim
    if (frame_count >= tween->end_frame) {
        // done

        // clean up effects

        // disable window
        REG_DISPCNT &= ~DCNT_WIN0;

        // disable blend
        REG_BLDY = BLDY_BUILD(0);

        // propagate real actions
        game_logic_interact(tween->initiator_gid, tween->pawn_gid);

        request_step = TRUE;

        // clear tween info
        memset(tween, 0, sizeof(PawnFlashTweenInfo));
        tween->pawn_gid = -1;

        return;
    }

    // get the assigned sprite
    int pawn_sprite_ix = get_sprite_index_for_pawn(tween->pawn_gid);
    Sprite* pawn_sprite = &sprites[pawn_sprite_ix];
    if (pawn_sprite_ix < 0)
        return; // FAIL

    // check if we are at start of tween
    if (frame_count == tween->start_frame) {
        // enable window
        REG_DISPCNT |= DCNT_WIN0;

        // set up win0
        REG_WIN0H = WIN_BUILD(pawn_sprite->x + 8, pawn_sprite->x);
        REG_WIN0V = WIN_BUILD(pawn_sprite->y + 8, pawn_sprite->y);
        REG_WININ = WININ_BUILD(WIN_OBJ | WIN_BLD, 0);
        REG_WINOUT = WINOUT_BUILD(WIN_ALL, 0);

        // set up blending
        REG_BLDCNT = BLD_OBJ;
        if (tween->flash_color)
            REG_BLDCNT |= BLD_WHITE;
        else
            REG_BLDCNT |= BLD_BLACK;

        mgba_printf(MGBA_LOG_ERROR, "lechuga");
    }

    // get anim progress
    int tween_len = tween->end_frame - tween->start_frame; // total length of tween in frames
    int frame_prog = frame_count - tween->start_frame;     // how many frames have elapsed since the start frame

    // do sprite flash

    if (tween_len > 16) {
        int fade_step1 = tween_len / 16; // frames per blend step
        REG_BLDY = frame_prog / fade_step1;
    } else {
        int fade_step2 = 16 / tween_len; // blend steps per frame
        REG_BLDY = frame_prog * fade_step2;
    }
}

void update_pawn_tweens() {
    update_pawn_move_tween();
    update_pawn_flash_tween();
}
