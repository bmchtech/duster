#include "game.h"
#include <stdio.h>
#include <memory.h>
#include <tonc.h>
#include "contrib/mgba.h"

void game_logic_step() {
    game_state.steps++; // counter

    mgba_printf(MGBA_LOG_ERROR, "logic step (%d)", game_state.steps);
}

void game_logic_kill_if_dead(pawn_gid_t pawn_gid) {
    Pawn* pawn = game_get_pawn_by_gid(pawn_gid);

    if (pawn->unit_data.hitpoints <= 0) {
        // dead
        int pawn_tile = board_find_pawn_tile(pawn_gid);
        board_set_pawn(pawn_tile, -1);
        mgba_printf(MGBA_LOG_ERROR, "pawn died: %d", pawn_gid);
    }
}

typedef struct {
    int main_dmg;
    int counter_dmg;
} HostileUnitDuel;

HostileUnitDuel game_logic_calc_hostile_damage(Pawn* initiator_pawn, Pawn* receiver_pawn) {
    UnitDataStats* i_stats = &initiator_pawn->unit_data.stats;
    UnitDataStats* r_stats = &receiver_pawn->unit_data.stats;

    // get attack strength
    int atk_strength = i_stats->atk;

    // get defense strength
    int def_strength = r_stats->def;

    HostileUnitDuel ret;

    // mitigate attack
    int mitigated_atk = atk_strength - def_strength;
    clamp(mitigated_atk, 0, atk_strength);

    ret.main_dmg = mitigated_atk;
    ret.counter_dmg = 0;

    return ret;
}

void game_logic_interact(pawn_gid_t initiator, pawn_gid_t receiver) {
    int initiator_tid = board_find_pawn_tile(initiator);
    int receiver_tid = board_find_pawn_tile(receiver);

    int initiator_team = PAWN_WHICH_TEAM(initiator);
    int receiver_team = PAWN_WHICH_TEAM(receiver);

    Pawn* initiator_pawn = game_get_pawn_by_gid(initiator);
    Pawn* receiver_pawn = game_get_pawn_by_gid(receiver);

    // calculate vars/parameters
    BOOL same_team = initiator_team == receiver_team;

    if (same_team) {
        // friendly
    } else {
        // hostile

        // do damage calculation
        HostileUnitDuel duel = game_logic_calc_hostile_damage(initiator_pawn, receiver_pawn);

        // exchange damage
        mgba_printf(MGBA_LOG_ERROR, "damage exchanged: %d %d", duel.main_dmg, duel.counter_dmg);
        receiver_pawn->unit_data.hitpoints -= duel.main_dmg;
        initiator_pawn->unit_data.hitpoints -= duel.counter_dmg;

        // if hp empty, kill
        game_logic_kill_if_dead(receiver);
    }
}