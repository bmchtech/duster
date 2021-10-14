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