#include "game.h"
#include <stdio.h>
#include <memory.h>
#include <tonc.h>
#include "cold_data.h"
#include "cc_queue.h"
#include "cc_pqueue.h"
#include "cc_hashset.h"
#include "contrib/mgba.h"

BOOL board_util_is_on_board(int tx, int ty) {
    return ((tx >= 0) && tx < game_state.board_size) && ((ty >= 0) && ty < game_state.board_size);
}

BOOL board_util_is_walkable(int tx, int ty) {
    if (!board_util_is_on_board(tx, ty))
        return FALSE;

    // check terrain
    Terrain terrain = board_get_terrain(BOARD_POS(tx, ty));
    if (terrain == TERRAIN_GROUND)
        return TRUE;

    return FALSE;
}

tile_neighbors_t board_util_get_neighbors(int tile_id) {
    tile_neighbors_t ret;

    VPos16 tile_pos = board_util_tid_to_pos(tile_id);

    // fill the neighbors
    // north, east, south, west
    // if not valid, then -1

    ret.neighbors[0] = -1;
    ret.neighbors[1] = -1;
    ret.neighbors[2] = -1;
    ret.neighbors[3] = -1;

    // north
    if (board_util_is_on_board(tile_pos.x, tile_pos.y - 1)) {
        ret.neighbors[0] = BOARD_POS(tile_pos.x, tile_pos.y - 1);
    }
    // east
    if (board_util_is_on_board(tile_pos.x + 1, tile_pos.y)) {
        ret.neighbors[1] = BOARD_POS(tile_pos.x + 1, tile_pos.y);
    }
    // south
    if (board_util_is_on_board(tile_pos.x, tile_pos.y + 1)) {
        ret.neighbors[2] = BOARD_POS(tile_pos.x, tile_pos.y + 1);
    }
    // west
    if (board_util_is_on_board(tile_pos.x - 1, tile_pos.y)) {
        ret.neighbors[3] = BOARD_POS(tile_pos.x - 1, tile_pos.y);
    }

    return ret;
}

// check whether a tile id is a neighbor
BOOL board_util_is_neighbor(int tile_id, int neighbor_id) {
    tile_neighbors_t neighbors = board_util_get_neighbors(tile_id);
    return neighbors.neighbors[0] == neighbor_id || neighbors.neighbors[1] == neighbor_id || neighbors.neighbors[2] == neighbor_id || neighbors.neighbors[3] == neighbor_id;
}

// get tile board position from tid
VPos16 board_util_tid_to_pos(int tile_id) {
    return (VPos16) {tile_id % MAX_BOARD_SIZE, tile_id / MAX_BOARD_SIZE};
}

// get tid from board position
int board_util_pos_to_tid(VPos16 pos) {
    return BOARD_POS(pos.x, pos.y);
}

typedef struct {
    int tile_id;
} BFSVisitStorage;

typedef struct {
    int dist;
    int ix;
    int value;
} DijkstraStorage;

typedef struct {
    int value;
    int prio;
    int ix;
} PQueuePair;

static int pqueue_pair_cmp(const void* a, const void* b) {
    PQueuePair* a1 = (PQueuePair*)a;
    PQueuePair* b1 = (PQueuePair*)b;

    return b1->prio - a1->prio;
}

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

BOOL pawn_util_is_valid_move(pawn_gid_t pawn_gid, VPos16 start_pos, VPos16 end_pos) {
    ClassData* class_data = pawn_get_classdata(pawn_gid);

    int pawn_max_move = class_data->move;

    // check dist from start to end tiles
    int start_end_dist = board_dist(start_pos.x, start_pos.y, end_pos.x, end_pos.y);

    // check if dist exceeds max move
    if (start_end_dist > pawn_max_move)
        return FALSE;

    return TRUE;
}

BOOL pawn_util_is_valid_interaction(pawn_gid_t pawn1_gid, VPos16 pawn1_pos, pawn_gid_t pawn2_gid, VPos16 pawn2_pos) {
    // ClassData* class_data = pawn_get_classdata(pawn_gid);

    // int pawn_max_move = class_data->move;

    // // check dist from start to end tiles
    // int start_end_dist = board_dist(start_pos.x, start_pos.y, end_pos.x, end_pos.y);

    // // check if dist exceeds max move
    // if (start_end_dist > pawn_max_move)
    //     return FALSE;

    return TRUE;
}

BOOL pawn_util_on_same_team(pawn_gid_t pawn1, pawn_gid_t pawn2) {
    return PAWN_WHICH_TEAM(pawn1) == PAWN_WHICH_TEAM(pawn2);
}

BOOL game_util_is_my_turn(pawn_gid_t pawn_gid) {
    int my_team = PAWN_WHICH_TEAM(pawn_gid);
    int whose_turn = game_util_whose_turn();

    return my_team == whose_turn;
}

int game_util_whose_turn() {
    int whose_turn = game_state.turns % NUM_TEAMS;
    return whose_turn;
}

BOOL pawn_util_moved_this_turn(Pawn* pawn) {
    if (pawn->last_moved_turn >= game_state.turns)
        return TRUE;
    return FALSE;
}

// this applies the growth spread out over every 5 levels
// for example, with an atk growth of 4, it will spread those
// 4 atk stat boosts over every 5 levels
#define MACRO_CALC_STATS_GROWTH(stat) calc_stats.stat = base.stat + ((growth.stat * num_growths) / 5)

UnitDataStats pawn_util_calc_stats(ClassData* class_data, int level) {
    int num_growths = (level - 1);
    UnitDataStats base = class_data->base_stats;
    UnitDataStats growth = class_data->stat_growths;

    UnitDataStats calc_stats = base;

    // compute additional stats based on adding to base stats
    // calc_stats.atk = base.atk + (num_growths * growth.atk);
    // calc_stats.def = base.def + (num_growths * growth.def);
    // calc_stats.hp = base.hp + (num_growths * growth.hp);

    // use a macro to spread out the growth over every 5 levels

    MACRO_CALC_STATS_GROWTH(atk);
    MACRO_CALC_STATS_GROWTH(def);
    MACRO_CALC_STATS_GROWTH(hp);
    MACRO_CALC_STATS_GROWTH(spd);

    return calc_stats;
}