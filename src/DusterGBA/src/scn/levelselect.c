#include <string.h>
#include "dusk.h"
#include <tonc.h>
#include "res.h"
#include "scenes.h"
#include "contrib/mgba.h"

BOOL levelselect_dirty = TRUE;
int levelselect_selected = 0;

typedef struct {
    char name[32];
    char map_file[32];
    char description[4][128];
    int description_lines;
} LevelInfo;

EWRAM_DATA LevelInfo level_infos[16];
int level_infos_count = 0;

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

    // default levelinfo
    memset(level_infos, 0, sizeof(level_infos));

    LevelInfo* lev0 = &level_infos[0];
    sprintf(lev0->name, "helo1 demo");
    sprintf(lev0->map_file, "helo1");
    sprintf(lev0->description[0], "%s", "The testing demo used for main game tests.");
    sprintf(lev0->description[1], "%s", "This is a simple map.");
    sprintf(lev0->description[2], "%s", "Bean.");
    lev0->description_lines = 3;

    LevelInfo* lev1 = &level_infos[1];
    sprintf(lev1->name, "test3 demo");
    sprintf(lev1->map_file, "test3");
    sprintf(lev1->description[0], "%s", "The secondary testing map.");
    sprintf(lev1->description[1], "%s", "Auxiliary");
    sprintf(lev1->description[2], "%s", "page when looking at its layout.");
    lev1->description_lines = 3;

    LevelInfo* lev2 = &level_infos[2];
    sprintf(lev2->name, "underground");
    sprintf(lev2->map_file, "underground");
    sprintf(lev2->description[0], "%s", "A shitty clone of a Fire Emblem Three Houses");
    sprintf(lev2->description[1], "%s", "Map. It is not very good.");
    sprintf(lev2->description[2], "%s", "Cheesy crispy.");
    lev2->description_lines = 3;

    level_infos_count = 3;
}

void draw_levelselect_ui() {
    tte_erase_screen();

    LevelInfo *info = &level_infos[levelselect_selected];

    // draw level name
    tte_printf("#{P:8,8}#{ci:1}map selection");
    tte_printf("#{P:16,20}#{ci:1}< %s >", info->name);

    // draw level description
    tte_printf("#{P:8,36}#{ci:1}description");
    // tte_printf("#{P:16,48}#{ci:1}It is a long established fact that a reader will be distracted by the readable content of a page when looking at its layout. The point of using Lorem Ipsum is that it has a more-or-less normal distribution of letters, as opposed to using 'Content here, content here', making it look like readable English.");
    // draw line by line
    for (int i = 0; i < info->description_lines; i++) {
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
        levelselect_selected = (levelselect_selected + lr_input + level_infos_count) % level_infos_count;
        levelselect_dirty = TRUE;
    }

    if (level_infos_count > 0 && levelselect_dirty) {
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