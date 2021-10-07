#include "cold_data.h"

const ClassData cold_class_data[NUM_UNIT_CLASSES] = {
    (ClassData){.name = "soldier", .move = 4, .interact_range = 1},
    (ClassData){.name = "horse", .move = 6, .interact_range = 1},
    (ClassData){.name = "tank", .move = 3, .interact_range = 1},
    (ClassData){.name = "mage", .move = 4, .interact_range = 2},
};
