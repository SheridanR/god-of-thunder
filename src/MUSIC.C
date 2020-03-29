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
int music_init(void){

    if(!setup.music) return 0;
    music_current=-1;
    return 1;
}
//=========================================================================
void music_play(int num,int override){
    return; // TODO
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
    return; // TODO
    /*
    if(!setup.music) return;
    MU_MusicOff();
    */
}
//=========================================================================
void music_resume(void){
    return; // TODO
    /*
    if(!setup.music) return;
    MU_MusicOn();
    */
}
//=========================================================================
int music_is_on(void){
    return 0; // TODO
    /*
    if(!setup.music) return 0;
    return MU_MusicPlaying();
    */
}





