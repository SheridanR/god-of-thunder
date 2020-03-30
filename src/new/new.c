// THOR - The God of Thunder
// See LICENSE for details

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <Windows.h>

#include "../define.h"
#include "../proto.h"
#include "../res_man.h"
#include "new.h"

//===========================================================================
void* movedata(void* src0, void* src1, void* dest0, void* dest1, size_t bytes) {
    assert(dest0 == dest1);
    assert(src0 == src1);
    return memmove(dest0, src0, bytes);
}
//===========================================================================
void delay(int16_t ms) {
    Sleep((DWORD)ms);
}
//===========================================================================
void read_joystick(void) {
    return; // TODO read_joystick
}
//===========================================================================
void xsetmode(void) {
    return; // TODO xsetmode
}
//===========================================================================
void xshowpage(unsigned page) {
    return; // TODO xshowpage
}
//===========================================================================
void xfillrectangle(int16_t StartX, int16_t StartY, int16_t EndX, int16_t EndY,
    uint16_t PageBase, int16_t Color) {
    return; // TODO xfillrectangle
}
//===========================================================================
void xpset(int16_t X, int16_t Y, uint16_t PageBase, int16_t Color) {
    return; // TODO xpset
}
//===========================================================================
int16_t xpoint(int16_t X, int16_t Y, uint16_t PageBase) {
    return 0; // TODO xpoint
}
//===========================================================================
void xput(int16_t x,int16_t y,uint16_t pagebase,char *buff) {
    return; // TODO xput
}
//===========================================================================
void xtext(int16_t x,int16_t y,uint16_t pagebase,char *buff,int16_t color) {
    return; // TODO xtext
}
//===========================================================================
void xtext1(int16_t x,int16_t y,uint16_t pagebase,char *buff,int16_t color) {
    return; // TODO xtext1
}
//===========================================================================
void xtextx(int16_t x,int16_t y,uint16_t pagebase,char *buff,int16_t color) {
    return; // TODO xtextx
}
//===========================================================================
void xfput(int16_t x,int16_t y,uint16_t pagebase,char *buff) {
    return; // TODO xfput
}
//===========================================================================
void xfarput(int16_t x,int16_t y,uint16_t pagebase,char *buff) {
    return; // TODO xfarput
}
//===========================================================================
void xcopys2d(int16_t SourceStartX, int16_t SourceStartY,
    int16_t SourceEndX, int16_t SourceEndY, int16_t DestStartX,
    int16_t DestStartY, char* SourcePtr, uint16_t DestPageBase,
    int16_t SourceBitmapWidth, int16_t DestBitmapWidth) {
    return; // TODO xcopys2d
}
//===========================================================================
void xcopyd2d(int16_t SourceStartX, int16_t SourceStartY,
    int16_t SourceEndX, int16_t SourceEndY, int16_t DestStartX,
    int16_t DestStartY, uint16_t SourcePageBase,
    uint16_t DestPageBase, int16_t SourceBitmapWidth,
    int16_t DestBitmapWidth) {
    return; // TODO xcopyd2d
}
//===========================================================================
void xdisplay_actors(ACTOR *act,uint16_t page) {
    return; // TODO xdisplay_actors
}
//===========================================================================
void xerase_actors(ACTOR *act,uint16_t page) {
    return; // TODO xerase_actors
}
//===========================================================================
void pal_fade_in(char *buff) {
    return; // TODO pal_fade_in
}
//===========================================================================
void pal_fade_out(char *buff) {
    return; // TODO pal_fade_out
}
//===========================================================================
int32_t lzss_decompress(char *src,char *dest) {
    int32_t result = 0;

    uint16_t original_size = *(uint16_t*)src;
    ++src; ++src;
    uint16_t one = *(uint16_t*)src;
    ++src; ++src;
    assert(one == 1);

    char* original_src = src;
    char* original_dest = dest;

    while (result < original_size) {
        char b = *src;
        src++;
        for (int bit = 0; bit < 8; ++bit) {
            if (b & (1 << bit)) {
                *dest = *src;
                ++dest;
                ++src;
                ++result;
                if (result == original_size) {
                    goto done;
                }
            } else {
                uint16_t d = *((uint16_t*)src);
                uint8_t count = (uint8_t)((d >> 12) + 2);
                uint16_t offset = (d & 0x0fff);
                char *lookup = dest - (size_t)offset;
                assert(lookup >= original_dest);
                for (uint8_t c = 0; c < count; ++c) {
                    *dest = *lookup;
                    ++dest;
                    ++lookup;
                    ++result;
                    if (result == original_size) {
                        goto done;
                    }
                }
                ++src;
                ++src;
            }
        }
    }
done:
    return result;
}