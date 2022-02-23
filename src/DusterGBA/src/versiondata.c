/***
 * 
 * Version Data
 * 
 *****************************************************************************/

#define RES_GAME_VERSION ("$DUSTER " VERSION_ID_DEF)
#define RES_GAME_COPYING "$COPYING Copyright (C) 2018-2022 bean machine."

#ifndef GAME_BUILD_ID_DEF
#define GAME_BUILD_ID_DEF "unknown"
#endif
#define RES_GAME_BUILD ("$BUILD " GAME_BUILD_ID_DEF)

__attribute__((used)) const char* VERSION_ID = VERSION_ID_DEF;
__attribute__((used)) const char* BUILD_ID = GAME_BUILD_ID_DEF;

__attribute__((used)) const char* GAME_VERSION = RES_GAME_VERSION;
__attribute__((used)) const char* GAME_BUILD = RES_GAME_BUILD;
__attribute__((used)) const char* GAME_COPYING = RES_GAME_COPYING;