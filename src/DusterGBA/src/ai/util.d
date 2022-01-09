module ai.util;

import dusk;
import game;
import libtind.ds.dict;
import libtind.ds.vector;
import libtind.util;
import ai;

extern (C):

struct Relocs {
    Dict!(VPos16, pawn_gid_t) map;
}

struct PawnMoveCache {
    Relocs relocs;
    Vector!VPos16 range_data;

    void free() {
        relocs.map.free();
        range_data.free();
    }
}

enum ProximityType {
    NEAR,
    FAR
}

/** make a list of all available pawns on this team */
Vector!pawn_gid_t ai_scan_pawns(int team_id) {
    Vector!pawn_gid_t my_pawns;

    for (int pawn_ix = 0; pawn_ix < TEAM_MAX_PAWNS; pawn_ix++) {
        pawn_gid_t pawn_gid = PAWN_GID(team_id, pawn_ix);
        Pawn* pawn = game_get_pawn_by_gid(pawn_gid);

        if (pawn.alive) {
            my_pawns.push_back(pawn_gid);
        }
    }

    return my_pawns;
}

pawn_gid_t ai_get_closest_enemy_pawn(VPos16 pos, Vector!pawn_gid_t enemy_pawns) {
    auto closest_enemy_pawn = enemy_pawns[0];
    int closest_enemy_pawn_dist = 999_999;

    for (int i = 0; i < enemy_pawns.length; i++) {
        auto pawn = enemy_pawns[i];
        auto pawn_tile = board_find_pawn_tile(pawn);
        auto pawn_pos = board_util_tid_to_pos(pawn_tile);
        auto pawn_dist = board_dist_pos(pos, pawn_pos);

        if (pawn_dist < closest_enemy_pawn_dist) {
            closest_enemy_pawn = pawn;
            closest_enemy_pawn_dist = pawn_dist;
        }
    }

    return closest_enemy_pawn;
}

bool ai_can_pawn_move_to(pawn_gid_t pawn_gid, VPos16 from_pos, VPos16 pos, PawnMoveCache* move_cache) {
    // first, check that tile is empty and walkable
    bool tile_check = false;

    if (board_util_is_on_board_pos(pos) > 0 && board_util_is_walkable_pos(pos) > 0) {
        // valid tile, check if occupied
        auto is_occupied_before = board_get_pawn(BOARD_POSV(pos)) is null;
        auto is_reloc = pos in move_cache.relocs.map;

        // double check using relocs to see if pawn is already planned to move there
        if (is_reloc) {
            auto reloc_pawn_gid = move_cache.relocs.map[pos];
            if (!is_occupied_before && reloc_pawn_gid >= 0) {
                // someone else is already planning to move there
                tile_check = false;
            }
            if (is_occupied_before && reloc_pawn_gid < 0) {
                // actually it is no longer occupied
                tile_check = true;
            }
        } else if (!is_occupied_before) {
            // no one is planning to move there. we can move there
            tile_check = true;
        }
    }

    // check range
    bool range_check = false;

    for (int i = 0; i < move_cache.range_data.length; i++) {
        auto range_pos = move_cache.range_data[i];
        if (range_pos == pos) {
            range_check = true;
            break;
        }
    }

    return tile_check && range_check;
}

VPos16 ai_get_closest_tile_in_proximity(pawn_gid_t pawn_gid, VPos16 from_pos, VPos16 to_pos, ProximityType proximity_type, PawnMoveCache* move_cache) {
    // find closest tile in range to our destination

    // we can do this with less duplicated code
    auto target_tile = from_pos;
    auto target_dist = -1;
    for (int i = 0; i < move_cache.range_data.length; i++) {
        auto range_pos = move_cache.range_data[i];
        auto range_dist = board_dist_pos(range_pos, to_pos);

        // ensure we can move to this tile
        if (!ai_can_pawn_move_to(pawn_gid, from_pos, range_pos, move_cache)) {
            continue;
        }

        switch (proximity_type) {
        case ProximityType.NEAR:
            if (range_dist < target_dist || target_dist < 0) {
                target_tile = range_pos;
                target_dist = range_dist;
            }
            break;
        case ProximityType.FAR:
            if (range_dist > target_dist || target_dist < 0) {
                target_tile = range_pos;
                target_dist = range_dist;
            }
            break;
        default:
            ds_assert(0, "unhandled proximity type");
        }
    }

    return target_tile;
}
