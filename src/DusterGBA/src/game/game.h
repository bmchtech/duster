#pragma once

#include <tonc_types.h>
#include "vpos.h"

#define MAX_BOARD_SIZE 64
#define NUM_TEAMS 4
#define TEAM_MAX_PAWNS 32

#define NUM_UNIT_CLASSES 4

typedef s16 pawn_gid_t;

typedef enum {
    TERRAIN_GROUND,
    TERRAIN_BLOCKED,
} Terrain;

typedef struct {
    pawn_gid_t pawn_gid;
    Terrain terrain;
} BoardTile;

typedef struct {
    BoardTile tiles[MAX_BOARD_SIZE * MAX_BOARD_SIZE]; // 8K section for tile state
} GameBoard;

typedef struct {
    u8 atk, def, hp;
} UnitDataStats;

typedef struct {
    s16 hitpoints;
    s16 item;
    UnitDataStats stats;
} UnitData;

typedef struct {
    u8 unit_class;
    UnitData unit_data;
    int last_moved_step;
} Pawn;

typedef struct {
    char name[16];
    Pawn pawns[TEAM_MAX_PAWNS];
} Team;

typedef struct {
    GameBoard board;
    u8 board_size;
    Team teams[NUM_TEAMS];
    int steps;
    int turns;
} GameState;

typedef struct {
    char name[16];
    u8 move;
    u8 interact_range;
    UnitDataStats base_stats;
} ClassData;

typedef struct {
    ClassData* class_data;
} GameColdData;

typedef struct {
    int neighbors[4];
} tile_neighbors_t;

typedef struct {
    u8 board_size;
} GameMap;

extern GameState game_state;
extern GameColdData game_data;

#define BOARD_POS(x, y) ((y)*MAX_BOARD_SIZE + (x))
#define PAWN_GID(team, pawn) ((team)*TEAM_MAX_PAWNS + (pawn))
#define PAWN_WHICH_TEAM(gid) ((gid) / TEAM_MAX_PAWNS)
#define PAWN_NUM_IN_TEAM(gid) ((gid) % TEAM_MAX_PAWNS)
#define POS_TO_TID(pos) (BOARD_POS((pos).x, (pos).y))

void game_clear_state();
void game_load_cold_data();
void game_init();
void game_init_board(u8 board_size);
void game_init_team(u8 team_id, const char* name);
Team* game_get_team(u8 team_id);
/** get the pawn with by global id */
Pawn* game_get_pawn_by_gid(pawn_gid_t pawn_gid);

int board_find_pawn_tile(pawn_gid_t pawn_gid);
BoardTile* board_get_tile(int tile_id);
Pawn* board_get_pawn(int tile_id);
void board_set_pawn(int tile_id, pawn_gid_t pawn_gid);
void board_move_pawn(pawn_gid_t pawn_gid, int start_tile_id, int end_tile_id);
void board_set_terrain(int tile_id, Terrain terrain);
Terrain board_get_terrain(int tile_id);

int board_dist(int tx1, int ty1, int tx2, int ty2);

void team_set_pawn_t(Team* team, int pawn_id, int class);
pawn_gid_t team_set_pawn(int team_id, int pawn_id, int class);

ClassData* pawn_get_classdata(pawn_gid_t pawn_gid);

// LOGIC

void game_logic_step();
void game_logic_interact(pawn_gid_t initiator, pawn_gid_t receiver);

// UTIL

BOOL board_util_is_on_board(int tx, int ty);
BOOL board_util_is_walkable(int tx, int ty);
VPos16 board_util_tile_id_to_pos(int tile_id);
tile_neighbors_t board_util_get_neighbors(int tile_id);
int board_util_calc_rangebuf(int start_tx, int start_ty, int range, VPos16* pos_buf, int pos_buf_len);
BOOL pawn_util_is_valid_move(pawn_gid_t pawn_gid, VPos16 start_pos, VPos16 end_pos);
BOOL pawn_util_on_same_team(pawn_gid_t pawn1, pawn_gid_t pawn2);
BOOL game_util_is_my_turn(pawn_gid_t pawn_gid);

// MAP
GameMap load_game_map(u8* data, u32 len);