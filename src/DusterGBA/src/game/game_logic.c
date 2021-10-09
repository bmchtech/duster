#include "game.h"
#include <stdio.h>
#include <memory.h>
#include <tonc.h>
#include "contrib/mgba.h"

void game_logic_step() {
    game_state.steps++; // counter

    mgba_printf(MGBA_LOG_ERROR, "logic step (%d)", game_state.steps);
}

void game_logic_interact(pawn_gid_t initiator, pawn_gid_t receiver) {
    int initiator_tid = board_find_pawn_tile(initiator);
    int receiver_tid = board_find_pawn_tile(receiver);

    // kill the dest pawn
    board_set_pawn(receiver_tid, -1);
    mgba_printf(MGBA_LOG_ERROR, "killed dest pawn: %d (at %d)", receiver, receiver_tid);
}