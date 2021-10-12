#include "board_scn.h"
#include "scenes.h"

VPos pause_cursor_pos;
int pause_cursor_selection = 0;

void pause_menu_back_selected() { board_scene_page = BOARDSCN_BOARD; }

void pause_menu_quit_selected() { dusk_scene_set(logo_scene); }

typedef struct PauseMenuItem {
    char* display_name;
    void (*action)();
} PauseMenuItem_t;

PauseMenuItem_t pause_menu_items[3] = {
    {"back", pause_menu_back_selected},
    {"save", pause_menu_back_selected},
    {"quit", pause_menu_quit_selected}
};

#define NUM_PAUSE_SELECTIONS   (sizeof(pause_menu_items) / sizeof(PauseMenuItem_t))
#define PAUSE_MENU_OFFSET_X    16
#define PAUSE_MENU_OFFSET_Y    22
#define PAUSE_MENU_INCREMENT_Y 12

void update_pause_ui() {
    if (key_hit(KEY_DOWN) && pause_cursor_selection < (NUM_PAUSE_SELECTIONS - 1)) {
        pause_cursor_selection++;
        pausemenu_dirty = TRUE;
    }

    if (key_hit(KEY_UP) && pause_cursor_selection > 0) {
        pause_cursor_selection--;
        pausemenu_dirty = TRUE;
    }

    if (key_hit(KEY_A)) {        
        pause_menu_items[pause_cursor_selection].action();
    }
}

int get_pause_cursor_y() {
    return PAUSE_MENU_OFFSET_Y + pause_cursor_selection * PAUSE_MENU_INCREMENT_Y + PAUSE_MENU_INCREMENT_Y / 2 - 1;
}

void draw_pause_ui() {
    if (!pausemenu_dirty)
        return;
    pausemenu_dirty = FALSE;

    // clear sprite oam
    memset32(oam_mem, 0, OAM_SIZE / 4);

    // hide all sprites from M to NUM_SPRITES
    for (int i = 0; i < NUM_SPRITES; i++) {
        obj_hide(&obj_mem[i]);
    }

    // clear tte and bg layers
    draw_clear_text_surface();
    draw_clear_ui_surface();

    // bottom text
    tte_printf("#{P:200,140}#{ci:1}paused");

    // menu
    for (int i = 0; i < NUM_PAUSE_SELECTIONS; i++)
        tte_printf("#{P:%d,%d}#{ci:1}%s", 
                   PAUSE_MENU_OFFSET_X, 
                   PAUSE_MENU_OFFSET_Y + i * PAUSE_MENU_INCREMENT_Y, 
                   pause_menu_items[i].display_name);

    // box

    // for now just draw something
    int cursor_y = get_pause_cursor_y();
    schr4c_hline(&bg0_srf, 5, cursor_y,     8,  1);
    schr4c_hline(&bg0_srf, 5, cursor_y + 2, 10, 1);
    schr4c_hline(&bg0_srf, 5, cursor_y + 4, 8,  1);

    // todo
}