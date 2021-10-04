#include <string.h>
#include "dusk.h"
#include <tonc.h>
#include "scenes.h"
#include "res.h"
#include "soundbank.h"
#include "maxmod.h"

Sprite* logo;
const int FADE_LENGTH = 30; // fade length in frames
int start_frame;
int fade_step; // frames per fade unit
mm_sound_effect intro_chime;

void logo_init_audio() {
    // irq setup for maxmod dma
    irq_init(NULL);
    irq_add(II_VBLANK, mmVBlank);
    irq_enable(II_VBLANK);

    // load soundbank
    u32 soundbank_len;
    const u32* soundbank_bin = gbfs_get_obj(gbfs_dat, "soundbank.bin", &soundbank_len);

    // initialise maxmod with soundbank and 8 channels
    mmInitDefault((mm_addr)soundbank_bin, 8);

    // define sfx
    intro_chime.handle = 0;
    intro_chime.id = SFX_INTRO;
    intro_chime.rate = (int)(1.0f * (1 << 10));
    intro_chime.volume = 255;
    intro_chime.panning = 128;
}

void logo_start() {
    dusk_init_graphics_mode0();

    logo_init_audio();

    REG_DISPCNT |= DCNT_BG0;

    start_frame = frame_count;

    // load sprite atlas
    dusk_sprites_init();
    dusk_sprites_configure(FALSE);
    SpriteAtlas atlas = dusk_load_atlas("a_logo");
    dusk_sprites_upload_atlas(&atlas);

    logo = dusk_sprites_make(0, 64, 64,
                             (Sprite){
                                 .x = SCREEN_WIDTH / 2 - 32,
                                 .y = SCREEN_HEIGHT / 2 - 32,
                                 .base_tid = 0,
                             });

    // enable blend on this object
    OBJ_ATTR* logo_attr = &obj_buffer[0];
    obj_set_attr(logo_attr, logo_attr->attr0 | ATTR0_BLEND, logo_attr->attr1, logo_attr->attr2);

    // set up blending registers
    REG_BLDCNT = BLD_OBJ | BLD_BG0 | BLD_BG1 | BLD_BLACK;
    REG_BLDY = BLDY_BUILD(16);

    fade_step = FADE_LENGTH / 16;

    // pal_bg_mem[0] = RES_PAL[0]; // bg col

    // ----------

    REG_DISPCNT |= DCNT_BG1;
    tte_init_chr4c(1, BG_CBB(0) | BG_SBB(31), 0, 0x0201, CLR_WHITE, NULL, NULL);
    tte_init_con();

    // pal_gradient_ex(pal_bg_mem, 1, 4, 0x6F98, 0x4964);
    pal_bg_mem[2] = RES_PAL[4];
    pal_bg_mem[3] = RES_PAL[2];

    tte_printf("#{P:92,24}#{ci:2}bean machine");
    tte_printf("#{P:100,36}#{ci:3}presents");

    // play sound effect
    mmEffectEx(&intro_chime);
}

void logo_update() {
    dusk_frame();
    mmFrame();

    int progress = (frame_count - start_frame);
    if (progress <= FADE_LENGTH) {
        int fade = clamp(progress / fade_step, 0, 16);
        REG_BLDY = BLDY_BUILD(16 - fade);
    } else if (progress <= FADE_LENGTH * 2) {
        int fade = clamp((progress - FADE_LENGTH) / fade_step, 0, 16);
        REG_BLDY = BLDY_BUILD(fade);
    }
    if (progress > FADE_LENGTH * 2) {
        // done
        dusk_scene_set(menu_scene);
    }

    // update sprites
    dusk_sprites_update();
}

void logo_end() {
    // clear blending registers
    REG_BLDCNT = BLD_OFF;
}

Scene logo_scene = {
    .start = logo_start,
    .update = logo_update,
    .end = logo_end,
};