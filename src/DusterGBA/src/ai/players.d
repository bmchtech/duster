module ai.players;

import game;
import tonc;
import dusk;
import dusk.contrib.mgba;
import ai;

extern (C):

int ai_plan_moves_variant_1(int team_id, QueuedMove* moves, int moves_buf_len) {

    // pick blackboard base slot based on team id
    auto bb_base = team_id * TEAM_BLACKBOARD_SIZE;
    auto my_bb = game_ai_blackboard[bb_base .. bb_base + TEAM_BLACKBOARD_SIZE - 1];

    int planned_moves = 0;

    // list all pawns on this team
    auto my_pawns = ai_scan_pawns(team_id);
    // list all pawns on enemy team (for now assume only 2 teams)
    auto enemy_pawns = ai_scan_pawns((team_id + 1) % 2);

    // make plan, assigning random targets to each pawn
    if (my_bb[0] == 0) {
        // set first blackboard slot as plan set flag
        my_bb[0] = 1;

        // assign random targets to each pawn
        for (int i = 0; i < my_pawns.length; i++) {
            int rand = game_util_randint() >> 16;
            pawn_gid_t target_enemy_pawn_id = cast(pawn_gid_t)(rand % enemy_pawns.length);
            // NOTE: THIS IS CURSED WE SHOULD NOT CAST GID (16-bit) TO 8-bit BLACKBOARD VALUE
            // my_bb[i + 1] = target_enemy_pawn_id;
            my_bb[i + 1] = cast(u8) target_enemy_pawn_id;
        }
    }

    // vars for planning movement
    auto move_cache = PawnMoveCache();

    // advance our plan
    for (int i = 0; i < my_pawns.length; i++) {
        auto pawn_gid = my_pawns[i];
        auto pawn = game_get_pawn_by_gid(pawn_gid);
        auto curr_tile = board_find_pawn_tile(pawn_gid);
        auto curr_tile_pos = board_util_tid_to_pos(curr_tile);

        auto dest_pos = curr_tile_pos;

        // ensure there are enemies available
        if (enemy_pawns.length <= 0) {
            break;
        }

        // get assigned target
        auto target_enemy_pawn_id = my_bb[i + 1];
        auto target_enemy_pawn = enemy_pawns[target_enemy_pawn_id];

        // reset pawn-specific cache
        move_cache.range_data.clear();

        // calc range data
        auto class_data = pawn_get_classdata(pawn_gid);

        board_util_calc_pawn_range(curr_tile_pos.x, curr_tile_pos.y,
            class_data.move, &move_cache.range_data);

        // plan a move towards target
        auto enemy_target_tile = board_find_pawn_tile(target_enemy_pawn);
        auto enemy_target_tile_pos = board_util_tid_to_pos(enemy_target_tile);
        auto pos_closer_to_enemy = ai_get_closest_tile_in_proximity(pawn_gid, curr_tile_pos,
            enemy_target_tile_pos, ProximityType.NEAR, &move_cache);

        dest_pos = pos_closer_to_enemy;

        // check if we can move there
        if (!ai_can_pawn_move_to(pawn_gid, curr_tile_pos, dest_pos, &move_cache)) {
            // can't move there
            continue;
        }

        // add a move for this pawn
        QueuedMove move = {
            type: QueuedMoveType.QUEUEDMOVE_MOVE,
            pawn0: pawn_gid, start_pos: curr_tile_pos, end_pos: dest_pos
        };
        moves[planned_moves] = move;

        // log that we are moving to this tile
        mgba_printf(MGBALogLevel.INFO, "pawn %d is moving to %d,%d", pawn_gid,
            dest_pos.x, dest_pos.y);

        // update relocs
        move_cache.relocs.map[curr_tile_pos] = -1;
        move_cache.relocs.map[dest_pos] = pawn_gid;

        // increment planned moves counter
        planned_moves++;
    }

    // clean up
    my_pawns.free();
    enemy_pawns.free();
    move_cache.free();

    // return number of planned moves
    return planned_moves;
}

int ai_plan_moves_variant_2(int team_id, QueuedMove* moves, int moves_buf_len) {
    return ai_plan_moves_variant_1(team_id, moves, moves_buf_len);
}
