module ai.utils;

import game;
import libtind.ds.dict;
import libtind.ds.vector;
import libtind.util;
import ai;

extern (C):

// old lua implementation
/*
local range_data_max_size <comptime> = 128
local Relocs = @record{
    map: hashmap(VPos16, cshort)
}
local PawnMoveCache = @record{
    relocs: *Relocs,
    range_data: *[range_data_max_size]VPos16,
    range_data_count: cint
}

-- make a list of all available pawns on this team
local function ai_scan_pawns(team_id: cint): vector(cshort)
    local my_pawns: vector(cshort) = {}

    -- scan through all pawn slots
    -- add all that are alive to a list
    for pawn_ix = 0, TEAM_MAX_PAWNS - 1 do
        local pawn_gid = LPAWN_GID(team_id, pawn_ix)
        local pawn: *Pawn = game_get_pawn_by_gid(pawn_gid)

        if pawn.alive > 0 then
            my_pawns:push(pawn_gid)
        end
    end

    return my_pawns
end

local function ai_get_range_data(pos: VPos16, move: cint)
    local range_data: *[range_data_max_size]VPos16 = default_allocator:new(@[range_data_max_size]VPos16)
    local range_data_count = board_util_calc_rangebuf(pos.x, pos.y, move, range_data, range_data_max_size)

    return range_data, range_data_count
end

local function ai_destroy_range_data(range_data: *[range_data_max_size]VPos16)
    default_allocator:delete(range_data)
end

local function ai_can_pawn_move_to(pawn_gid: cshort, from_pos: VPos16, pos: VPos16, move_cache: PawnMoveCache): boolean
    local tile_check = false
    -- check tile, empty and walkable
    if (board_util_is_on_board(pos.x, pos.y) > 0 and board_util_is_walkable(pos.x, pos.y) > 0) then
        -- valid tile, check if occupied
        local is_occupied_before = board_get_pawn(LBOARD_POS(pos.x, pos.y)) ~= nilptr
        local is_reloc = move_cache.relocs.map:peek(pos) ~= nilptr

        -- double check using relocs to see if pawn is already planned to move there
        if is_reloc then
            local reloc_pawn_gid: cshort = move_cache.relocs.map[pos]
            -- mgba_printf(1, "pawn %d is already planned to move to %d,%d", reloc_pawn_gid, pos.x, pos.y)
            if ((not is_occupied_before) and reloc_pawn_gid >= 0) then
                -- someone else is already there
                tile_check = false
            end
            if is_occupied_before and reloc_pawn_gid < 0 then
                -- actually it is no longer occupied
                tile_check = true
            end
        elseif not is_occupied_before then
            -- no reloc, so it is free
            tile_check = true
        end

        -- tile_check = not is_occupied_before
    end

    local range_check = false
    -- check range    
    for i = 0, move_cache.range_data_count - 1 do
        local range_pos = ($move_cache.range_data)[i]
        if (range_pos.x == pos.x and range_pos.y == pos.y) then
            range_check = true
            break
        end
    end

    return range_check and tile_check
end

local ProximityType = @enum{
    NEAR = 0,
    FAR
}

local function ai_get_nearest_enemy_pawn(pos: VPos16, enemy_pawns: *vector(cshort))
    local closest_pawn = enemy_pawns[0]
    local closest_dist = 99999

    for i, pawn in ipairs(enemy_pawns) do
        local pawn_tile = board_find_pawn_tile(pawn)
        local pawn_pos  = board_util_tid_to_pos(pawn_tile)
        local pawn_dist = board_dist(pos.x, pos.y, pawn_pos.x, pawn_pos.y)

        if (pawn_dist < closest_dist) then
            closest_dist = pawn_dist
            closest_pawn = pawn
        end
    end

    return closest_pawn
end

    -- get the closest tile in range to the destination
local function ai_get_closest_tile_in_range(pawn_gid: cshort, from_pos: VPos16, to_pos: VPos16, move_cache: PawnMoveCache, proximity_type: ProximityType): VPos16
    local class_data = pawn_get_classdata(pawn_gid)

    -- find the closest tile in range
    local target_tile = from_pos

    switch proximity_type
    case ProximityType.NEAR then
        local target_dist = 999999
        for i = 0, move_cache.range_data_count - 1 do
            local range_pos = ($move_cache.range_data)[i]
            local dist = board_dist(range_pos.x, range_pos.y, to_pos.x, to_pos.y)

            -- ensure that we are able to move there
            if not ai_can_pawn_move_to(pawn_gid, from_pos, range_pos, move_cache) then
                continue
            end

            if dist < target_dist then
                target_tile = range_pos
                target_dist = dist
            end
        end
    case ProximityType.FAR then
        local target_dist = 0
        for i = 0, move_cache.range_data_count - 1 do
            local range_pos = ($move_cache.range_data)[i]
            local dist = board_dist(range_pos.x, range_pos.y, to_pos.x, to_pos.y)

            -- ensure that we are able to move there
            if not ai_can_pawn_move_to(pawn_gid, from_pos, range_pos, move_cache) then
                continue
            end

            if dist > target_dist then
                target_tile = range_pos
                target_dist = dist
            end
        end
    else
    end

    return target_tile
end
*/

// we want to port that lua code to D

struct Relocs {
    Dict!(VPos16, pawn_gid_t) map;
}

struct PawnMoveCache {
    Relocs relocs;
    Vector!VPos16 range_data;
}

/** make a list of all available pawns on this team */
Vector!pawn_gid_t ai_scan_pawns(int team_id) {
    Vector!pawn_gid_t my_pawns;

    for (int pawn_ix = 0; pawn_ix < TEAM_MAX_PAWNS; pawn_ix++) {
        pawn_gid_t pawn_gid = PAWN_GID(team_id, pawn_ix);
        Pawn* pawn = game_get_pawn_by_gid(pawn_gid);

        if (pawn.alive) {
            my_pawns.push_back(pawn_gid);
        }
    }

    return my_pawns;
}

