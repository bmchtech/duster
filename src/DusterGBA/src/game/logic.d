module game.logic;

import ldc.attributes;

import dusk.contrib.mgba;
import tonc;
import game;

extern (C):

void game_logic_step() {
    game_state.steps++; // counter

    mgba_printf(MGBALogLevel.INFO, "logic step (%d)", game_state.steps);

    int turn_team = game_util_whose_turn();
    // check if all pawns on the current team have moved
    bool all_moved = true;
    for (int i = 0; i < TEAM_MAX_PAWNS; i++) {
        Pawn* pawn = game_get_pawn_by_gid(PAWN_GID(turn_team, i));
        if (pawn.alive) {
            if (!pawn_util_moved_this_turn(pawn)) {
                all_moved = false;
            }
        }
    }

    if (all_moved) {
        // all have moved, advance turn to next alive team
        int next_turn = game_state.turns + 1;
        for (int i = 0; i < NUM_TEAMS; i++) {
            int check_team = next_turn + i;
            if (!game_state.teams[check_team % NUM_TEAMS].alive) {
                // this team isn't alive, so we skip
                continue;
            }
            // we are all good
            next_turn = check_team;
            break;
        }
        game_state.turns = next_turn;
    }
}

void game_logic_kill_if_dead(pawn_gid_t pawn_gid) {
    Pawn* pawn = game_get_pawn_by_gid(pawn_gid);

    if (pawn.unit_data.hitpoints <= 0) {
        // dead
        pawn.alive = false;
        int pawn_tile = board_find_pawn_tile(pawn_gid);
        board_set_pawn(pawn_tile, -1);
        mgba_printf(MGBALogLevel.ERROR, "pawn died: %d", pawn_gid);

        import scn.board.defs : sfx_play_death;

        sfx_play_death();
    }
}

HostileUnitDuel game_logic_calc_hostile_damage(Pawn* initiator_pawn, Pawn* receiver_pawn) {
    UnitDataStats* i_stats = &initiator_pawn.unit_data.stats;
    UnitDataStats* r_stats = &receiver_pawn.unit_data.stats;

    mgba_printf(MGBALogLevel.INFO, "duel between (stats %d %d %d %d) and (stats %d %d %d %d)", // initiator_pawn.gid,
        i_stats.atk, i_stats.def, i_stats.hp, i_stats.spd, // receiver_pawn.gid,
        r_stats.atk, r_stats.def, r_stats.hp, r_stats.spd
    );

    // get attack strength
    int atk_strength = i_stats.atk;
    int atk_speed = i_stats.spd;

    // get defense strength
    int def_strength = r_stats.def;
    int def_speed = r_stats.spd;

    HostileUnitDuel ret;

    // calculate base damage
    // this damage is reduced by opponent def stat
    int atk_advantage = atk_strength - def_strength;
    // if the attacker is the same or lower, then the base damage is 0
    auto atk_dmg_base = clamp(atk_advantage, 0, atk_strength);
    // the denominator of the rng chances
    auto atk_pool = 5;
    // if def is more than 3 levels higher, then pool is doubled
    if (def_strength - atk_strength >= 3) {
        atk_pool *= 2;
    }
    auto spd_advantage = atk_speed - def_speed;
    if (spd_advantage <= -2) {
        // if the attacker is moderately slower, then the base damage is halved
        atk_dmg_base /= 2;
    }
    if (spd_advantage >= 4) {
        // if the attacker is greatly faster, then the base damage is greatly increased
        atk_dmg_base = (atk_dmg_base + 1) * 2;
    }

    // calculate 5 different attack landings
    auto atk_miss = 0;
    auto atk_glancing = (2 * atk_dmg_base) / 3;
    auto atk_hit = atk_dmg_base;
    auto atk_strong = (4 * atk_dmg_base) / 3;
    auto atk_crit = (3 * atk_dmg_base) / 2;

    // since base pool is 5, base distr is 50
    auto distr_pool = atk_pool * 10;

    // calculate rng value
    auto rng1 = qran_range(0, distr_pool + 1);

    /*
        chances:
            miss: 2/10 -- cdf = 2/10
            glancing: 2/10 -- cdf = 4/10
            hit: 3/10 -- cdf = 7/10
            strong: 2/10 -- cdf = 9/10
            crit: 1/10 -- cdf = 10/10
    */

    // get attack type
    auto calc_dmg = 1; // 1 free point

    if (rng1 < 10) {
        // critical
        calc_dmg = atk_crit;
    } else if (rng1 < 30) {
        // strong
        calc_dmg = atk_strong;
    } else if (rng1 < 60) {
        // hit
        calc_dmg = atk_hit;
    } else if (rng1 < 80) {
        // glancing
        calc_dmg = atk_glancing;
    } else {
        // miss
        calc_dmg = atk_miss;
    }

    // clamp attack damage
    if (calc_dmg < 0)
        calc_dmg = 0;

    // now, calculate counter
    // counter is the damage the defender returns
    auto counter_ceiling = (calc_dmg + r_stats.atk + def_strength) / 2;
    auto counter_penalty = 0;
    auto counter_roll = qran_range(0, counter_ceiling);
    auto speed_counter_penalty = clamp(spd_advantage, 0, 4);
    counter_penalty = (atk_strength + speed_counter_penalty) / 2;

    auto counter_dmg = counter_roll - counter_penalty;

    if (atk_advantage < 0) {
        // defender has an advantage, so counter damage is increased
        counter_dmg = (4 * counter_dmg) / 3;
    }

    if (counter_dmg < 0)
        counter_dmg = 0;

    // log all damage calculation variables
    mgba_printf(MGBALogLevel.INFO, "atk calc: miss %d, glancing %d, hit %d, strong %d, crit %d",
        atk_miss, atk_glancing, atk_hit, atk_strong, atk_crit
    );
    mgba_printf(MGBALogLevel.INFO, "atk dmg: atkadv %d, spdadv %d, base %d, distr %d, roll %d",
        atk_advantage, spd_advantage, atk_dmg_base, distr_pool, rng1
    );
    mgba_printf(MGBALogLevel.INFO, "ctr dmg: ceil %d, roll %d, penalty %d, dmg %d",
        counter_ceiling, counter_roll, counter_penalty, counter_dmg
    );

    ret.main_dmg = calc_dmg;
    ret.counter_dmg = counter_dmg;

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
    bool same_team = initiator_team == receiver_team;

    if (same_team) {
        // friendly
    } else {
        // hostile

        // do damage calculation
        HostileUnitDuel duel = game_logic_calc_hostile_damage(initiator_pawn, receiver_pawn);

        // exchange damage
        mgba_printf(MGBALogLevel.ERROR, "damage exchanged: atk: %d, ctr: %d", duel.main_dmg, duel
                .counter_dmg);
        receiver_pawn.unit_data.hitpoints -= duel.main_dmg;
        initiator_pawn.unit_data.hitpoints -= duel.counter_dmg;

        // if hp empty, kill
        game_logic_kill_if_dead(receiver);
        game_logic_kill_if_dead(initiator);
    }
}
