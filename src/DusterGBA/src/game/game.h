#pragma once

#include <tonc_types.h>

#define MAX_BOARD_SIZE 64
#define NUM_TEAMS 4
#define TEAM_MAX_PAWNS 32

typedef struct {
    s16 pawn_index;
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