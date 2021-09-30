#include "game.h"

EWRAM_DATA GameState game_state;

Pawn* game_get_pawn(int pawn_id) {
    int team_ix = pawn_id / TEAM_MAX_PAWNS;
    int team_pawn_ix = pawn_id % TEAM_MAX_PAWNS;

    // get pawn from team
    Team* team = &game_state.teams[team_ix];
    Pawn* pawn = &team->pawns[team_pawn_ix];

    return pawn;
}