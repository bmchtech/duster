#pragma once

#include <tonc_types.h>

#define MAX_BOARD_SIZE 64
#define NUM_TEAMS 4
#define TEAM_MAX_PAWNS 32

#define NUM_UNIT_CLASSES 4

typedef struct {
    s16 pawn_gid;
} BoardTile;

typedef struct {
    BoardTile tiles[MAX_BOARD_SIZE * MAX_BOARD_SIZE]; // 8K section for tile state
} GameBoard;

typedef struct {
    u8 unit_class;
    u8 unit_data;
} Pawn;

typedef struct {
    char name[16];
    Pawn pawns[TEAM_MAX_PAWNS];
} Team;

typedef struct {
    GameBoard board;
    u8 board_size;
    Team teams[NUM_TEAMS];
} GameState;

typedef struct {
    char name[16];
    u8 move;
} ClassData;

typedef struct {
    ClassData* class_data;
} GameColdData;

extern GameState game_state;
extern GameColdData game_data;

#define BOARD_POS(x, y) (y * MAX_BOARD_SIZE + x)
#define PAWN_GID(team, pawn) (team * TEAM_MAX_PAWNS + pawn)

void game_clear_state();
void game_load_cold_data();
void game_init_board(u8 board_size);
void game_init_team(u8 id, const char* name);
/** get the pawn with by global id */
Pawn* game_get_pawn_by_gid(s16 pawn_gid);

Pawn* board_get_pawn(int tile_id);
void board_set_pawn(int tile_id, s16 pawn_gid);

void team_set_pawn(Team* team, int id, int class);