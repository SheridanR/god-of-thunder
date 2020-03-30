// See LICENSE for details

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "define.h"
#include "proto.h"

extern char *song;
extern char music_current;
extern struct sup setup;
//=========================================================================
int16_t music_init(void){

    if(!setup.music) return 0;
    music_current=-1;
    return 1;
}
//=========================================================================
void music_play(int16_t num,int16_t override){
    return; // TODO music_play
    /*
    if(!setup.music) return;
    if((num!=music_current) || override){
        if(num!=music_current) load_music(num);
        MU_StartMusic(song,1);
        music_current=num;
    }
    */
}
//=========================================================================
void music_pause(void){
    return; // TODO music_pause
    /*
    if(!setup.music) return;
    MU_MusicOff();
    */
}
//=========================================================================
void music_resume(void){
    return; // TODO music_resume
    /*
    if(!setup.music) return;
    MU_MusicOn();
    */
}
//=========================================================================
int16_t music_is_on(void){
    return 0; // TODO music_is_on
    /*
    if(!setup.music) return 0;
    return MU_MusicPlaying();
    */
}





