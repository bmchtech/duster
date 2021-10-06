#include "board_scn.h"

void draw_pause_ui() {
    if (!pausemenu_dirty)
        return;
    pausemenu_dirty = FALSE;

    // clear sprite oam
    memset32(oam_mem, 0, OAM_SIZE / 4);

    // clear tte and bg layers
    draw_clear_text_surface();
    draw_clear_ui_surface();

    // bottom text
    tte_printf("#{P:200,140}#{ci:1}paused");

    // menu
    tte_printf("#{P:16,22}#{ci:1}back");
    tte_printf("#{P:16,34}#{ci:1}save");
    tte_printf("#{P:16,46}#{ci:1}quit");

    // box
    // todo
}