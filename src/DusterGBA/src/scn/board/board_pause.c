#include "board_scn.h"


#define NUM_PAUSE_SELECTIONS 3
VPos pause_cursor_pos;
int pause_cursor_selection = 0;

void update_pause_ui() {
    if (key_hit(KEY_DOWN) && pause_cursor_selection < (NUM_PAUSE_SELECTIONS - 1)) {
        pause_cursor_selection++;
        pausemenu_dirty = TRUE;
    }

    if (key_hit(KEY_UP) && pause_cursor_selection > 0) {
        pause_cursor_selection--;
        pausemenu_dirty = TRUE;
    }
}

int get_pause_cursor_y() {
    return 22 + pause_cursor_selection * 12 + 6;
}

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

    // for now just draw something
    schr4c_hline(&bg0_srf, 50, get_pause_cursor_y(), 55, 1);

    // todo
}