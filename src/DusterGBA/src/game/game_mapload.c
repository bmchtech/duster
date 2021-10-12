#include "game.h"
#include "../util/cute_tiled.h"
#include "string.h"

GameMap load_game_map(u8* data, u32 len) {
    cute_tiled_map_t* map = cute_tiled_load_map_from_memory(data, len, NULL);

    GameMap ret;
    ret.valid = TRUE;

    // set board size from width
    ret.board_size = map->width;

    BOOL terrain_layer_found = FALSE;

    cute_tiled_layer_t* layer = map->layers;
    while (layer) {
        int* data = layer->data;
        int data_count = layer->data_count;

        // stuff and things
        if (strcmp(layer->name.ptr, "terrain") == 0) {
            terrain_layer_found = TRUE;

            // copy tile data from terrain layer
        }

        layer = layer->next;
    }

    if (!terrain_layer_found) {
        ret.valid = FALSE;
    }

    cute_tiled_free_map(map);

    return ret;
}