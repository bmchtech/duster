module game.cold_data;

import game.defs;

enum ClassData[NUM_UNIT_CLASSES] cold_class_data = [
    // (ClassData){.name = "soldier", .move = 4, .interact_range = 1, .base_stats = {4, 4, 12, 3}, .stat_growths = {2, 1, 2, 2}},
    // (ClassData){.name = "horse", .move = 6, .interact_range = 1, .base_stats = {4, 2, 10, 6}, .stat_growths = {1, 1, 1, 4}},
    // (ClassData){.name = "tank", .move = 3, .interact_range = 1, .base_stats = {4, 8, 20, 1}, .stat_growths = {1, 2, 3, 1}},
    // (ClassData){.name = "mage", .move = 4, .interact_range = 2, .base_stats = {6, 2, 8, 2}, .stat_growths = {3, 1, 1, 2}},
    ClassData("soldier", 4, 1, UnitDataStats(4, 4, 12, 3), UnitDataStats(2, 1, 2, 2)),
    ClassData("horse", 6, 1, UnitDataStats(4, 2, 10, 6), UnitDataStats(1, 1, 1, 4)),
    ClassData("tank", 3, 1, UnitDataStats(4, 8, 20, 1), UnitDataStats(1, 2, 3, 1)),
    ClassData("mage", 4, 2, UnitDataStats(6, 2, 8, 2), UnitDataStats(3, 1, 1, 2)),
];
