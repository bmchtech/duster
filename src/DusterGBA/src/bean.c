#include <string.h>
#include "dusk.h"
#include "tonc.h"
#include "scene.h"
#include "wasm3.h"
#include "bean.h"

Sprite* bean;
const int FADE_LENGTH = 60; // fade length in frames
int start_frame;
int fade_step;

void bean_start() {
    dusk_init_graphics_mode0();

    start_frame = frame_count;

    pal_bg_mem[0] = 0x0C02; // background color

	REG_DISPCNT |= DCNT_BG1;
	tte_init_chr4c(1, BG_CBB(0)|BG_SBB(31), 0, 0x0201, CLR_WHITE, &verdana9Font, NULL);
	tte_init_con();

    pal_gradient_ex(pal_bg_mem, 1, 4, CLR_YELLOW, CLR_ORANGE);
    pal_gradient_ex(pal_bg_mem, 5, 8, CLR_BLACK, CLR_WHITE);

    tte_printf("#{P:12,0}#{ci:4}Wasm #{ci:2}demo");
    //tte_printf("#{P:12,24}#{ci:2}bean");
    //tte_printf("#{P:12,36}#{ci:7}start >");

    wasm_bean_init();
    wasm_bean_start();
}

void bean_update() {
    dusk_frame();

    // update sprites
    dusk_sprites_update();

    if (key_hit(KEY_A)) {
        tte_erase_screen();
        tte_printf("#{P:12,84}#{ci:4}Bean #{ci:2}demo");
    }

    wasm_bean_update();
}

void bean_end() {
    // clear blending registers
    REG_BLDCNT = BLD_OFF;
}

Scene bean_scene = {
    .start = bean_start,
    .update = bean_update,
    .end = bean_end,
};
