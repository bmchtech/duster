#pragma once

#include <tonc_types.h>
#include "vpos.h"
#include <stdint.h>

#define MAX_BOARD_SIZE 64
#define NUM_TEAMS 4
#define TEAM_MAX_PAWNS 32

#define NUM_UNIT_CLASSES 4

#define MOVEQUEUE_MAX_SIZE (TEAM_MAX_PAWNS + 1)

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
    u8 atk, def, hp, spd;
} UnitDataStats;

typedef struct {
    s16 hitpoints;
    s16 item;
    u16 level;
    UnitDataStats stats;
} UnitData;

typedef struct {
    u8 unit_class;
    UnitData unit_data;
    int last_moved_step;
    int last_moved_turn;
    BOOL alive;
} Pawn;

typedef struct {
    char name[16];
    Pawn pawns[TEAM_MAX_PAWNS];
    BOOL alive;
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
    UnitDataStats stat_growths;
} ClassData;

typedef struct {
    ClassData* class_data;
} GameColdData;

typedef struct {
    int neighbors[4];
} tile_neighbors_t;

typedef struct {
    BOOL valid;
    VPos16 pos;
    u32 team;
    u32 pawn;
} PawnSpawnPoint;

typedef struct {
    int main_dmg;
    int counter_dmg;
} HostileUnitDuel;

typedef enum {
    QUEUEDMOVE_MOVE,
    QUEUEDMOVE_INTERACT,
} QueuedMoveType;

typedef struct {
    QueuedMoveType type;
    pawn_gid_t pawn0;
    pawn_gid_t pawn1;
    VPos16 start_pos;
    VPos16 end_pos;
} QueuedMove;

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
void team_pawn_recalculate(int team_id, int pawn_id);

ClassData* pawn_get_classdata(pawn_gid_t pawn_gid);

// LOGIC

void game_logic_step();
void game_logic_interact(pawn_gid_t initiator, pawn_gid_t receiver);
HostileUnitDuel game_logic_calc_hostile_damage(Pawn* initiator_pawn, Pawn* receiver_pawn);

// UTIL

BOOL board_util_is_on_board(int tx, int ty);
BOOL board_util_is_walkable(int tx, int ty);
VPos16 board_util_tid_to_pos(int tile_id);
tile_neighbors_t board_util_get_neighbors(int tile_id);
int board_util_calc_rangebuf(int start_tx, int start_ty, int range, VPos16* pos_buf, int pos_buf_len);
BOOL pawn_util_is_valid_move(pawn_gid_t pawn_gid, VPos16 start_pos, VPos16 end_pos);
BOOL pawn_util_on_same_team(pawn_gid_t pawn1, pawn_gid_t pawn2);
BOOL game_util_is_my_turn(pawn_gid_t pawn_gid);
int game_util_whose_turn();
BOOL pawn_util_moved_this_turn(Pawn* pawn);
UnitDataStats pawn_util_calc_stats(ClassData* class_data, int level);

// MAP
BOOL game_load_gamemap(void* data, u32 len);

// lua code binding
int32_t game_gs_ai_plan_moves(int32_t team_id, QueuedMove* move_buf, int32_t move_buf_len);