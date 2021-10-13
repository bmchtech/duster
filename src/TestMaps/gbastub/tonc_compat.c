#include <tonc_types.h>
#include <stdio.h>
#include <memory.h>

void memset32(void *dst, u32 wd, uint wcount)
{
    memset(dst, wd, wcount * 4);
}