#include "game.h"
#include <stdio.h>
#include <tonc.h>

EWRAM_DATA GameState game_state;

void game_init_board(u8 board_size) {
    GameBoard* board = &game_state.board;
    // set whole board to -1 (no pawn)
    memset32(board, -1, sizeof(GameBoard) / 4);
    game_state.board_size = board_size;
}

void game_init_team(u8 id, const char* name) {
    Team* team = &game_state.teams[id];
    sprintf(team->name, "%s", name);

    // set all pawns to empty
    memset32(team->pawns, 0, sizeof(team->pawns) / 4);
}

Pawn* game_get_pawn_by_gid(s16 pawn_id) {
    int team_ix = pawn_id / TEAM_MAX_PAWNS;
    int team_pawn_ix = pawn_id % TEAM_MAX_PAWNS;

    // get pawn from team
    Team* team = &game_state.teams[team_ix];
    Pawn* pawn = &team->pawns[team_pawn_ix];

    return pawn;
}

s16 game_calc_gid(int team, int pawn) {
    return team * TEAM_MAX_PAWNS + pawn;
}

void board_set_pawn(int tile, s16 pawn_gid) {
    GameBoard* board = &game_state.board;

    board->tiles[tile].pawn_gid = pawn_gid;
}

void team_set_pawn(Team* team, int id, int class) {
    team->pawns[id] = (Pawn){.unit_class = class};
}