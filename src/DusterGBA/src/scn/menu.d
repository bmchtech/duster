module scn.menu;

import core.stdc.string;
import dusk;
import tonc;
import libgba.maxmod;
import res;

extern (C):

void menu_start() {
    dusk_init_graphics_mode0();

    dusk_sprites_init();
    dusk_sprites_configure(false);

    // init tte
    *REG_DISPCNT |= DCNT_BG0;
    tte_init_chr4c(0, cast(u16) (BG_CBB(1) | BG_SBB(31)), 0, 0x0201, CLR_GRAY, null, null);
    tte_init_con();

    // load bg for title
    GritImage bg_img = dusk_load_image(cast(char*)"title");
    dusk_background_upload_raw(&bg_img, 0, 30);

    dusk_background_make(1, BG_REG_32x32, Background(/*x*/ 0, /*y*/ 0, /*cbb*/ 0, /*sbb*/ 30));

    // draw text
    pal_bg_mem[16] = RES_PAL[0];
    tte_printf("#{P:200,140}#{ci:16}play >");
}

void menu_update() {
    dusk_frame();

    // if (key_hit(KEY_A) || key_hit(KEY_START)) {
    //     // dusk_scene_set(levelselect_scene);
    // }

    // update sprites
    dusk_sprites_update();
}

void menu_end() {}

__gshared Scene menu_scene = Scene(&menu_start, &menu_end, &menu_update);