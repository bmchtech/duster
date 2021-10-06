#include "board_scn.h"

void draw_pause_ui() {
    if (pausemenu_dirty) {
        pausemenu_dirty = FALSE;

        // clear sprite oam
        memset32(oam_mem, 0, OAM_SIZE / 4);

        // clear tte and bg layers
        draw_clear_text_surface();
        draw_clear_ui_surface();
    }

}