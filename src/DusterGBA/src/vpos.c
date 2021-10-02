#include "vpos.h"

VPos vpos_zero() { return (VPos){.x = 0, .y = 0}; }
VPos vpos_one() { return (VPos){.x = 1, .y = 1}; }
VPos vpos_add(VPos p1, VPos p2) { return (VPos){.x = p1.x + p2.x, .y = p1.y + p2.y}; }