#include <graphx.h>

#include "effects.h"
#include "gfx/gfx.h"

#define base_palette ((uint16_t*)global_palette)
#define start 0
#define length (sizeof_global_palette / 2)
#define stepLength 2

void fadeout()
{
    for (int step = 255; step >= 0; step -= stepLength)
    {
        for (int i = start; i < start + length; i++)
            gfx_palette[i] = gfx_Darken(base_palette[i], step);
    }
}

void fadein()
{
    for (int step = 0; step < 256; step += stepLength)
    {
        for (int i = start; i < start + length; i++)
            gfx_palette[i] = gfx_Darken(base_palette[i], step);
    }
}