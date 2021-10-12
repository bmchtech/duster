#include "game.h"
#include <stdio.h>
#include <memory.h>
#include <tonc.h>
#include "cold_data.h"

EWRAM_DATA GameState game_state;
EWRAM_DATA GameColdData game_data;

void game_clear_state() { memset32(&game_state, 0, sizeof(GameState) / 4); }

void game_load_cold_data() { game_data.class_data = (ClassData*)&cold_class_data; }

void game_init() {
    game_state.steps = 0;
    game_state.turns = 0;
}

void game_init_board(u8 board_size) {
    game_state.board_size = board_size;

    // clear all board tiles
    for (int i = 0; i < MAX_BOARD_SIZE * MAX_BOARD_SIZE; i++) {
        game_state.board.tiles[i] = (BoardTile){.pawn_gid = -1, .terrain = TERRAIN_GROUND};
    }
}

void game_init_team(u8 team_id, const char* name) {
    Team* team = &game_state.teams[team_id];
    sprintf(team->name, "%s", name);

    // set all pawns to empty
    memset32(team->pawns, 0, sizeof(team->pawns) / 4);
}

Team* game_get_team(u8 team_id) {
    Team* team = &game_state.teams[team_id];
    return team;
}

Pawn* game_get_pawn_by_gid(pawn_gid_t pawn_id) {
    int team_ix = pawn_id / TEAM_MAX_PAWNS;      // get which team
    int team_pawn_ix = pawn_id % TEAM_MAX_PAWNS; // get which spot in team

    // get pawn from team
    Team* team = &game_state.teams[team_ix];
    Pawn* pawn = &team->pawns[team_pawn_ix];

    return pawn;
}

int board_find_pawn_tile(pawn_gid_t pawn_gid) {
    for (int by = 0; by < game_state.board_size; by++) {
        for (int bx = 0; bx < game_state.board_size; bx++) {
            int curr_tid = BOARD_POS(bx, by);
            BoardTile* tile = &game_state.board.tiles[curr_tid];
            if (tile->pawn_gid == pawn_gid) {
                return curr_tid;
            }
        }
    }
    return -1; // not found
}

BoardTile* board_get_tile(int tile_id) {
    GameBoard* board = &game_state.board;
    BoardTile* tile = &board->tiles[tile_id];

    return tile;
}

Pawn* board_get_pawn(int tile_id) {
    BoardTile* tile = board_get_tile(tile_id);
    pawn_gid_t pawn_gid = tile->pawn_gid;

    // this means there is no pawn
    if (pawn_gid == -1)
        return NULL;

    return game_get_pawn_by_gid(pawn_gid);
}

void board_set_pawn(int tile_id, pawn_gid_t pawn_gid) {
    BoardTile* tile = board_get_tile(tile_id);

    tile->pawn_gid = pawn_gid;
}

void board_move_pawn(pawn_gid_t pawn_gid, int start_tile_id, int end_tile_id) {
    // clear start tile
    board_set_pawn(start_tile_id, -1);
    // set end tile
    board_set_pawn(end_tile_id, pawn_gid);

    // set last moved
    Pawn* pawn = game_get_pawn_by_gid(pawn_gid);
    pawn->last_moved_step = game_state.steps;
    pawn->last_moved_turn = game_state.turns;
}

void board_set_terrain(int tile_id, Terrain terrain) {
    BoardTile* tile = board_get_tile(tile_id);

    tile->terrain = terrain;
}

Terrain board_get_terrain(int tile_id) { return board_get_tile(tile_id)->terrain; }

void team_set_pawn_t(Team* team, int pawn_id, int class) {
    Pawn pw;

    ClassData* class_data = &game_data.class_data[class];

    // initialize pawn
    // set class
    pw.unit_class = class;
    // set stats to base
    pw.unit_data.stats = class_data->base_stats;
    // set initial hp
    pw.unit_data.hitpoints = pw.unit_data.stats.hp;

    // set initial turn
    pw.last_moved_step = -1;
    pw.last_moved_turn = -1;

    team->pawns[pawn_id] = pw;
}

pawn_gid_t team_set_pawn(int team_id, int pawn_id, int class) {
    Team* team = game_get_team(team_id);
    team_set_pawn_t(team, pawn_id, class);
    return PAWN_GID(team_id, pawn_id);
}

int board_dist(int tx1, int ty1, int tx2, int ty2) { return ABS(tx2 - tx1) + ABS(ty2 - ty1); }

ClassData* pawn_get_classdata(pawn_gid_t pawn_gid) {
    Pawn* pawn = game_get_pawn_by_gid(pawn_gid);

    ClassData* class_data = &game_data.class_data[pawn->unit_class];

    return class_data;
}