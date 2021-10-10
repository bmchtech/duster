#include "dusk.h"
#include "ds_sys.h"
#include "scenes.h"
#include "contrib/mgba.h"
#include "maxmod.h"

void maxmod_audio_init() {
    // irq setup for maxmod dma
    irq_init(NULL);
    irq_add(II_VBLANK, mmVBlank);
    irq_enable(II_VBLANK);

    // load soundbank
    u32 soundbank_len;
    const u32* soundbank_bin = gbfs_get_obj(gbfs_dat, "soundbank.bin", &soundbank_len);

    // initialise maxmod with soundbank and 8 channels
    mmInitDefault((mm_addr)soundbank_bin, 8);
}

int main() {
    dusk_init_all();

    maxmod_audio_init();

    // set waitstates to be as fast as possible
    REG_WAITCNT = 0b0100011011011010;

#ifdef DEBUG
    mgba_open();
#endif

    dusk_scene_set(logo_scene);

    while (TRUE) {
        key_poll(); // update inputF

        mgba_printf(MGBA_LOG_ERROR, "%d", REG_VCOUNT);
        dusk_scene_update();
        mmFrame();
    }
}