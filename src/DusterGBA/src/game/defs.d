module game.defs;

import tonc;
import game;
import ldc.attributes;

// #define MAX_BOARD_SIZE 64
// #define NUM_TEAMS 4
// #define TEAM_MAX_PAWNS 32

enum MAX_BOARD_SIZE = 64;
enum NUM_TEAMS = 4;
enum TEAM_MAX_PAWNS = 32;

// #define NUM_UNIT_CLASSES 4

enum NUM_UNIT_CLASSES = 4;

// #define MOVEQUEUE_MAX_SIZE (TEAM_MAX_PAWNS + 1)

enum MOVEQUEUE_MAX_SIZE = (TEAM_MAX_PAWNS + 1);

// typedef s16 pawn_gid_t;

alias pawn_gid_t = s16;

// typedef enum {
//     TERRAIN_GROUND,
//     TERRAIN_BLOCKED,
// } Terrain;

enum Terrain {
    TERRAIN_GROUND,
    TERRAIN_BLOCKED,
}

// typedef struct {
//     pawn_gid_t pawn_gid;
//     Terrain terrain;
// } BoardTile;

struct BoardTile {
    pawn_gid_t pawn_gid;
    Terrain terrain;
}

// typedef struct {
//     BoardTile tiles[MAX_BOARD_SIZE * MAX_BOARD_SIZE]; // 8K section for tile state
// } GameBoard;

struct GameBoard {
    BoardTile[MAX_BOARD_SIZE * MAX_BOARD_SIZE] tiles; // 8K section for tile state
}

// typedef struct {
//     u8 atk, def, hp, spd;
// } UnitDataStats;

struct UnitDataStats {
    u8 atk, def, hp, spd;
}

// typedef struct {
//     s16 hitpoints;
//     s16 item;
//     u16 level;
//     UnitDataStats stats;
// } UnitData;

struct UnitData {
    s16 hitpoints;
    s16 item;
    u16 level;
    UnitDataStats stats;
}

// typedef struct {
//     u8 unit_class;
//     UnitData unit_data;
//     int last_moved_step;
//     int last_moved_turn;
//     bool alive;
// } Pawn;

struct Pawn {
    u8 unit_class;
    UnitData unit_data;
    int last_moved_step;
    int last_moved_turn;
    bool alive;
}

// typedef struct {
//     char name[16];
//     Pawn pawns[TEAM_MAX_PAWNS];
//     bool alive;
// } Team;

struct Team {
    char[16] name;
    Pawn[TEAM_MAX_PAWNS] pawns;
    bool alive;
}

// typedef struct {
//     GameBoard board;
//     u8 board_size;
//     Team teams[NUM_TEAMS];
//     int steps;
//     int turns;
// } GameState;

struct GameState {
    GameBoard board;
    u8 board_size;
    Team[NUM_TEAMS] teams;
    int steps;
    int turns;
}

// typedef struct {
//     char name[16];
//     u8 move;
//     u8 interact_range;
//     UnitDataStats base_stats;
//     UnitDataStats stat_growths;
// } ClassData;

struct ClassData {
    char[16] name;
    u8 move;
    u8 interact_range;
    UnitDataStats base_stats;
    UnitDataStats stat_growths;
}

// typedef struct {
//     ClassData* class_data;
// } GameColdData;

struct GameColdData {
    ClassData* class_data;
}

// typedef struct {
//     int neighbors[4];
// } tile_neighbors_t;

struct tile_neighbors_t {
    int[4] neighbors;
}

// typedef struct {
//     bool valid;
//     VPos16 pos;
//     u32 team;
//     u32 pawn;
// } PawnSpawnPoint;

struct PawnSpawnPoint {
    bool valid;
    VPos16 pos;
    u32 team;
    u32 pawn;
}

// typedef struct {
//     int main_dmg;
//     int counter_dmg;
// } HostileUnitDuel;

struct HostileUnitDuel {
    int main_dmg;
    int counter_dmg;
}

// typedef enum {
//     QUEUEDMOVE_MOVE,
//     QUEUEDMOVE_INTERACT,
// } QueuedMoveType;

enum QueuedMoveType {
    QUEUEDMOVE_MOVE,
    QUEUEDMOVE_INTERACT,
}

// typedef struct {
//     QueuedMoveType type;
//     pawn_gid_t pawn0;
//     pawn_gid_t pawn1;
//     VPos16 start_pos;
//     VPos16 end_pos;
// } QueuedMove;

struct QueuedMove {
    QueuedMoveType type;
    pawn_gid_t pawn0;
    pawn_gid_t pawn1;
    VPos16 start_pos;
    VPos16 end_pos;
}

// GameState game_state;
// GameColdData game_data;
@(ldc.attributes.section(".ewram")) __gshared u8[4096] game_ai_blackboard;

// macros

// #define BOARD_POS(x, y) ((y)*MAX_BOARD_SIZE + (x))
// #define PAWN_GID(team, pawn) ((team)*TEAM_MAX_PAWNS + (pawn))
// #define PAWN_WHICH_TEAM(gid) ((gid) / TEAM_MAX_PAWNS)
// #define PAWN_NUM_IN_TEAM(gid) ((gid) % TEAM_MAX_PAWNS)
// #define POS_TO_TID(pos) (BOARD_POS((pos).x, (pos).y))

auto BOARD_POS(int x, int y) {
    return (y) * MAX_BOARD_SIZE + (x);
}

auto PAWN_GID(int team, int pawn) {
    return (team) * TEAM_MAX_PAWNS + (pawn);
}

auto PAWN_WHICH_TEAM(int gid) {
    return (gid) / TEAM_MAX_PAWNS;
}

auto PAWN_NUM_IN_TEAM(int gid) {
    return (gid) % TEAM_MAX_PAWNS;
}

auto POS_TO_TID(VPos16 pos) {
    return BOARD_POS((pos).x, (pos).y);
}
