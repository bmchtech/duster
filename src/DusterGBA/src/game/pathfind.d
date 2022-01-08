module game.pathfind;

import core.stdc.stdio;
import core.stdc.string;
import tonc;
import game;

import libtind.ds.vector;
import libtind.ds.dict;
import libtind.ds.heap;
import libtind.ds.set;

extern (C):

// old routine for range buffer calculation
/*
int board_util_calc_rangebuf(int start_tx, int start_ty, int range, VPos16* pos_buf, int pos_buf_len) {
    // clear rangebuf
    memset(pos_buf, 0, sizeof(VPos16) * pos_buf_len);
    int pos_buf_ix = 0;

    int start_tid = BOARD_POS(start_tx, start_ty);

    // storage for intermediate positions
    const int visit_tile_storage_size = 256;
    BFSVisitStorage visit_tile_storage[visit_tile_storage_size];
    int visit_tile_storage_index = 0;

    // do BFS

    // 0. data structures
    CC_HashSet* visited;
    CC_Deque* queue;

    // set up hashtable to work with int values
    CC_HashSetConf visited_conf;
    cc_hashset_conf_init(&visited_conf);
    visited_conf.hash = GENERAL_HASH;
    visited_conf.key_length = sizeof(int);
    cc_hashset_new_conf(&visited_conf, &visited);

    cc_deque_new(&queue);

    // 1. initial nodes
    cc_deque_add_first(queue, &start_tid);
    cc_hashset_add(visited, &start_tid);

    // 2. iterate
    while (cc_deque_size(queue) > 0) {
        int* curr_node_out;
        cc_deque_remove_first(queue, (void*)&curr_node_out);
        int curr_node = *curr_node_out;

        // add all unvisited neighbors
        // get neighbors
        tile_neighbors_t tn = board_util_get_neighbors(curr_node);
        // check them
        for (int i = 0; i < 4; i++) {
            int scan_node = tn.neighbors[i];

            if (scan_node < 0)
                continue; // invalid

            // calculate scan node values
            VPos16 scan_node_pos = board_util_tid_to_pos(scan_node);

            // make sure this tile in range
            if (board_dist(start_tx, start_ty, scan_node_pos.x, scan_node_pos.y) > range)
                continue;

            if (!board_util_is_walkable(scan_node_pos.x, scan_node_pos.y))
                continue;

            // check if visited
            if (!cc_hashset_contains(visited, &scan_node)) {
                // put in storage, then add to queues
                BFSVisitStorage* storage_slot = &visit_tile_storage[visit_tile_storage_index];

                // set storage slot values
                storage_slot->tile_id = scan_node;

                // ensure pos mem storage has space
                visit_tile_storage_index++;
                if (visit_tile_storage_index >= visit_tile_storage_size) {
                    mgba_printf(MGBA_LOG_ERROR, "bfs error, pos mem out of space");
                    return -1;
                }

                // now add to lists
                cc_deque_add_first(queue, &storage_slot->tile_id);
                cc_hashset_add(visited, &storage_slot->tile_id);
            }
        }
    }

    // dijkstra
    CC_HashTable* nodedist;
    CC_PQueue* unvisited;

    const int dij_storage_size = 256;
    DijkstraStorage dij_storage[dij_storage_size];
    PQueuePair pair_storage[dij_storage_size];
    int dij_storage_index = 0;

    CC_HashTableConf nodedist_conf;
    cc_hashtable_conf_init(&nodedist_conf);
    nodedist_conf.hash = GENERAL_HASH;
    nodedist_conf.key_length = sizeof(int);
    cc_hashtable_new_conf(&nodedist_conf, &nodedist);

    cc_pqueue_new(&unvisited, pqueue_pair_cmp);

    // add all nodes to unvisited
    CC_HashSetIter visited_iter_dij;
    cc_hashset_iter_init(&visited_iter_dij, visited);
    void* visited_iter_dij_next;
    while (cc_hashset_iter_next(&visited_iter_dij, &visited_iter_dij_next) != CC_ITER_END) {
        int tid = *(int*)visited_iter_dij_next;

        DijkstraStorage* st1 = &dij_storage[dij_storage_index];
        PQueuePair* p1 = &pair_storage[dij_storage_index];

        if (tid == start_tid) {
            st1->dist = 0;
        } else {
            st1->dist = 99999;
        }

        st1->ix = dij_storage_index;
        st1->value = tid;

        p1->ix = dij_storage_index;
        p1->prio = st1->dist;
        p1->value = tid;

        dij_storage_index++;

        cc_hashtable_add(nodedist, &st1->value, (void**)st1);
        cc_pqueue_push(unvisited, p1);
    }

    PQueuePair* current;
    while (cc_pqueue_top(unvisited, (void*)&current) == CC_OK) {
        int tid = current->value;

        // get storage entry
        DijkstraStorage* curr_entry;
        cc_hashtable_get(nodedist, &tid, (void*)&curr_entry);

        int curr_dist = curr_entry->dist;

        // VPos16 curr_pos = board_util_tid_to_pos(tid);
        // mgba_printf(MGBA_LOG_ERROR, "visit: (%d, %d), prio: %d, dist: %d", curr_pos.x, curr_pos.y, current->prio,
        //             curr_dist);

        tile_neighbors_t tn = board_util_get_neighbors(tid);

        // neighbors
        for (int i = 0; i < 4; i++) {
            int scan_node = tn.neighbors[i];
            if (scan_node < 0)
                continue;

            int cost = 1;

            // check if any pawns on that tile
            BoardTile* scan_tile = board_get_tile(scan_node);
            if (scan_tile->pawn_gid > 0) {
                // a pawn is here
                cost += 1;
            }

            int scan_dist = curr_dist + cost;

            // get storage entry
            DijkstraStorage* nb_entry;
            cc_hashtable_get(nodedist, &scan_node, (void**)&nb_entry);
            PQueuePair* nb_pair = &pair_storage[nb_entry->ix];

            // VPos16 scan_pos = board_util_tid_to_pos(scan_node);
            // mgba_printf(MGBA_LOG_ERROR, "neighbor: (%d, %d), dist: %d", scan_pos.x, scan_pos.y, scan_dist);

            // try to set dist
            if (scan_dist < nb_entry->dist) {
                // set entry dist
                nb_entry->dist = scan_dist;

                // update priority

                // find index of prio slot
                int pq_ix = -1;
                for (int j = 0; j < unvisited->size; j++) {
                    PQueuePair* item = (PQueuePair*)unvisited->buffer[j];
                    if (nb_pair->prio == item->prio) {
                        pq_ix = j;
                    }
                }

                if (pq_ix >= 0) {
                    // set value
                    nb_pair->prio = nb_entry->dist;
                    // percolate
                    cc_pqueue_percolate(unvisited, pq_ix);
                }
            }
        }

        cc_pqueue_heapify(unvisited, 0, true);

        // done visit
        cc_pqueue_pop(unvisited, (void*)current);
    }

    // copy visited data to pos buf
    CC_HashSetIter visited_iter;
    cc_hashset_iter_init(&visited_iter, visited);
    void* visited_iter_next;
    while (cc_hashset_iter_next(&visited_iter, &visited_iter_next) != CC_ITER_END) {
        int iter_val = *(int*)visited_iter_next;
        // mgba_printf(MGBA_LOG_ERROR, "visited iter: %d", iter_val);

        int scan_tid = iter_val;
        VPos16 scan_pos = board_util_tid_to_pos(scan_tid);

        // check the distance using our shortest path
        DijkstraStorage* scan_tile_shortest_entry;
        cc_hashtable_get(nodedist, &scan_tid, (void**)&scan_tile_shortest_entry);

        // mgba_printf(MGBA_LOG_ERROR, "shortest dist to (%d,%d): %d", scan_pos.x, scan_pos.y,
        // scan_tile_shortest_entry->dist);

        if (scan_tile_shortest_entry->dist > range)
            continue;

        // ensure not starting point
        if (scan_pos.x == start_tx && scan_pos.y == start_ty)
            continue;
        // ensure on board
        if (!board_util_is_on_board(scan_pos.x, scan_pos.y))
            continue;

        // // make sure no other pawn is there
        // if (board_get_pawn(BOARD_POS(scan_pos.x, scan_pos.y)))
        //     continue;

        pos_buf[pos_buf_ix] = scan_pos;
        pos_buf_ix++;

        // if pos buf is full, leave
        if (pos_buf_ix >= pos_buf_len - 1)
            return pos_buf_ix;
    }

    // clean up
    cc_hashset_destroy(visited);
    cc_deque_destroy(queue);
    cc_hashtable_destroy(nodedist);
    cc_pqueue_destroy(unvisited);

    return pos_buf_ix;
}
*/

int board_util_calc_rangebuf(int start_tx, int start_ty, int range, VPos16* pos_buf, int pos_buf_len) {
    // clear rangebuf
    memset(pos_buf, 0, (VPos16.sizeof) * pos_buf_len);
    int pos_buf_ix = 0;

    int start_tid = BOARD_POS(start_tx, start_ty);

    // create data structures
    Dict!(int, int) node_distance;
    Set!int node_visited;
    Heap!int node_queue;


    // free data structures
    node_distance.free();
    node_visited.free();
    node_queue.free();

    return pos_buf_ix;
}