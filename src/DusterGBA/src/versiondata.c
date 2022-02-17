/***
 * 
 * Version Data
 * 
 *****************************************************************************/

#define D_VERSION_ID "v0.4.2"

#define RES_GAME_VERSION ("$DUSTER " D_VERSION_ID)
#define RES_GAME_COPYING "$COPYING Copyright (C) 2018-2022 bean machine."

#ifndef GAME_BUILD_ID
#define GAME_BUILD_ID "unknown"
#endif
#define RES_GAME_BUILD ("$BUILD " GAME_BUILD_ID)

__attribute__((used)) const char* VERSION_ID = D_VERSION_ID;
__attribute__((used)) const char* GAME_VERSION = RES_GAME_VERSION;
__attribute__((used)) const char* GAME_BUILD = RES_GAME_BUILD;
__attribute__((used)) const char* GAME_COPYING = RES_GAME_COPYING;