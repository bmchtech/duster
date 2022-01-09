module res;

import tonc.tonc_types;

extern (C):

__gshared extern const char* VERSION_ID;

__gshared extern const char* GAME_VERSION;
__gshared extern const char* GAME_BUILD;
__gshared extern const char* GAME_COPYING;

enum u16[5] RES_PAL = [0x2D09, 0x5AAE, 0x6B9D, 0x4B3E, 0x39BB];

// globals
__gshared char[32] selected_map_file;