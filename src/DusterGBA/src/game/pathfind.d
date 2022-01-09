module game.pathfind;

import core.stdc.stdio;
import core.stdc.string;
import tonc;
import dusk.contrib.mgba;
import game;

import libtind.ds.vector;
import libtind.ds.dict;
import libtind.ds.heap;
import libtind.ds.set;

extern (C):

alias NodeDistanceDict = Dict!(int, int);

NodeDistanceDict board_util_calc_node_distances(int start_tx, int start_ty, int range) {
    int start_tid = BOARD_POS(start_tx, start_ty);

    // create data structures
    NodeDistanceDict node_distances;
    Heap!int unvisited;

    scope (exit) { // free data structures
        // node_distances.free(); // we will return the dict
        unvisited.free();
    }

    // set initial dijstra values
    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; j++) {
            int tid = BOARD_POS(start_tx + i, start_ty + j);
            if (board_util_tid_is_on_board(tid)) {
                // set dist value for this node
                if (tid == start_tid) {
                    node_distances.set(tid, 0);
                } else {
                    node_distances.set(tid, 999_999);
                }

                // add to unvisited
                unvisited.add(Heap!int.Node(node_distances.get(tid), tid));
            }
        }
    }

    // run dijkstra's algorithm to get shortest dist to each nearby node
    while (unvisited.count > 0) {
        auto unvisited_heapnode = unvisited.remove_min();
        int tid = unvisited_heapnode.value;
        int curr_dist = node_distances[tid];

        // mgba_printf(// mgbaLogLevel.ERROR, "DIJ1: %d %d\n", tid, curr_dist);

        // get neighbors
        tile_neighbors_t tn = board_util_get_neighbors(tid);

        // for each neighbor
        for (int i = 0; i < NUM_TILE_NEIGHBORS; i++) {
            int nid = tn.neighbors[i];
            if (nid < 0)
                continue;

            // mgba_printf(// mgbaLogLevel.ERROR, "DIJ11: %d %d\n", tn.neighbors[i], curr_dist + 1);
            auto npos = board_util_tid_to_pos(nid);
            // mgba_printf(// mgbaLogLevel.ERROR, "DIJ16: %d %d\n", npos.x, npos.y);
            // mgba_printf(// mgbaLogLevel.ERROR, "DIJ15: %d\n", board_util_tid_is_on_board(nid) ? 1 : 0);
            // sanity check the tile
            if (!board_util_tid_is_on_board(nid))
                continue;
            // mgba_printf(// mgbaLogLevel.ERROR, "DIJ12: %d %d\n", tn.neighbors[i], curr_dist + 1);

            // ensure walkable
            if (!board_util_tid_is_walkable(nid))
                continue;
            // mgba_printf(// mgbaLogLevel.ERROR, "DIJ13: %d %d\n", tn.neighbors[i], curr_dist + 1);

            // mgba_printf(// mgbaLogLevel.ERROR, "DIJ2: %d %d\n", tid, curr_dist);

            int cost = board_util_get_neighbor_tile_cost(tid, nid);

            // check if any pawns on this tile
            BoardTile* scan_tile = board_get_tile(nid);
            if (scan_tile.pawn_gid > 0) {
                // a pawn is here
                // TODO: make this more clear, use a function
                cost += 1;
            }

            // calculate new distance
            int ndist = curr_dist + cost;
            // mgba_printf(// mgbaLogLevel.ERROR, "DIJ3: %d %d\n", nid, ndist);

            // if this is a better path
            if (ndist < node_distances[nid]) {
                // update distance
                node_distances[nid] = ndist;

                // add to unvisited
                unvisited.add(Heap!int.Node(node_distances[nid], nid));
            }
        }
    }

    return node_distances;
}

int board_util_calc_pawn_range(int start_tx, int start_ty, int range, Vector!VPos16* range_vec) {
    auto node_distances = board_util_calc_node_distances(start_tx, start_ty, range);
    int start_tid = BOARD_POS(start_tx, start_ty);

    scope (exit) {
        node_distances.free();
    }

    // finally, populate the rangebuf with the positions we can reach
    // mgba_printf(// mgbaLogLevel.ERROR, "checking dists %d\n", node_distances.length);
    range_vec.clear(); // clear range buffer
    foreach (node_key; node_distances.byKey()) {
        // get info about this tile
        int tid = node_key;
        int dist = node_distances[node_key];

        // mgba_printf(// mgbaLogLevel.ERROR, "CHECK: tid: %d, dist: %d\n", tid, dist);

        // ensure not starting tile
        if (tid == start_tid)
            continue;

        // ensure dist within range
        if (dist > range)
            continue;

        // ensure on board
        if (!board_util_tid_is_on_board(tid))
            continue;

        // make sure no other pawn is there
        // if (board_get_pawn(tid))
        //     continue;

        // log this tile info
        // mgba_printf(// mgbaLogLevel.ERROR, "ACCEPT: tid: %d, dist: %d\n", tid, dist);

        // add to range vec
        range_vec.push_back(board_util_tid_to_pos(tid));
    }

    return range_vec.length;
}
