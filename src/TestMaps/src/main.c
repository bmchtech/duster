#include <stdio.h>
#include "raylib.h"
#include "inc.h"
#include "stdbool.h"

#define CUTE_TILED_NO_EXTERNAL_TILESET_WARNING
#define CUTE_TILED_IMPLEMENTATION

#include "cute_tiled.h"

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)

int main(void)
{
    // open TMX
    const char *tmx_file = "../../DusterGBA/asset/gamemap/test1.json";
    FILE *inf_fp = fopen(tmx_file, "rb");
    if (inf_fp == NULL)
    {
        fprintf(stderr, "cannot open input file\n");
        return 1;
    }

    FileReadResult inf_read = util_read_file_contents(inf_fp);
    fclose(inf_fp);

    cute_tiled_map_t *map = cute_tiled_load_map_from_memory(inf_read.content, inf_read.size, NULL);

    printf("map width: %d\n", map->width);

    bool terrain_layer_found = false;
    bool pawns_layer_found = false;

    cute_tiled_layer_t *layer = map->layers;
    while (layer)
    {
        int *data = layer->data;
        int data_count = layer->data_count;

        // stuff and things

        // terrain
        if (strcmp(layer->name.ptr, "terrain") == 0)
        {
            terrain_layer_found = true;

            // copy tile data from terrain layer
        }

        // spawn points
        if ((strcmp(layer->name.ptr, "pawns") == 0) && layer->objects)
        {
            pawns_layer_found = true;

            cute_tiled_object_t *obj = layer->objects;
            while (obj)
            {
                printf("checking obj: %s (%d)\n", obj->name, obj->id);

                // check if spawn
                if ((strcmp(obj->type.ptr, "pawn") == 0) && obj->property_count > 0)
                {
                    // add to spawn
                    int pawn_ix = -1;
                    int team_ix = -1;

                    for (int i = 0; i < obj->property_count; i++)
                    {
                        cute_tiled_property_t *prop = &obj->properties[i];
                        if (strcmp(prop->name.ptr, "pawn") == 0)
                        {
                            pawn_ix = prop->data.integer;
                        }
                        if (strcmp(prop->name.ptr, "team") == 0)
                        {
                            team_ix = prop->data.integer;
                        }
                    }

                    if (pawn_ix >= 0 && team_ix >= 0)
                    {
                        printf("spawn: pos: (%d, %d), team: %d, pawn: %d\n", obj->x, obj->y, team_ix, pawn_ix);
                    }
                }

                obj = obj->next;
            }
        }

        layer = layer->next;
    }

    if (!(terrain_layer_found && pawns_layer_found))
    {
        printf("not all stuff found!\n");
    }
    else
    {
        printf("DONE WITH MAP ---\n\n\n");
    }

    cute_tiled_free_map(map);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Window title");
    SetTargetFPS(60);

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
