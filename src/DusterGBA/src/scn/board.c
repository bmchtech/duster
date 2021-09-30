#include <string.h>
#include "dusk.h"
#include "tonc.h"
#include "res.h"
#include "contrib/mgba.h"
#include "game/game.h"

GameBoard* board;

void board_start() {
    // init
    dusk_init_graphics_mode0();
    dusk_sprites_init();
    dusk_sprites_configure(FALSE);

    // main bg
    REG_DISPCNT |= DCNT_BG0;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(31);

    pal_bg_mem[0] = RES_PAL[0];
    pal_bg_mem[1] = RES_PAL[2];

    // try drawing
    TSurface srf;
    srf_init(&srf, SRF_CHR4C, tile_mem[0], 240, 160, 4, pal_bg_mem);
    schr4c_prep_map(&srf, se_mem[31], 0);

    // schr4r_line(&srf, 20, 20, 30, 30, 1);

    // // clear CBB 0
    // memset32(tile_mem[0], 0x00000000, 4096);
    // schr4c_prep_map(&srf, se_mem[31], 0);

    mgba_printf(MGBA_LOG_INFO, "bean");

    // pawn spritesheet
    SpriteAtlas atlas = dusk_load_atlas("a_pawn");
    dusk_sprites_upload_atlas(&atlas);

    // reset game state
    memset32(&game_state, 0, sizeof(GameState) / 4);

    // set up new game
    game_state.board_size = 4;
    board = &game_state.board;
    // set whole board to -1 (no pawn)
    memset32(board, -1, sizeof(GameBoard) / 4);
    Team* team0 = &game_state.teams[0];
    sprintf(team0->name, "%s", "BEAN");
    team0->pawns[0] = (Pawn){.unit_class = 1};
    board->tiles[BOARD_POS(0, 0)].pawn_index = 0; // point to pawn #0
    board->tiles[BOARD_POS(3, 1)].pawn_index = 0; // point to pawn #0
}

void update_board_layout() {
    // start assigning sprites from sprite M, and every time a new pawn is found increment the counter
    int pawn_sprite_ix = 1;

    // first, we want to draw the pawns
    for (int by = 0; by < game_state.board_size; by++) {
        for (int bx = 0; bx < game_state.board_size; bx++) {
            BoardTile* tile = &board->tiles[BOARD_POS(bx, by)];
            if (tile->pawn_index >= 0) {
                // this is a pawn

                // assign a sprite to drawing this pawn
                dusk_sprites_make(pawn_sprite_ix++, 8, 8,
                                  (Sprite){
                                      .x = bx << 3,
                                      .y = by << 3,
                                      .base_tid = 0,
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
    update_board_layout();

    // update sprites
    dusk_sprites_update();
}

void board_end() {}

Scene board_scene = {
    .start = board_start,
    .update = board_update,
    .end = board_end,
};