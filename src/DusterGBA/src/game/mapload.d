module game.mapload;

import ldc.attributes;

import tonc;
import game;
import dusk.contrib.mgba;

extern (C):

struct PawnSpawn {
    int team;
    int pawn;
    int pclass;
    int level;
    int tx;
    int ty;
}

struct BinDusterMap {
    int board_size;
    int num_tiles;
    int num_spawns;
}

int read_int(u8* data, u32* offset) {
    int int_word = data[*offset] | (data[*offset + 1] << 8) | (
        data[*offset + 2] << 16) | (data[*offset + 3] << 24);
    *offset += 4;
    return int_word;
}

bool game_load_gamemap(void* data, u32 len) {
    u8* binmap = cast(u8*) data;
    BinDusterMap map;

    u32 offset = 0;

    int magic = read_int(binmap, &offset);
    mgba_printf(MGBALogLevel.INFO, "binmap magic number: %08x", magic);

    const int expected_magic = 0xD0570000;
    if (magic != expected_magic) {
        mgba_printf(MGBALogLevel.INFO, "binmap magic number did not match!: found: %08x, expected: %08x", magic,
            expected_magic);
    }

    map.board_size = read_int(binmap, &offset);

    mgba_printf(MGBALogLevel.INFO, "board size: %d", map.board_size);

    // initialize board
    game_init_board(cast(u8) map.board_size);

    // init default teams
    game_init_team(0, "blue");
    game_init_team(1, "red");

    // read in terrain tiles
    map.num_tiles = read_int(binmap, &offset);

    for (int i = 0; i < map.num_tiles; i++) {
        int tile = read_int(binmap, &offset);
        Terrain terrain = cast(Terrain)(tile - 1);
        int tx = i % map.board_size;
        int ty = i / map.board_size;
        board_set_terrain(BOARD_POS(tx, ty), terrain);
    }

    // spawn pawns
    mgba_printf(MGBALogLevel.DEBUG, "offset: %08x", offset);
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
        auto spawn_pos = VPos16(cast(s16)(spawn.tx / 8), cast(s16)(spawn.ty / 8));

        team_set_pawn(cast(u8) spawn.team, spawn.pawn, cast(u8) spawn.pclass);
        // set level
        Pawn* pawn = game_get_pawn_by_gid(PAWN_GID(spawn.team, spawn.pawn));
        pawn.unit_data.level = cast(u16) spawn.level;
        team_pawn_recalculate(spawn.team, spawn.pawn);

        board_set_pawn(BOARD_POS(spawn_pos.x, spawn_pos.y), PAWN_GID(spawn.team, spawn.pawn));

        mgba_printf(MGBALogLevel.INFO, "spawned pawn (team: %d, pawn: %d, c: %d, l: %d) at pos (%d, %d)", spawn.team,
            spawn.pawn, spawn.pclass, spawn.level, spawn_pos.x, spawn_pos.y);
    }

    return true;
}
