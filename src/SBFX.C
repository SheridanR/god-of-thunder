// See LICENSE for details

#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include "define.h"
#include "proto.h"

extern SETUP setup;
extern char AdLibPresent;
extern volatile uint16_t timer_cnt,vbl_cnt,magic_cnt,extra_cnt;
extern int16_t music_flag,sound_flag,pcsound_flag;
extern char noal,nosb;

void interrupt (*t0OldService)(void);
int32_t TimerDivisor,TimerCount;
volatile int32_t TickCount2,TickCount;
//===========================================================================
uint32_t interrupt t0Service(uint32_t interval, void* param){
    SDL_UserEvent userevent;
    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user = userevent;
    SDL_PushEvent(&event);

    timer_cnt++;
    vbl_cnt++;
    magic_cnt++;
    extra_cnt++;

    TickCount2++;
    TickCount = TickCount2 >> 1;
    TimerCount += TimerDivisor;

    if(TimerCount >= 0x10000L){
        TimerCount -= 0x10000L;
    }

    return interval;
}
char *SB_DetectAdLib(void);
//===========================================================================
int16_t sbfx_init(void){
    return 0; // TODO sbfx_init

    /*
    unsigned speed;
    char *sberr;

    t0OldService = getvect(8);
    setvect(8,t0Service);

    speed=(unsigned) (1192030L/120L);
    outportb(0x43,0x36);
    outportb(0x40,speed);
    outportb(0x40,speed >> 8);
    TimerDivisor=speed;

    music_flag=0;       //is hardware available flags
    sound_flag=0;
    if(noal) return 1;
    if(nosb){
        sberr=SB_DetectAdLib();
        //  if(sberr) return 0;
        if(!AdLibPresent){
            SB_Shutdown();
            return 0;
        }
        music_flag=1;
        return 1;
    }
    else{
        sberr=SB_Startup(getenv("BLASTER"));
        if(sberr){
            exit_code(0);
            printf("\r\n%s\r\n",sberr);
            //getch();
            exit(0);
        }
        //if(!SoundBlasterPresent){
        if (0) {
            SB_Shutdown();
            return 0;
        }
        music_flag=1;
        sound_flag=1;
    }
    return 1;
    */
}
//===========================================================================
void sbfx_exit(void){
    return; // TODO sbfx_exit

    /*
    int16_t i;

    if(setup.pc_sound) FX_StopPC();
    if(setup.music) MU_MusicOff();
    if(setup.music || setup.dig_sound) SB_Shutdown();

    for(i=0;i<0xf5;i++) SB_ALOut(i,0);

    outportb(0x43,0x36);				// Change timer 0
    outportb(0x40,0);
    outportb(0x40,0);
    setvect(8,t0OldService);			// Restore old timer 0 ISR
    */
}
