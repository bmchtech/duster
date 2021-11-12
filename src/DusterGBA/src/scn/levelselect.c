#include <string.h>
#include "dusk.h"
#include <tonc.h>
#include "res.h"
#include "scenes.h"
#include "contrib/mgba.h"

BOOL levelselect_dirty = TRUE;
int levelselect_selected = 0;

#define NUM_LEVEL_INFOS 2
#define LEVELINFO_DESCRIPTION_LINES 4

typedef struct {
    char name[32];
    char map_file[32];
    char description[LEVELINFO_DESCRIPTION_LINES][128];
} LevelInfo;

const LevelInfo level_infos[NUM_LEVEL_INFOS] = {
    (LevelInfo){
        .name = "helo demo",
        .map_file = "helo1",
        .description = {
            "The testing demo used for main game tests.",
            "",
            "",
            "",
        }
    },
    (LevelInfo){
        .name = "underground",
        .map_file = "underground",
        .description = {
            "A shitty clone of a Fire Emblem Three Houses",
            "Map. It is not very good.",
            "Cheesy crispy.",
        }
    },
};

void levelselect_start() {
    dusk_init_graphics_mode0();

    dusk_sprites_init();
    dusk_sprites_configure(FALSE);

    // main background
    REG_DISPCNT |= DCNT_BG0;
    pal_bg_mem[0] = RES_PAL[2];

    // init tte
    REG_DISPCNT |= DCNT_BG1;
    tte_init_chr4c(1, BG_CBB(1) | BG_SBB(31), 0, 0x0201, CLR_GRAY, NULL, NULL);
    tte_init_con();

    // draw text
    pal_bg_mem[1] = RES_PAL[0];
}

void draw_levelselect_ui() {
    tte_erase_screen();

    const LevelInfo *info = &level_infos[levelselect_selected];

    // draw level name
    tte_printf("#{P:8,8}#{ci:1}map selection");
    tte_printf("#{P:16,20}#{ci:1}< %s >", info->name);

    // draw level description
    tte_printf("#{P:8,36}#{ci:1}description");
    // tte_printf("#{P:16,48}#{ci:1}It is a long established fact that a reader will be distracted by the readable content of a page when looking at its layout. The point of using Lorem Ipsum is that it has a more-or-less normal distribution of letters, as opposed to using 'Content here, content here', making it look like readable English.");
    // draw line by line
    for (int i = 0; i < LEVELINFO_DESCRIPTION_LINES; i++) {
        int descline_y = 48 + (i * 12);
        tte_printf("#{P:16,%d}#{ci:1}%s", descline_y, info->description[i]);
    }

    tte_printf("#{P:200,140}#{ci:1}play >");
}

void levelselect_update() {
    dusk_frame();

    if (key_hit(KEY_A) || key_hit(KEY_START)) {
        // selected, copy data
        strcpy(selected_map_file, level_infos[levelselect_selected].map_file);

        // switch to scene
        dusk_scene_set(board_scene);
    }

    int lr_touched = key_transit(KEY_LEFT | KEY_RIGHT);
    int lr_input = key_tri_horz();
    if (lr_input != 0 && lr_touched) {
        // change selected item
        levelselect_selected = (levelselect_selected + lr_input + NUM_LEVEL_INFOS) % NUM_LEVEL_INFOS;
        levelselect_dirty = TRUE;
    }

    if (NUM_LEVEL_INFOS > 0 && levelselect_dirty) {
        levelselect_dirty = FALSE;

        draw_levelselect_ui();
    }

    // update sprites
    dusk_sprites_update();
}

void levelselect_end() {}

Scene levelselect_scene = {
    .start = levelselect_start,
    .update = levelselect_update,
    .end = levelselect_end,
};