module scn.levelselect;

import core.stdc.string;
import dusk;
import tonc;
import libgba.maxmod;
import res;
import dusk.contrib.mgba;
import scn.board;

extern (C):

__gshared bool levelselect_dirty = false;
__gshared int levelselect_selected = 0;

enum NUM_LEVEL_INFOS = 2;
enum LEVELINFO_DESCRIPTION_LINES = 4;

struct LevelInfo {
    string name;
    string map_file;
    string[LEVELINFO_DESCRIPTION_LINES] description;
}


const LevelInfo[] level_infos = [
    LevelInfo("helo demo", "helo1", [
        "The testing demo used for main game tests.",
        "",
        "",
        "",
    ]),
    LevelInfo("underground", "underground", [
        "A shitty clone of a Fire Emblem Three Houses",
        "Map. It is not very good.",
        "Cheesy crispy.",
        ""
    ]),
];

void levelselect_start() {
    dusk_init_graphics_mode0();

    dusk_sprites_init();
    dusk_sprites_configure(false);

    // main background
    *REG_DISPCNT |= DCNT_BG0;
    pal_bg_mem[0] = RES_PAL[2];

    // init tte
    *REG_DISPCNT |= DCNT_BG1;
    tte_init_chr4c(1, cast(u16) (BG_CBB(1) | BG_SBB(31)), 0, 0x0201, CLR_GRAY, null, null);
    tte_init_con();

    // text color
    pal_bg_mem[1] = RES_PAL[0];

    // draw first time
    levelselect_dirty = true;
}

void draw_levelselect_ui() {
    tte_erase_screen();

    const LevelInfo *info = &level_infos[levelselect_selected];

    // draw level name
    tte_printf("#{P:8,8}#{ci:1}map selection");
    tte_printf("#{P:16,20}#{ci:1}< %s >", cast(char*) info.name);

    // draw level description
    tte_printf("#{P:8,36}#{ci:1}description");
    // tte_printf("#{P:16,48}#{ci:1}It is a long established fact that a reader will be distracted by the readable content of a page when looking at its layout. The point of using Lorem Ipsum is that it has a more-or-less normal distribution of letters, as opposed to using 'Content here, content here', making it look like readable English.");
    // draw line by line
    for (int i = 0; i < LEVELINFO_DESCRIPTION_LINES; i++) {
        int descline_y = 48 + (i * 12);
        tte_printf("#{P:16,%d}#{ci:1}%s", descline_y, cast(char*) info.description[i]);
    }

    tte_printf("#{P:200,140}#{ci:1}play >");
}

void levelselect_update() {
    dusk_frame();

    if (key_hit(KEY_A) || key_hit(KEY_START)) {
        sfx_play_aux1();

        // selected, copy data
        strcpy(cast(char*) selected_map_file, cast(char*) level_infos[levelselect_selected].map_file);

        // switch to scene
        dusk_scene_set(board_scene);
    }

    int lr_touched = key_transit(KEY_LEFT | KEY_RIGHT);
    int lr_input = key_tri_horz();
    if (lr_input != 0 && lr_touched) {
        // change selected item
        levelselect_selected = (levelselect_selected + lr_input + NUM_LEVEL_INFOS) % NUM_LEVEL_INFOS;
        levelselect_dirty = true;
    }

    if (NUM_LEVEL_INFOS > 0 && levelselect_dirty) {
        levelselect_dirty = false;

        draw_levelselect_ui();
    }

    // update sprites
    dusk_sprites_update();
}

void levelselect_end() {}

__gshared Scene levelselect_scene = Scene(&levelselect_start, &levelselect_end, &levelselect_update);