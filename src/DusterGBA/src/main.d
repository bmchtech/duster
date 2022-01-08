module main;

import core.stdc.stdio;
import tonc;
import dusk;
import dusk.contrib.mgba;
import dusk.contrib.gbfs;
import libgba.maxmod;
import scn.logo;
import res;

extern (C):

void maxmod_audio_init() {
    // irq setup for maxmod dma
    irq_init(null);
    irq_add(IrqIndex.VBLANK, &mmVBlank);
    irq_enable(IrqIndex.VBLANK);

    // load soundbank
    u32 soundbank_len;
    const u32* soundbank_bin = cast(const u32*) gbfs_get_obj(gbfs_dat, "soundbank.bin", &soundbank_len);

    // initialise maxmod with soundbank and 8 channels
    mmInitDefault(cast(mm_addr) soundbank_bin, 8);
}

int main() {
    dusk_init_all();

    maxmod_audio_init();

    // set waitstates to be as fast as possible
    *REG_WAITCNT = WS_SRAM_2 | WS_ROM0_N2 | WS_ROM1_N2 | WS_ROM2_N2 | WS_PHI_OFF | WS_PREFETCH | WS_GBA;

    version (DEBUG) {
        mgba_open();
    }

    // log rom info
    mgba_printf(MGBALogLevel.INFO, "build info: %s %s (%s)\n", GAME_VERSION, GAME_BUILD, GAME_COPYING);

    dusk_scene_set(logo_scene);

    while (true) {
        key_poll(); // update inputF

        dusk_scene_update();
        mmFrame();
    }
}
