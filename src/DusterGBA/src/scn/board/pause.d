module scn.board.pause;

import core.stdc.string;
import core.stdc.stdio;
import dusk;
import tonc;
import libgba.maxmod;
import res;
import dusk.contrib.mgba;
import typ.vpos;
import scn.board;
import scn.logo;
import game;

// VPos pause_cursor_pos;
// int pause_cursor_selection = 0;

void pause_menu_back_selected() {
    // set scene back to board
    board_scene_page = BoardScenePage.BOARDSCN_BOARD;
}

void pause_menu_quit_selected() {
    // set scene back to board
    board_scene_page = BoardScenePage.BOARDSCN_BOARD;

    // restart game
    dusk_scene_set(logo_scene);
}

struct PauseMenuItem {
    char* display_name;
    void function() action;
}

enum PauseMenuItem[3] pause_menu_items = [
    PauseMenuItem(cast(char*) "back", &pause_menu_back_selected),
    PauseMenuItem(cast(char*) "save", &pause_menu_back_selected),
    PauseMenuItem(cast(char*) "quit", &pause_menu_quit_selected)
];

enum NUM_PAUSE_SELECTIONS = ((pause_menu_items.sizeof) / (PauseMenuItem.sizeof));
enum PAUSE_MENU_OFFSET_X = 16;
enum PAUSE_MENU_OFFSET_Y = 22;
enum PAUSE_MENU_INCREMENT_Y = 12;

void update_pause_ui() {
    int ud_touched = key_transit(KEY_UP | KEY_DOWN);
    int ud_input = key_tri_vert();
    if (ud_input != 0 && ud_touched) {
        // change selected item
        pause_cursor_selection = (pause_cursor_selection + ud_input + NUM_PAUSE_SELECTIONS) % NUM_PAUSE_SELECTIONS;
        pausemenu_dirty = TRUE;
        sfx_play_scroll();
    }

    if (key_hit(KEY_A)) {
        // an item is selected

        int sel_item = pause_cursor_selection;
        pause_cursor_selection = 0; // reset item selection index

        // call the action
        pause_menu_items[sel_item].action();
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
        tte_printf("#{P:%d,%d}#{ci:1}%s", PAUSE_MENU_OFFSET_X, PAUSE_MENU_OFFSET_Y + i * PAUSE_MENU_INCREMENT_Y,
            pause_menu_items[i].display_name);

    // box

    // for now just draw something
    int cursor_y = get_pause_cursor_y();
    schr4c_hline(&bg0_srf, 5, cursor_y, 8, 1);
    schr4c_hline(&bg0_srf, 5, cursor_y + 2, 10, 1);
    schr4c_hline(&bg0_srf, 5, cursor_y + 4, 8, 1);

    // todo
}
