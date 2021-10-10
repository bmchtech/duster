#include "cold_data.h"

const ClassData cold_class_data[NUM_UNIT_CLASSES] = {
    (ClassData){.name = "soldier", .move = 4, .interact_range = 1, .base_stats = {4, 4, 12}},
    (ClassData){.name = "horse", .move = 6, .interact_range = 1, .base_stats = {4, 2, 10}},
    (ClassData){.name = "tank", .move = 3, .interact_range = 1, .base_stats = {4, 8, 20}},
    (ClassData){.name = "mage", .move = 4, .interact_range = 2, .base_stats = {6, 2, 8}},
};
