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

int board_util_calc_rangebuf(int start_tx, int start_ty, int range, VPos16* pos_buf, int pos_buf_len) {
    // clear rangebuf
    memset(pos_buf, 0, (VPos16.sizeof) * pos_buf_len);
    int pos_buf_ix = 0;

    int start_tid = BOARD_POS(start_tx, start_ty);

    // create data structures
    Dict!(int, int) node_distance;
    // Set!int node_visited;
    // Deque!int node_queue;
    Heap!int unvisited;

    scope (exit) { // free data structures
        node_distance.free();
        // node_visited.free();
        // node_queue.free();
        unvisited.free();
    }

    // set initial dijstra values
    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; j++) {
            int tid = BOARD_POS(start_tx + i, start_ty + j);
            if (board_util_tid_is_on_board(tid)) {
                // set dist value for this node
                if (tid == start_tid) {
                    node_distance.set(tid, 0);
                } else {
                    node_distance.set(tid, 999_999);
                }

                // add to unvisited
                unvisited.add(Heap!int.Node(node_distance.get(tid), tid));
            }
        }
    }

    // run dijkstra's algorithm to get shortest dist to each nearby node
    while (unvisited.count > 0) {
        auto unvisited_heapnode = unvisited.remove_min();
        int tid = unvisited_heapnode.value;
        int curr_dist = node_distance[tid];

        // mgba_printf(MGBALogLevel.ERROR, "DIJ1: %d %d\n", tid, curr_dist);

        // get neighbors
        tile_neighbors_t tn = board_util_get_neighbors(tid);

        // for each neighbor
        for (int i = 0; i < NUM_TILE_NEIGHBORS; i++) {
            int nid = tn.neighbors[i];
            if (nid < 0)
                continue;

            // mgba_printf(MGBALogLevel.ERROR, "DIJ11: %d %d\n", tn.neighbors[i], curr_dist + 1);
            auto npos = board_util_tid_to_pos(nid);
            // mgba_printf(MGBALogLevel.ERROR, "DIJ16: %d %d\n", npos.x, npos.y);
            // mgba_printf(MGBALogLevel.ERROR, "DIJ15: %d\n", board_util_tid_is_on_board(nid) ? 1 : 0);
            // sanity check the tile
            if (!board_util_tid_is_on_board(nid))
                continue;
            // mgba_printf(MGBALogLevel.ERROR, "DIJ12: %d %d\n", tn.neighbors[i], curr_dist + 1);

            // ensure walkable
            if (!board_util_tid_is_walkable(nid))
                continue;
            // mgba_printf(MGBALogLevel.ERROR, "DIJ13: %d %d\n", tn.neighbors[i], curr_dist + 1);

            // mgba_printf(MGBALogLevel.ERROR, "DIJ2: %d %d\n", tid, curr_dist);

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
            // mgba_printf(MGBALogLevel.ERROR, "DIJ3: %d %d\n", nid, ndist);

            // if this is a better path
            if (ndist < node_distance[nid]) {
                // update distance
                node_distance[nid] = ndist;

                // add to unvisited
                unvisited.add(Heap!int.Node(node_distance[nid], nid));
            }
        }
    }

    // mgba_printf(MGBALogLevel.ERROR, "checking dists %d\n", node_distance.length);

    // finally, populate the rangebuf with the positions we can reach
    foreach (node_key; node_distance.byKey()) {
        // get info about this tile
        int tid = node_key;
        int dist = node_distance[node_key];

        // // mgba_printf(MGBALogLevel.ERROR, "CHECK: tid: %d, dist: %d\n", tid, dist);

        // ensure not starting tile
        if (tid == start_tid)
            continue;

        // ensure dist within range
        if (dist > range)
            continue;

        // ensure on board
        if (!board_util_tid_is_on_board(tid))
            continue;

        // // make sure no other pawn is there
        // if (board_get_pawn(tid))
        //     continue;

        // log this tile info
        // // mgba_printf(MGBALogLevel.ERROR, "ACCEPT: tid: %d, dist: %d\n", tid, dist);

        // add to rangebuf
        pos_buf[pos_buf_ix++] = board_util_tid_to_pos(tid);

        // if we have filled the rangebuf, return
        if (pos_buf_ix >= pos_buf_len)
            return pos_buf_ix;
    }

    return pos_buf_ix;
}
