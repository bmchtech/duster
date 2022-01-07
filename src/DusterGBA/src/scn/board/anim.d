module scn.board.anim;

import core.stdc.string;
import core.stdc.stdio;
import dusk;
import tonc;
import libgba.maxmod;
import res;
import dusk.contrib.mgba;
import typ.vpos;
import scn.board;
import game;

void animate_pawn_move(pawn_gid_t pawn_gid, VPos16 start_pos, VPos16 end_pos) {
    pawn_move_tween.start_pos = start_pos;
    pawn_move_tween.end_pos = end_pos;
    pawn_move_tween.pawn_gid = pawn_gid;
    pawn_move_tween.start_frame = frame_count;
    pawn_move_tween.end_frame = frame_count + 6;
}

void animate_pawn_flash(pawn_gid_t pawn_gid, pawn_gid_t initiator_gid, bool flash_color) {
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

    // TODO: fix this
    // if (cc_hashtable_get(pawn2sprite, &pawn_gid, cast(void*)&pawn_sprite_ix_out) != CC_OK) {
    //     mgba_printf(ERROR, "failed to get sprite index for pawn gid: %d", pawn_gid);
    //     return -1;
    // }

    return *pawn_sprite_ix_out;
}

void update_pawn_move_tween() {
    PawnMoveTweenInfo* tween = &pawn_move_tween;

    // make sure there is a running tween
    if (tween.pawn_gid < 0)
        return;

    // check if we are at end of anim
    if (frame_count >= tween.end_frame) {
        // done

        // propagate real actions

        // set real pos to end
        int pawn_old_pos = board_find_pawn_tile(tween.pawn_gid);
        board_move_pawn(tween.pawn_gid, pawn_old_pos, BOARD_POS(tween.end_pos.x, tween.end_pos.y));

        request_step = TRUE; // step

        // clear tween info
        memset(tween, 0, (PawnFlashTweenInfo.sizeof));
        tween.pawn_gid = -1;

        set_ui_dirty(); // ui dirty

        return;
    }

    // continue the anim...

    // get the assigned sprite
    int pawn_sprite_ix = get_sprite_index_for_pawn(tween.pawn_gid);
    if (pawn_sprite_ix < 0)
        return; // FAIL
    Sprite* pawn_sprite = &sprites[pawn_sprite_ix];

    // get anim progress
    int tween_len = tween.end_frame - tween.start_frame; // total length of tween in frames
    int frame_prog = frame_count - tween.start_frame; // how many frames have elapsed since the start frame

    // calculate the between vpos
    VPos16 start_pix_pos = board_vpos_to_pix_pos(tween.start_pos.x, tween.start_pos.y);
    VPos16 end_pix_pos = board_vpos_to_pix_pos(tween.end_pos.x, tween.end_pos.y);

    int dx = end_pix_pos.x - start_pix_pos.x;
    int dy = end_pix_pos.y - start_pix_pos.y;

    int x_step = dx / tween_len; // pix per frame
    int y_step = dy / tween_len; // pix per frame

    // mgba_printf(ERROR, "sr: %d, st: %d", x_step_rate, curr_x_step);
    int x_prog = start_pix_pos.x + (frame_prog * x_step);
    int y_prog = start_pix_pos.y + (frame_prog * y_step);

    pawn_sprite.x = cast(u16) x_prog;
    pawn_sprite.y = cast(u16) y_prog;
}

void update_pawn_flash_tween() {
    PawnFlashTweenInfo* tween = &pawn_flash_tween;

    // make sure there is a running tween
    if (tween.pawn_gid < 0)
        return;

    // check if we are at end of anim
    if (frame_count >= tween.end_frame) {
        // done

        // clean up effects

        // disable window
        *REG_DISPCNT &= ~DCNT_WIN0;

        // disable blend
        *REG_BLDY = BLDY_BUILD(0);

        // propagate real actions
        game_logic_interact(cast(pawn_gid_t) tween.initiator_gid, tween.pawn_gid);

        request_step = TRUE;

        // clear tween info
        memset(tween, 0, (PawnFlashTweenInfo.sizeof));
        tween.pawn_gid = -1;

        set_ui_dirty(); // ui dirty

        return;
    }

    // get the assigned sprite
    int pawn_sprite_ix = get_sprite_index_for_pawn(tween.pawn_gid);
    Sprite* pawn_sprite = &sprites[pawn_sprite_ix];
    if (pawn_sprite_ix < 0)
        return; // FAIL

    // check if we are at start of tween
    if (frame_count == tween.start_frame) {
        // enable window
        *REG_DISPCNT |= DCNT_WIN0;

        // set up win0
        *REG_WIN0H = cast(u16) WIN_BUILD(pawn_sprite.x + 8, pawn_sprite.x);
        *REG_WIN0V = cast(u16) WIN_BUILD(pawn_sprite.y + 8, pawn_sprite.y);
        *REG_WININ = cast(u16) WININ_BUILD(WIN_OBJ | WIN_BLD, 0);
        *REG_WINOUT = cast(u16) WINOUT_BUILD(WIN_ALL, 0);

        // set up blending
        *REG_BLDCNT = BLD_OBJ;
        if (tween.flash_color)
            *REG_BLDCNT |= BLD_WHITE;
        else
            *REG_BLDCNT |= BLD_BLACK;
    }

    // get anim progress
    int tween_len = tween.end_frame - tween.start_frame; // total length of tween in frames
    int frame_prog = frame_count - tween.start_frame; // how many frames have elapsed since the start frame

    // do sprite flash

    if (tween_len > 16) {
        int fade_step1 = tween_len / 16; // frames per blend step
        *REG_BLDY = cast(u16)(frame_prog / fade_step1);
    } else {
        int fade_step2 = 16 / tween_len; // blend steps per frame
        *REG_BLDY = cast(u16)(frame_prog * fade_step2);
    }
}

void update_pawn_tweens() {
    update_pawn_move_tween();
    update_pawn_flash_tween();
}

void run_queued_move(QueuedMove* move) {
    switch (move.type) {
    case QueuedMoveType.QUEUEDMOVE_MOVE:
        animate_pawn_move(move.pawn0, move.start_pos, move.end_pos);
        break;
    case QueuedMoveType.QUEUEDMOVE_INTERACT:
        animate_pawn_move(move.pawn0, move.start_pos, move.end_pos);
        animate_pawn_flash(move.pawn1, move.pawn0,
            pawn_util_on_same_team(move.pawn1, move.pawn0));
        break;
    default:
        break;
    }
}

int step_running_queued_moves(QueuedMove* moves, int length, int progress) {
    int curr_step = progress;

    // start initial move
    if (curr_step == -1) {
        run_queued_move(&moves[0]);
        curr_step = 0;
    }

    QueuedMove* curr_move = &moves[curr_step];

    // check if last one is done
    bool move_done = FALSE;
    if (curr_move.type == QueuedMoveType.QUEUEDMOVE_MOVE) {
        if (pawn_move_tween.pawn_gid == -1)
            move_done = TRUE;
    }
    if (curr_move.type == QueuedMoveType.QUEUEDMOVE_INTERACT) {
        if (pawn_flash_tween.pawn_gid == -1 && pawn_move_tween.pawn_gid == -1)
            move_done = TRUE;
    }

    if (move_done) {
        curr_step++;
        // check if we are at end
        if (curr_step >= length) {
            return length;
        }

        // start the next
        QueuedMove* move = &moves[curr_step];
        run_queued_move(move);
    }

    // we are not at end

    return curr_step;
}

void update_queued_moves() {
    if (movequeue_length <= 0 || movequeue_progress >= movequeue_length) {
        return; // all done for now
    }

    if (frame_count > movequeue_delay_timer) {
        movequeue_delay_timer = frame_count + 20; // schedule next run

        int new_progress = step_running_queued_moves(cast(QueuedMove*) movequeue_queue, movequeue_length, movequeue_progress);
        movequeue_progress = new_progress;
    }
}
