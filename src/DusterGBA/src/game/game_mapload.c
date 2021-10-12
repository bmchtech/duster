#include "game.h"
#include "../util/cute_tiled.h"
#include "string.h"
#include "contrib/mgba.h"

BOOL game_load_gamemap(void* data, u32 len) {
    cute_tiled_map_t* map = cute_tiled_load_map_from_memory(data, len, NULL);

    // set board size from width
    int board_size = map->width;

    // initialize board
    game_init_board(board_size);

    // init default teams
    game_init_team(0, "player");
    game_init_team(1, "enmy");

    BOOL terrain_layer_found = FALSE;
    BOOL pawns_layer_found = FALSE;

    cute_tiled_layer_t* layer = map->layers;
    while (layer) {
        int* data = layer->data;
        int data_count = layer->data_count;

        // stuff and things

        // terrain
        if (strcmp(layer->name.ptr, "terrain") == 0) {
            terrain_layer_found = TRUE;

            // copy tile data from terrain layer
            for (int i = 0; i < data_count; i++) {
                int tile = data[i];
                int tx = i % map->width;
                int ty = i / map->width;

                Terrain terrain = (Terrain)(tile - 1);

                board_set_terrain(BOARD_POS(tx, ty), terrain);
            }
        }

        // spawn points
        if ((strcmp(layer->name.ptr, "pawns") == 0) && layer->objects) {
            pawns_layer_found = TRUE;

            // array of pawn count for each team
            int team_pawn_count[NUM_TEAMS];
            memset(&team_pawn_count, 0, sizeof(team_pawn_count));

            cute_tiled_object_t* obj = layer->objects;
            while (obj) {
                // check if spawn
                if ((strcmp(obj->type.ptr, "pawn") == 0) && obj->property_count > 0) {
                    // add to spawn
                    int team_ix = -1;
                    int pawn_class = 0;
                    int pawn_level = 1;

                    for (int i = 0; i < obj->property_count; i++) {
                        cute_tiled_property_t* prop = &obj->properties[i];
                        if (strcmp(prop->name.ptr, "team") == 0) {
                            team_ix = prop->data.integer;
                        }
                        if (strcmp(prop->name.ptr, "class") == 0) {
                            pawn_class = prop->data.integer;
                        }
                        if (strcmp(prop->name.ptr, "level") == 0) {
                            pawn_level = prop->data.integer;
                        }
                    }

                    if (team_ix >= 0) {
                        VPos16 spawn_pos = (VPos16){.x = obj->x / 8, .y = obj->y / 8};

                        // get next open pawn slot using team counts
                        int pawn_ix = team_pawn_count[team_ix];
                        team_set_pawn(team_ix, pawn_ix, pawn_class);
                        // set level
                        Pawn* pawn = game_get_pawn_by_gid(PAWN_GID(team_ix, pawn_ix));
                        pawn->unit_data.level = pawn_level;
                        team_pawn_recalculate(team_ix, pawn_ix);
                        // increment pawn slot number
                        team_pawn_count[team_ix]++;

                        board_set_pawn(BOARD_POS(spawn_pos.x, spawn_pos.y), PAWN_GID(team_ix, pawn_ix));

                        mgba_printf(MGBA_LOG_ERROR, "spawned pawn (team: %d, pawn: %d, c: %d, l: %d) at pos (%d, %d)",
                                    team_ix, pawn_ix, pawn_class, pawn_level, spawn_pos.x, spawn_pos.y);
                    }
                }

                obj = obj->next;
            }
        }

        layer = layer->next;
    }

    // clean up map
    cute_tiled_free_map(map);

    BOOL valid = TRUE;

    if (!(terrain_layer_found && pawns_layer_found)) {
        valid = FALSE;
    }

    return valid;
}