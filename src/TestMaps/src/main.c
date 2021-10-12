#include <stdio.h>
#include "raylib.h"
#include "inc.h"

#define CUTE_TILED_NO_EXTERNAL_TILESET_WARNING
#define CUTE_TILED_IMPLEMENTATION

#include "cute_tiled.h"

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Window title");
    SetTargetFPS(60);

    // open TMX
    const char *tmx_file = "../../DusterGBA/asset/tmx/test1.json";
    FILE *inf_fp = fopen(tmx_file, "rb");
    if (inf_fp == NULL)
    {
        fprintf(stderr, "cannot open input file\n");
        return 1;
    }

    FileReadResult inf_read = util_read_file_contents(inf_fp);
    fclose(inf_fp);

    cute_tiled_map_t *map = cute_tiled_load_map_from_memory(inf_read.content, inf_read.size, NULL);

    // get map width and height
    int w = map->width;
    int h = map->height;

    // loop over the map's layers
    cute_tiled_layer_t *layer = map->layers;
    while (layer)
    {
        int *data = layer->data;
        int data_count = layer->data_count;

        printf("beep: LAYER NAME: %s\n", layer->name);

        layer = layer->next;
    }

    cute_tiled_free_map(map);

    Texture2D texture = LoadTexture(ASSETS_PATH "test.png");

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        const int texture_x = SCREEN_WIDTH / 2 - texture.width / 2;
        const int texture_y = SCREEN_HEIGHT / 2 - texture.height / 2;
        DrawTexture(texture, texture_x, texture_y, WHITE);

        const char *text = "OMG! IT WORKS!";
        const Vector2 text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
        DrawText(text, SCREEN_WIDTH / 2 - text_size.x / 2, texture_y + texture.height + text_size.y + 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
