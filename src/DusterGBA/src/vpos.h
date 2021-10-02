#pragma once

#include <tonc_types.h>

typedef struct VPos {
    u8 x;
    u8 y;
} VPos;

typedef struct {
    s16 x;
    s16 y;
} VPos16;

VPos vpos_zero();
VPos vpos_one();
VPos vpos_add(VPos p1, VPos p2);

VPos16 vpos16_from_vpos(VPos p1);
VPos vpos_from_vpos16(VPos16 p1);

VPos16 vpos16_zero();
VPos16 vpos16_one();
VPos16 vpos16_add(VPos16 p1, VPos16 p2);
VPos16 vpos16_neg(VPos16 p1);

// VPos vpos_multi(VPos p1, int s);
// VPos vpos_multf(VPos p1, int s);
// VPos vpos_lerp(VPos p1, VPos p2);