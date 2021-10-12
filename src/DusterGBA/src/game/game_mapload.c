#include "game.h"
#include "../util/cute_tiled.h"
#include "string.h"

GameMap load_game_map(u8* data, u32 len) {
    cute_tiled_map_t* map = cute_tiled_load_map_from_memory(data, len, NULL);

    GameMap ret;
    // reset
    memset(&ret, 0, sizeof(GameMap));

    ret.valid = TRUE;

    // set board size from width
    ret.board_size = map->width;

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
        }

        // spawn points
        if ((strcmp(layer->name.ptr, "pawns") == 0) && layer->objects) {
            pawns_layer_found = TRUE;

            cute_tiled_object_t* obj = layer->objects;
            while (obj) {
                // check if spawn
                if ((strcmp(obj->type.ptr, "pawn") == 0) && obj->property_count > 0) {
                    // add to spawn
                    int pawn_ix = -1;
                    int team_ix = -1;

                    for (int i = 0; i < obj->property_count; i++) {
                        cute_tiled_property_t* prop = &obj->properties[i];
                        if (strcmp(prop->name.ptr, "pawn") == 0) {
                            pawn_ix = prop->data.integer;
                        }
                        if (strcmp(prop->name.ptr, "team") == 0) {
                            team_ix = prop->data.integer;
                        }
                    }

                    if (pawn_ix >= 0 && team_ix >= 0) {
                        PawnSpawnPoint* spawn = &ret.pawn_spawn[PAWN_GID(team_ix, pawn_ix)];
                        spawn->valid = TRUE;
                        spawn->pos = (VPos16){.x = obj->x, .y = obj->y};
                        spawn->team = team_ix;
                        spawn->pawn = pawn_ix;
                    }
                }

                obj = obj->next;
            }
        }

        layer = layer->next;
    }

    if (!(terrain_layer_found && pawns_layer_found)) {
        ret.valid = FALSE;
    }

    cute_tiled_free_map(map);

    return ret;
}