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

SDL_Window* sdl_window = NULL;
SDL_Surface* sdl_window_surface = NULL;
SDL_TimerID sdl_timer = 0;
palette_t sdl_palette;
uint16_t current_page = 0;

extern SETUP setup;
extern char AdLibPresent;
extern volatile uint16_t timer_cnt,vbl_cnt,magic_cnt,extra_cnt;
extern int16_t music_flag,sound_flag,pcsound_flag;
extern char noal,nosb;
volatile int32_t TickCount2,TickCount;
int32_t TimerDivisor,TimerCount;

//===========================================================================
void tick_update(void) {
    SDL_UpdateWindowSurface(sdl_window);
}
//===========================================================================
void sdl_update(void) {
    SDL_Event event;
    while (SDL_PollEvent (&event))
    {
        switch(event.type)
        {
        case SDL_QUIT:
            exit_code(0);
            exit(0);
            break;
        case SDL_USEREVENT:
            tick_update();
            break;
        }
    }
}
//===========================================================================
void sdl_exit(void) {
    if (sdl_window) {
        SDL_DestroyWindow(sdl_window);
        sdl_window = NULL;
        sdl_window_surface = NULL;
    }
    SDL_Quit();
}
//===========================================================================
uint32_t convert_color(uint8_t index) {
    return SDL_MapRGBA(sdl_window_surface->format,
        sdl_palette.colors[index].r,
        sdl_palette.colors[index].g,
        sdl_palette.colors[index].b,
        sdl_palette.colors[index].a);
}
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
    assert(SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_EVENTS) == 0);
    sdl_window = SDL_CreateWindow("God of Thunder", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, NATIVE_SCREEN_WIDTH * 4, NATIVE_SCREEN_HEIGHT * 4, 0);
    assert(sdl_window);
    sdl_window_surface = SDL_GetWindowSurface(sdl_window);
    assert(sdl_window_surface);
    sdl_timer = SDL_AddTimer(1000 / 50, t0Service, NULL);
}
//===========================================================================
void xshowpage(unsigned page) {
    current_page = page;
    return; // TODO xshowpage
}
//===========================================================================
void xfillrectangle(int16_t StartX, int16_t StartY, int16_t EndX, int16_t EndY,
    uint16_t PageBase, int16_t Color) {
    SDL_Rect rect;
    rect.x = StartX;
    rect.y = StartY;
    rect.w = EndX - StartX;
    rect.h = EndY - StartY;
    SDL_FillRect(sdl_window_surface, &rect, convert_color((uint8_t)Color));
}
//===========================================================================
void xpset(int16_t X, int16_t Y, uint16_t PageBase, int16_t Color) {
    int bpp = sdl_window_surface->format->BytesPerPixel;

    
    Uint32 pixel = convert_color((uint8_t)Color);
    Uint8 *p = (Uint8 *)sdl_window_surface->pixels + Y * sdl_window_surface->pitch + X * bpp;

    switch (bpp) {
    case 1:
        *p = pixel;
        break;
    case 2:
        *(Uint16 *)p = pixel;
        break;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;
    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}
//===========================================================================
int16_t xpoint(int16_t X, int16_t Y, uint16_t PageBase) {
    int bpp = sdl_window_surface->format->BytesPerPixel;

    Uint8 *p = (Uint8 *)sdl_window_surface->pixels + Y * sdl_window_surface->pitch + X * bpp;

    switch (bpp) {
    case 1:
        return *p;
    case 2:
        return *(Uint16 *)p;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            return p[0] << 16 | p[1] << 8 | p[2];
        } else {
            return p[0] | p[1] << 8 | p[2] << 16;
        }
    case 4:
        return *(Uint32 *)p;
    default:
        return 0;
    }
}
//===========================================================================
static void xput_layer(int16_t x,int16_t y,uint16_t pagebase,char *buff, int offset) {
    int16_t width = *(int16_t*)(buff);
    int16_t height = *(int16_t*)(buff + 2);
    int16_t mask = *(int16_t*)(buff + 4);
    uint8_t* pixels = buff + 6;
    uint32_t index = width * height * offset;
    int32_t pitch = sdl_window_surface->pitch / sdl_window_surface->format->BytesPerPixel;
    for (int32_t curr_y = 0; curr_y < height; ++curr_y) {
        for (int32_t curr_x = 0; curr_x < width; ++curr_x) {
            if (pixels[index] != mask) {
                int32_t sx = x + curr_x * 4 + offset;
                int32_t sy = y + curr_y;
                if (pagebase) {
                    sy += (pagebase / 19200) * NATIVE_SCREEN_HEIGHT;
                }
                //if (sx >= 0 && sx < NATIVE_SCREEN_WIDTH &&
                //    sy >= 0 && sy < NATIVE_SCREEN_HEIGHT) {
                ((uint32_t*)sdl_window_surface->pixels)[sy * pitch + sx] = convert_color(pixels[index]);
                //}
            }
            ++index;
        }
    }
}
//===========================================================================
void xput(int16_t x,int16_t y,uint16_t pagebase,char *buff) {
    xput_layer(x, y, pagebase, buff, 0);
    xput_layer(x, y, pagebase, buff, 1);
    xput_layer(x, y, pagebase, buff, 2);
    xput_layer(x, y, pagebase, buff, 3);
}
//===========================================================================
void xtext(int16_t x,int16_t y,uint16_t pagebase,char *buff,int16_t color) {
    int oldx = x;
    for (int c = 0; buff[c] != '\0'; ++c) {
        if (buff[c] == '\n') {
            y += 16;
            x = oldx;
        } else {
            xput(x, y, pagebase, text[buff[c] * 16 * 16]);
        }
    }
    return; // TODO xtext
}
//===========================================================================
void xtext1(int16_t x,int16_t y,uint16_t pagebase,char *buff,int16_t color) {
    xtext(x, y, pagebase, buff, color);
}
//===========================================================================
void xtextx(int16_t x,int16_t y,uint16_t pagebase,char *buff,int16_t color) {
    xtext(x, y, pagebase, buff, color);
}
//===========================================================================
void xfput(int16_t x,int16_t y,uint16_t pagebase,char *buff) {
    xput(x, y, pagebase, buff);
}
//===========================================================================
void xfarput(int16_t x,int16_t y,uint16_t pagebase,char *buff) {
    xput(x, y, pagebase, buff);
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
    uint16_t original_size = *(uint16_t*)src; src += 2;
    uint16_t one = *(uint16_t*)src; src += 2;
    assert(one == 1);

    char* original_src = src;
    char* original_dest = dest;

    while ((uint16_t)(dest - original_dest) < original_size) {
        char b = *src; ++src;
        for (int bit = 0; bit < 8; ++bit) {
            if (b & (1 << bit)) {
                *dest = *src;
                ++dest; ++src;
                if ((uint16_t)(dest - original_dest) == original_size) {
                    goto done;
                }
            } else {
                uint16_t d = *((uint16_t*)src); src += 2;
                uint8_t count = (uint8_t)((d >> 12) + 2);
                uint16_t offset = (d & 0x0fff);
                char *lookup = dest - (size_t)offset;
                assert(lookup >= original_dest);
                for (uint8_t c = 0; c < count; ++c) {
                    *dest = *lookup;
                    ++dest; ++lookup;
                    if ((uint16_t)(dest - original_dest) == original_size) {
                        goto done;
                    }
                }
            }
        }
    }
done:
    return (int32_t)(dest - original_dest);
}