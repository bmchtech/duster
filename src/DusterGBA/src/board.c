#include <string.h>
#include "dusk.h"
#include "tonc.h"
#include "res.h"
#include "contrib/mgba.h"

Sprite *board;
Anim th_anim1;

void board_start()
{
    dusk_init_graphics_mode0();

    // main bg
    REG_DISPCNT |= DCNT_BG0;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(31);

    pal_bg_mem[0] = RES_PAL[0];
    pal_bg_mem[1] = RES_PAL[2];

    // try drawing
    TSurface srf;
    srf_init(&srf, SRF_CHR4C, tile_mem[0], 240, 160, 4, pal_bg_mem);
    schr4c_prep_map(&srf, se_mem[31], 0);

    schr4r_line(&srf, 20, 20, 30, 30, 1);

    // // clear CBB 0
    // memset32(tile_mem[0], 0x00000000, 4096);
    // schr4c_prep_map(&srf, se_mem[31], 0);

    // nocash_puts("bean");

    mgba_open();
    mgba_printf(MGBA_LOG_INFO, "bean");
}

void board_update()
{
    dusk_frame();

    // update sprites
    dusk_sprites_update();
}

void board_end() {}

Scene board_scene = {
    .start = board_start,
    .update = board_update,
    .end = board_end,
};