#include "board_scn.h"

#include "soundbank.h"
#include "maxmod.h"

void boardscn_sfx_play_startchime() {
    // define sfx
    mm_sound_effect chime;
    chime.handle = 0;
    chime.id = SFX_OBEP2;
    chime.rate = (int)(1.0f * (1 << 10));
    chime.volume = 255;
    chime.panning = 128;
    // play sound effect
    mmEffectEx(&chime);
}

void boardscn_sfx_play_scroll() {}

void boardscn_sfx_play_click() {}

void boardscn_sfx_play_interact() {
    mm_sound_effect sfx;
    sfx.handle = 0;
    sfx.id = SFX_ZW01;
    sfx.rate = (int)(1.0f * (1 << 10));
    sfx.volume = 255;
    sfx.panning = 128;
    // play sound effect
    mmEffectEx(&sfx);
}
void boardscn_sfx_play_move() {}
void boardscn_sfx_play_cant() {}