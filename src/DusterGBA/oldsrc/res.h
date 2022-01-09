#pragma once

#include <tonc_types.h>

extern u16 RES_PAL[5];

#define RES_GAME_VERSION "$DUSTER v0.3.0-aicomp"
#define RES_GAME_COPYING "$COPYING Copyright (C) 2018-2021 bean machine."
#ifndef GAME_BUILD_ID
#define GAME_BUILD_ID "unknown"
#endif
#define RES_GAME_BUILD ("$BUILD " GAME_BUILD_ID)

// globals
extern char selected_map_file[32];