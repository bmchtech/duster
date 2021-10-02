#include "board_scn.h"

void foreach_pawn2sprite_key(const void* key) {
    // try getting the value and printing
    int* val_out;
    cc_hashtable_get(pawn2sprite, (void*)key, (void*)&val_out);
    int* key_out = (int*)key;

    mgba_printf(MGBA_LOG_ERROR, "pawn2sprite k: %d, v: %d", *key_out, *val_out);
}

void update_pawn_tween() {
    // log mappings

    // CC_Array* pawn2sprite_keys;
    // cc_hashtable_get_keys(pawn2sprite, &pawn2sprite_keys);
    // cc_array_destroy(pawn2sprite_keys);
    // mgba_printf(MGBA_LOG_ERROR, "pawn2sprite size: %d", cc_array_size(pawn2sprite_keys));

    cc_hashtable_foreach_key(pawn2sprite, foreach_pawn2sprite_key);

    // try getting
    int* get_test_out;
    int get_test_key = 32;
    enum cc_stat get_test_stat = cc_hashtable_get(pawn2sprite, &get_test_key, (void*) &get_test_out);
    if (get_test_stat == CC_OK) {
        mgba_printf(MGBA_LOG_ERROR, "pawn2sprite get test: %d = %d", get_test_key, *get_test_out);
    } else {
        mgba_printf(MGBA_LOG_ERROR, "pawn2sprite get test fail: key: %d", get_test_key);
    }

    if (pawn_tween.pawn_gid < 0)
        return;

    // // now we tween
    // int pawn_sprite = cc_hashtable_get()
}