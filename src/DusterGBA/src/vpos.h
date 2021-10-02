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
// VPos vpos_neg(VPos p1);
// VPos vpos_multi(VPos p1, int s);
// VPos vpos_multf(VPos p1, int s);
// VPos vpos_lerp(VPos p1, VPos p2);