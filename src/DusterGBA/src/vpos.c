#include "vpos.h"

VPos vpos_zero() { return (VPos){.x = 0, .y = 0}; }
VPos vpos_one() { return (VPos){.x = 1, .y = 1}; }
VPos vpos_add(VPos p1, VPos p2) { return (VPos){.x = p1.x + p2.x, .y = p1.y + p2.y}; }

VPos16 vpos16_from_vpos(VPos p1) { return (VPos16){.x = p1.x, .y = p1.y}; }

VPos16 vpos16_zero() { return (VPos16){.x = 0, .y = 0}; }
VPos16 vpos16_one() { return (VPos16){.x = 1, .y = 1}; }
VPos16 vpos16_add(VPos16 p1, VPos16 p2) { return (VPos16){.x = p1.x + p2.x, .y = p1.y + p2.y}; }
VPos16 vpos16_neg(VPos16 p1) { return (VPos16){.x = -p1.x, .y = -p1.y}; }