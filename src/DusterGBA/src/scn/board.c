#include <string.h>
#include "dusk.h"
#include "tonc.h"
#include "res.h"
#include "contrib/mgba.h"
#include "game/game.h"

TSurface bg0_srf;
int bg0_srf_cbb = 0;
int bg0_srf_sbb = 31;
VPos board_offset;
bool bg_ui_dirty = true;
int game_turn = 0;
VPos16 cursor_pos;
bool cursor_down = true;

void board_start() {
    // init
    dusk_init_graphics_mode0();
    dusk_sprites_init();
    dusk_sprites_configure(FALSE);

    // main bg
    REG_DISPCNT |= DCNT_BG0;
    REG_BG0CNT = BG_CBB(bg0_srf_cbb) | BG_SBB(bg0_srf_sbb) | BG_PRIO(0);

    // set up bg0 as a drawing surface
    srf_init(&bg0_srf, SRF_CHR4C, tile_mem[bg0_srf_cbb], 240, 160, 4, pal_bg_mem);
    schr4c_prep_map(&bg0_srf, se_mem[bg0_srf_sbb], 0); // set whole map to 0

    // set up bg1 with tte
    REG_DISPCNT |= DCNT_BG1;
    tte_init_chr4c(1, BG_CBB(2) | BG_SBB(29), 0, 0x0201, CLR_GRAY, NULL, NULL);
    REG_BG1CNT |= BG_PRIO(2);
    tte_init_con();

    // set bg palette
    pal_bg_mem[0] = RES_PAL[2]; // bg color
    pal_bg_mem[1] = RES_PAL[0]; // draw col 1
    pal_bg_mem[2] = RES_PAL[3]; // draw col 2

    mgba_printf(MGBA_LOG_INFO, "bean");

    // pawn spritesheet
    SpriteAtlas atlas = dusk_load_atlas("a_pawn");
    dusk_sprites_upload_atlas(&atlas);

    game_clear_state(); // reset game state

    // set up new game
    game_init_board(16);
    game_init_team(0, "player");

    Team* team = &game_state.teams[0];
    team_set_pawn(team, 0, 0); // first unit is soldier
    team_set_pawn(team, 1, 1); // second unit is horse

    board_set_pawn(BOARD_POS(0, 0), PAWN_GID(0, 0)); // pawn #0
    board_set_pawn(BOARD_POS(3, 2), PAWN_GID(0, 1)); // pawn #1

    // set vars for drawing
    board_offset = (VPos){.x = 8, .y = 8};
    cursor_pos = (VPos16){.x = 0, .y = 0};

    tte_printf("#{P:8,140}#{ci:1}turn: %s", game_state.teams[game_turn].name);
}

void draw_board_outline() {
    // draw the board outline
    int x1 = (board_offset.x);
    int x2 = (board_offset.x) + (game_state.board_size * 8);
    int y1 = (board_offset.y);
    int y2 = (board_offset.y) + (game_state.board_size * 8);

    schr4c_hline(&bg0_srf, x1 - 1, y1 - 1, x2 + 1, 1);
    schr4c_vline(&bg0_srf, x1 - 1, y1 - 1, y2 + 1, 1);
    schr4c_hline(&bg0_srf, x1 - 1, y2 + 1, x2 + 1, 1);
    schr4c_vline(&bg0_srf, x2 + 1, y1 - 1, y2 + 1, 1);
}

void draw_board_cursor() {
    int x1 = (board_offset.x) + (cursor_pos.x * 8);
    int x2 = x1 + 7;
    int y1 = (board_offset.y) + (cursor_pos.y * 8);
    int y2 = y1 + 7;

    schr4c_hline(&bg0_srf, x1, y1, x2, 2);
    schr4c_vline(&bg0_srf, x1, y1, y2, 2);
    schr4c_hline(&bg0_srf, x1, y2, x2, 2);
    schr4c_vline(&bg0_srf, x2, y1, y2, 2);
}

void draw_board() {
    if (bg_ui_dirty) {
        // clear whole bg ui surface
        memset32(tile_mem[bg0_srf_cbb], 0, 4096);          // clear cbb
        schr4c_prep_map(&bg0_srf, se_mem[bg0_srf_sbb], 0); // set whole map to 0

        // draw outline
        draw_board_outline();

        // draw cursor
        if (cursor_down)
            draw_board_cursor();

        // no longer dirty
        bg_ui_dirty = false;
    }

    // start assigning sprites from sprite M, and every time a new pawn is found increment the counter
    int pawn_sprite_ix = 1;

    // draw the pawns
    for (int by = 0; by < game_state.board_size; by++) {
        for (int bx = 0; bx < game_state.board_size; bx++) {
            BoardTile* tile = &game_state.board.tiles[BOARD_POS(bx, by)];
            if (tile->pawn_gid >= 0) {
                // this is a pawn

                // look up the pawn
                Pawn* pawn = game_get_pawn_by_gid(tile->pawn_gid);

                // assign a sprite to drawing this pawn
                dusk_sprites_make(pawn_sprite_ix++, 8, 8,
                                  (Sprite){
                                      .x = board_offset.x + (bx << 3),
                                      .y = board_offset.y + (by << 3),
                                      .base_tid = pawn->unit_class,
                                  });
            }
        }
    }

    // // for each team
    // for (int i = 0; i < NUM_TEAMS; i++) {
    //     Team* team = &game_state.teams[i];
    //     // draw pawns
    //     for (int j = 0; j < TEAM_MAX_PAWNS; j++) {
    //         Pawn* pawn = &team->pawns[j];
    //     }
    // }
}

void board_update() {
    dusk_frame();

    // do layout
    draw_board();

    // input

    // update sprites
    dusk_sprites_update();
}

void board_end() {}

Scene board_scene = {
    .start = board_start,
    .update = board_update,
    .end = board_end,
};