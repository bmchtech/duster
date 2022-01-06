module typ.vpos;

import tonc.tonc_types;

struct VPos {
    u8 x;
    u8 y;
}

struct VPos16 {
    s16 x;
    s16 y;
}

VPos vpos_zero() {
    return VPos(0, 0);
}

VPos vpos_one() {
    return VPos(1, 1);
}

VPos vpos_add(VPos p1, VPos p2) {
    return VPos(p1.x + p2.x, p1.y + p2.y);
}

VPos16 vpos16_from_vpos(VPos p1) {
    return VPos16(p1.x, p1.y);
}

VPos vpos_from_vpos16(VPos16 p1) {
    return VPos(p1.x, p1.y);
}

VPos16 vpos16_zero() {
    return VPos16(0, 0);
}

VPos16 vpos16_one() {
    return VPos16(1, 1);
}

VPos16 vpos16_add(VPos16 p1, VPos16 p2) {
    return VPos16(p1.x + p2.x, p1.y + p2.y);
}

VPos16 vpos16_neg(VPos16 p1) {
    return VPos16(-p1.x, -p1.y);
}