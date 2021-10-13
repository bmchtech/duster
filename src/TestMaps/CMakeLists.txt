cmake_minimum_required(VERSION 3.0)
project(raylib_template C)

set(CMAKE_C_STANDARD 99)

# Setting parameters for raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # or games

add_subdirectory(libs/raylib)

add_executable(raylib_template
    # main
    src/main.c src/util.c
    # compat
    gbastub/tonc_compat.c gbastub/dusk_compat.c
    # duster
    ../DusterGBA/src/game/cold_data.c ../DusterGBA/src/game/game_funcs.c ../DusterGBA/src/game/game_util.c ../DusterGBA/src/game/game_logic.c ../DusterGBA/src/game/game_mapload.c
    # ccoll
    ../../lib/ccoll/src/cc_array.c ../../lib/ccoll/src/cc_common.c ../../lib/ccoll/src/cc_deque.c ../../lib/ccoll/src/cc_hashset.c ../../lib/ccoll/src/cc_hashtable.c ../../lib/ccoll/src/cc_list.c ../../lib/ccoll/src/cc_pqueue.c ../../lib/ccoll/src/cc_queue.c ../../lib/ccoll/src/cc_ring_buffer.c ../../lib/ccoll/src/cc_slist.c ../../lib/ccoll/src/cc_stack.c ../../lib/ccoll/src/cc_treeset.c ../../lib/ccoll/src/cc_treetable.c ../../lib/ccoll/src/cc_tsttable.c ../../lib/ccoll/src/string.c
    )
target_link_libraries(raylib_template PRIVATE raylib)
target_compile_definitions(raylib_template PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/") # Set the asset path macro to the absolute path on the dev machine
#target_compile_definitions(raylib_template PUBLIC ASSETS_PATH="relative-path-to-assets-in-the-game-package") # Set the asset path macro in release more
target_include_directories(raylib_template PRIVATE ../DusterGBA/src gbastub ../../lib/dusk/src/dusk/include ../../lib/ccoll/include)