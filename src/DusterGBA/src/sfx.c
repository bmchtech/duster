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