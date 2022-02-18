#include "soundbank.h"
#include "maxmod.h"

void sfx_play_intro_chime() {
    mm_sound_effect intro_chime;
    // define sfx
    intro_chime.handle = 0;
    intro_chime.id = SFX_INTRO;
    intro_chime.rate = (int)(1.0f * (1 << 10));
    intro_chime.volume = 255;
    intro_chime.panning = 128;
    // play sound effect
    mmEffectEx(&intro_chime);
}

void sfx_play_startchime() {
    // define sfx
    mm_sound_effect chime;
    chime.handle = 0;
    chime.id = SFX_OBEP2;
    chime.rate = (int)(1.2f * (1 << 10));
    chime.volume = 200;
    chime.panning = 128;
    // play sound effect
    mmEffectEx(&chime);
}

void sfx_play_pause() {
    mm_sound_effect sfx;
    sfx.handle = 0;
    sfx.id = SFX_CB04;
    sfx.rate = (int)(1.0f * (1 << 10));
    sfx.volume = 255;
    sfx.panning = 128;
    // play sound effect
    mmEffectEx(&sfx);
}

void sfx_play_aux1() {
    mm_sound_effect sfx;
    sfx.handle = 0;
    sfx.id = SFX_CL04;
    sfx.rate = (int)(1.0f * (1 << 10));
    sfx.volume = 255;
    sfx.panning = 128;
    // play sound effect
    mmEffectEx(&sfx);
}

void sfx_play_scroll() {
    // mm_sound_effect sfx;
    // sfx.handle = 0;
    // sfx.id = SFX_HT01;
    // sfx.rate = (int)(1.0f * (1 << 10));
    // sfx.volume = 255;
    // sfx.panning = 128;
    // // play sound effect
    // mmEffectEx(&sfx);
}

void sfx_play_click() {
    mm_sound_effect sfx;
    sfx.handle = 0;
    sfx.id = SFX_SS02;
    sfx.rate = (int)(1.0f * (1 << 10));
    sfx.volume = 255;
    sfx.panning = 128;
    // play sound effect
    mmEffectEx(&sfx);
}

void sfx_play_interact_foe() {
    mm_sound_effect sfx;
    sfx.handle = 0;
    sfx.id = SFX_NV01;
    sfx.rate = (int)(1.0f * (1 << 10));
    sfx.volume = 255;
    sfx.panning = 128;
    // play sound effect
    mmEffectEx(&sfx);
}

void sfx_play_interact_ally() {
    mm_sound_effect sfx;
    sfx.handle = 0;
    sfx.id = SFX_DN02;
    sfx.rate = (int)(1.0f * (1 << 10));
    sfx.volume = 255;
    sfx.panning = 128;
    // play sound effect
    mmEffectEx(&sfx);
}

void sfx_play_move() {
    mm_sound_effect sfx;
    sfx.handle = 0;
    sfx.id = SFX_CB03;
    sfx.rate = (int)(1.0f * (1 << 10));
    sfx.volume = 255;
    sfx.panning = 128;
    // play sound effect
    mmEffectEx(&sfx);
}

void sfx_play_cant() {
    mm_sound_effect sfx;
    sfx.handle = 0;
    sfx.id = SFX_ZW01;
    sfx.rate = (int)(1.0f * (1 << 10));
    sfx.volume = 255;
    sfx.panning = 128;
    // play sound effect
    mmEffectEx(&sfx);
}

void sfx_play_death() {
    mm_sound_effect sfx;
    sfx.handle = 0;
    sfx.id = SFX_CB02;
    sfx.rate = (int)(1.0f * (1 << 10));
    sfx.volume = 255;
    sfx.panning = 128;
    // play sound effect
    mmEffectEx(&sfx);
}