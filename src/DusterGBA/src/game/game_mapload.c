#include "game.h"
#include "../util/cute_tiled.h"

GameMap load_game_map(u8* data, u32 len) {
    cute_tiled_map_t* map = cute_tiled_load_map_from_memory(data, len, NULL);
    // cute_tiled_layer_t* layer = map->layers;
    // while (layer) {
    //     int* data = layer->data;
    //     int data_count = layer->data_count;

    //     // stuff and things

    //     layer = layer->next;
    // }
    GameMap ret;
    ret.board_size = 1;
    return ret;
}