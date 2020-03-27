//Source code released to the public domain on March 27th, 2020.

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <dos.h>
#include <bios.h>
#include <fcntl.h>

#include <1_define.h>
#include <1_proto.h>
#include <res_man.h>
#include "FX_Manc.h"
#include "MU_Manc.h"
#include "SB_Manc.h"

extern char far *song;
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

if(!setup.music) return;
if((num!=music_current) || override){
  if(num!=music_current) load_music(num);
  MU_StartMusic(song,1);
  music_current=num;
}
}
//=========================================================================
void music_pause(void){

if(!setup.music) return;
MU_MusicOff();
}
//=========================================================================
void music_resume(void){

if(!setup.music) return;
MU_MusicOn();
}
//=========================================================================
int music_is_on(void){

if(!setup.music) return 0;
return MU_MusicPlaying();
}





