#include <string.h>
#include "dusk.h"
#include "tonc.h"
#include "res.h"

Sprite* menu;
Anim th_anim1;

void menu_start() {
    dusk_init_graphics_mode0();

    pal_bg_mem[0] = RES_PAL[2]; // bg col
    

    // text

    REG_DISPCNT |= DCNT_BG1;
    tte_init_chr4c(1, BG_CBB(0)|BG_SBB(31), 0, 0x0201, CLR_GRAY, NULL, NULL);
    tte_init_con();

    pal_bg_mem[2] = RES_PAL[0];

    tte_printf("#{P:12,12}#{ci:2}duster");
    tte_printf("#{P:200,140}#{ci:2}play >");
}

void menu_update() {
    dusk_frame();

    // update sprites
    dusk_sprites_update();
}

void menu_end() {}

Scene menu_scene = {
    .start = menu_start,
    .update = menu_update,
    .end = menu_end,
};