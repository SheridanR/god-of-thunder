// THOR - The God of Thunder
// See LICENSE for details

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <Windows.h>

#include "../define.h"
#include "../proto.h"
#include "new.h"

//===========================================================================
void* movedata (void* dest0, void* dest1, void* src0, void* src1, size_t bytes) {
    assert(dest0 == dest1);
    assert(src0 == src1);
    return memmove(dest0, src0, bytes);
}
//===========================================================================
void delay(int ms) {
    Sleep((DWORD)ms);
}
//===========================================================================
void read_joystick(void) {
    return; // TODO
}
//===========================================================================
void xsetmode(void) {
    return; // TODO
}
//===========================================================================
void xshowpage(unsigned page) {
    return; // TODO
}
//===========================================================================
void xfillrectangle(int StartX, int StartY, int EndX, int EndY,
    unsigned int PageBase, int Color) {
    return; // TODO
}
//===========================================================================
void xpset(int X, int Y, unsigned int PageBase, int Color) {
    return; // TODO
}
//===========================================================================
int xpoint(int X, int Y, unsigned int PageBase) {
    return 0; // TODO
}
//===========================================================================
void xput(int x,int y,unsigned int pagebase,char *buff) {
    return; // TODO
}
//===========================================================================
void xtext(int x,int y,unsigned int pagebase,char *buff,int color) {
    return; // TODO
}
//===========================================================================
void xtext1(int x,int y,unsigned int pagebase,char *buff,int color) {
    return; // TODO
}
//===========================================================================
void xtextx(int x,int y,unsigned int pagebase,char *buff,int color) {
    return; // TODO
}
//===========================================================================
void xfput(int x,int y,unsigned int pagebase,char *buff) {
    return; // TODO
}
//===========================================================================
void xfarput(int x,int y,unsigned int pagebase,char *buff) {
    return; // TODO
}
//===========================================================================
void xcopys2d(int SourceStartX, int SourceStartY,
    int SourceEndX, int SourceEndY, int DestStartX,
    int DestStartY, char* SourcePtr, unsigned int DestPageBase,
    int SourceBitmapWidth, int DestBitmapWidth) {
    return; // TODO
}
//===========================================================================
void xcopyd2d(int SourceStartX, int SourceStartY,
    int SourceEndX, int SourceEndY, int DestStartX,
    int DestStartY, unsigned int SourcePageBase,
    unsigned int DestPageBase, int SourceBitmapWidth,
    int DestBitmapWidth) {
    return; // TODO
}
//===========================================================================
void xdisplay_actors(ACTOR *act,unsigned int page) {
    return; // TODO
}
//===========================================================================
void xerase_actors(ACTOR *act,unsigned int page) {
    return; // TODO
}
//===========================================================================
void pal_fade_in(char *buff) {
    return; // TODO
}
//===========================================================================
void pal_fade_out(char *buff) {
    return; // TODO
}
//===========================================================================
long res_read(char *name,char *buff) {
    return 0; // TODO
}
//===========================================================================
char* res_falloc_read(char *name) {
    return NULL; // TODO
}
//===========================================================================
int res_find_name(char *name) {
    return 0; // TODO
}
//===========================================================================
void res_init(char *buf) {
    return; // TODO
}
//===========================================================================
int res_open(char *name) {
    return 0; // TODO
}
//===========================================================================
void res_close(void) {
    return; // TODO
}