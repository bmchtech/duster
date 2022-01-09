#include "game.h"
#include "string.h"
#include "contrib/mgba.h"

typedef struct {
    int team;
    int pawn;
    int pclass;
    int level;
    int tx;
    int ty;
} PawnSpawn;
typedef struct {
    int board_size;
    int num_tiles;
    int num_spawns;
} BinDusterMap;

int read_int(u8* data, u32* offset) {
    int int_word = data[*offset] | (data[*offset + 1] << 8) | (data[*offset + 2] << 16) | (data[*offset + 3] << 24);
    *offset += 4;
    return int_word;
}

BOOL game_load_gamemap(void* data, u32 len) {
    u8* binmap = (u8*)data;
    BinDusterMap map;

    u32 offset = 0;

    int magic = read_int(binmap, &offset);
    mgba_printf(MGBA_LOG_ERROR, "binmap magic number: %08x", magic);

    const int expected_magic = 0xD0570000;
    if (magic != expected_magic) {
        mgba_printf(MGBA_LOG_ERROR, "binmap magic number did not match!: found: %08x, expected: %08x", magic,
                    expected_magic);
    }

    map.board_size = read_int(binmap, &offset);

    mgba_printf(MGBA_LOG_ERROR, "board size: %d", map.board_size);

    // initialize board
    game_init_board(map.board_size);

    // init default teams
    game_init_team(0, "devil");
    game_init_team(1, "salt");

    // read in terrain tiles
    map.num_tiles = read_int(binmap, &offset);

    for (int i = 0; i < map.num_tiles; i++) {
        int tile = read_int(binmap, &offset);
        Terrain terrain = (Terrain)(tile - 1);
        int tx = i % map.board_size;
        int ty = i / map.board_size;
        board_set_terrain(BOARD_POS(tx, ty), terrain);
    }

    // spawn pawns
    mgba_printf(MGBA_LOG_ERROR, "offset: %08x", offset);
    map.num_spawns = read_int(binmap, &offset);
    for (int i = 0; i < map.num_spawns; i++) {
        // read in the spawn data
        PawnSpawn spawn;
        spawn.team = read_int(binmap, &offset);
        spawn.pawn = read_int(binmap, &offset);
        spawn.pclass = read_int(binmap, &offset);
        spawn.level = read_int(binmap, &offset);
        spawn.tx = read_int(binmap, &offset);
        spawn.ty = read_int(binmap, &offset);

        // place the pawn on the board
        VPos16 spawn_pos = (VPos16){.x = spawn.tx / 8, .y = spawn.ty / 8};

        team_set_pawn(spawn.team, spawn.pawn, spawn.pclass);
        // set level
        Pawn* pawn = game_get_pawn_by_gid(PAWN_GID(spawn.team, spawn.pawn));
        pawn->unit_data.level = spawn.level;
        team_pawn_recalculate(spawn.team, spawn.pawn);

        board_set_pawn(BOARD_POS(spawn_pos.x, spawn_pos.y), PAWN_GID(spawn.team, spawn.pawn));

        mgba_printf(MGBA_LOG_ERROR, "spawned pawn (team: %d, pawn: %d, c: %d, l: %d) at pos (%d, %d)", spawn.team,
                    spawn.pawn, spawn.pclass, spawn.level, spawn_pos.x, spawn_pos.y);
    }

    return TRUE;
}