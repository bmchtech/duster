module game.funcs;

import ldc.attributes;

import core.stdc.stdio;
import core.stdc.string;
import tonc;
import dusk.contrib.mgba;
import game;

extern (C):

void game_clear_state() {
    memset32(&game_state, 0, (GameState.sizeof) / 4);
}

void game_load_cold_data() {
    // game_data.class_data = cast (ClassData*) cold_class_data;
}

void game_init() {
    game_state.steps = 0;
    game_state.turns = 0;
}

void game_init_board(u8 board_size) {
    game_state.board_size = board_size;

    // clear all board tiles
    for (int i = 0; i < MAX_BOARD_SIZE * MAX_BOARD_SIZE; i++) {
        game_state.board.tiles[i] = BoardTile(-1, Terrain.TERRAIN_GROUND);
    }
}

void game_init_team(u8 team_id, const char* name) {
    Team* team = &game_state.teams[team_id];
    sprintf(cast(char*) team.name, "%s", name);
    team.alive = TRUE;

    // set all pawns to empty
    memset32(cast(void*) team.pawns, 0, (team.pawns.sizeof) / 4);
}

Team* game_get_team(u8 team_id) {
    Team* team = &game_state.teams[team_id];
    return team;
}

Pawn* game_get_pawn_by_gid(pawn_gid_t pawn_id) {
    int team_ix = pawn_id / TEAM_MAX_PAWNS; // get which team
    int team_pawn_ix = pawn_id % TEAM_MAX_PAWNS; // get which spot in team

    // get pawn from team
    Team* team = &game_state.teams[team_ix];
    Pawn* pawn = &team.pawns[team_pawn_ix];

    return pawn;
}

int board_find_pawn_tile(pawn_gid_t pawn_gid) {
    for (int by = 0; by < game_state.board_size; by++) {
        for (int bx = 0; bx < game_state.board_size; bx++) {
            int curr_tid = BOARD_POS(bx, by);
            BoardTile* tile = &game_state.board.tiles[curr_tid];
            if (tile.pawn_gid == pawn_gid) {
                return curr_tid;
            }
        }
    }
    return -1; // not found
}

BoardTile* board_get_tile(int tile_id) {
    GameBoard* board = &game_state.board;
    BoardTile* tile = &board.tiles[tile_id];

    return tile;
}

Pawn* board_get_pawn(int tile_id) {
    BoardTile* tile = board_get_tile(tile_id);
    pawn_gid_t pawn_gid = tile.pawn_gid;

    // this means there is no pawn
    if (pawn_gid == -1)
        return null;

    return game_get_pawn_by_gid(pawn_gid);
}

void board_set_pawn(int tile_id, pawn_gid_t pawn_gid) {
    BoardTile* tile = board_get_tile(tile_id);

    tile.pawn_gid = pawn_gid;
}

void board_move_pawn(pawn_gid_t pawn_gid, int start_tile_id, int end_tile_id) {
    // clear start tile
    board_set_pawn(start_tile_id, -1);
    // set end tile
    board_set_pawn(end_tile_id, pawn_gid);

    // set last moved
    Pawn* pawn = game_get_pawn_by_gid(pawn_gid);
    pawn.last_moved_step = game_state.steps;
    pawn.last_moved_turn = game_state.turns;
}

void board_set_terrain(int tile_id, Terrain terrain) {
    BoardTile* tile = board_get_tile(tile_id);

    tile.terrain = terrain;
}

Terrain board_get_terrain(int tile_id) {
    return board_get_tile(tile_id).terrain;
}

ClassData* game_get_class_data(u8 class_id) {
    ClassData* class_data = &game_data.class_data[class_id];
    return class_data;
}

void team_set_pawn_t(Team* team, int pawn_id, u8 class_id) {
    Pawn pw;

    ClassData* class_data = game_get_class_data(class_id);

    // initialize pawn
    pw.alive = TRUE;
    // set class
    pw.unit_class = class_id;
    // set stats to stats of level 1 (base stats)
    pw.unit_data.stats = pawn_util_calc_stats(class_data, 1);
    // set initial hp
    pw.unit_data.hitpoints = pw.unit_data.stats.hp;

    // set initial turn
    pw.last_moved_step = -1;
    pw.last_moved_turn = -1;

    team.pawns[pawn_id] = pw;
}

pawn_gid_t team_set_pawn(u8 team_id, int pawn_id, u8 class_id) {
    Team* team = game_get_team(team_id);
    team_set_pawn_t(team, pawn_id, class_id);
    return PAWN_GID(team_id, pawn_id);
}

void team_pawn_recalculate(int team_id, int pawn_id) {
    // recalculate values for this pawn
    Pawn* pawn = game_get_pawn_by_gid(PAWN_GID(team_id, pawn_id));
    ClassData* class_data = game_get_class_data(pawn.unit_class);

    // update stats
    pawn.unit_data.stats = pawn_util_calc_stats(class_data, pawn.unit_data.level);

    // update hp to max
    pawn.unit_data.hitpoints = pawn.unit_data.stats.hp;
}

int board_dist(int tx1, int ty1, int tx2, int ty2) {
    return ABS(tx2 - tx1) + ABS(ty2 - ty1);
}

ClassData* pawn_get_classdata(pawn_gid_t pawn_gid) {
    Pawn* pawn = game_get_pawn_by_gid(pawn_gid);

    // mgba_printf(MGBALogLevel.ERROR, "pawn unit class: %d", pawn.unit_class);

    u8 class_id = pawn.unit_class;
    return game_get_class_data(class_id);
}
