#pragma once

#include <tonc_types.h>

#define MAX_BOARD_SIZE 64
#define NUM_TEAMS 4
#define TEAM_MAX_PAWNS 32

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
    char name[7];
    Pawn pawns[TEAM_MAX_PAWNS];
} Team;

typedef struct {
    GameBoard board;
    u8 board_size;
    Team teams[NUM_TEAMS];
} GameState;

extern GameState game_state;

#define BOARD_POS(x, y) (y * MAX_BOARD_SIZE + x)

void game_clear_state();
void game_init_board(u8 board_size);
void game_init_team(u8 id, const char* name);
/** get the pawn with by global id */
Pawn* game_get_pawn_by_gid(s16 pawn_gid);
s16 game_calc_gid(int team, int pawn);

void board_set_pawn(int tile, s16 pawn_gid);

void team_set_pawn(Team* team, int id, int class);