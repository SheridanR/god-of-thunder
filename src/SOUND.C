// See LICENSE for details

#include <stdio.h>
#include <stdlib.h>

#include "define.h"
#include "proto.h"
#include "new/new.h"

void play_pc_sound(int index, int priority_override);
//===========================================================================
extern char *std_sounds;
extern char *pcstd_sounds;
extern char *pc_sound[NUM_SOUNDS];
extern char *dig_sound[NUM_SOUNDS];
extern char *std_sound_start;
extern char *pcstd_sound_start;
extern int level;

//enum{OW,GULP,SWISH,YAH,ELECTRIC,THUNDER,DOOR,FALL,
//     ANGEL,WOOP,ANGEL,BRAAPP,WIND,PUNCH1,CLANG,EXPLODE
//     BOSS11,BOSS12,BOSS13
int  sound_priority[]={1,2,3,3,3,1,4,4,4,5,4,3,1,2,2,5,1,3,1};
extern long pcsound_length[NUM_SOUNDS];

int current_priority;
//===========================================================================
extern SETUP setup;
extern char ds_file[];
//===========================================================================
int sound_init(void){
    int i;
    char *p;
    HEADER *header;

    std_sound_start=res_falloc_read("DIGSOUND");
    if(!std_sound_start) return 0;
    std_sounds=std_sound_start;
    header=(HEADER *) std_sounds;
    std_sounds=std_sounds+(sizeof(HEADER) * 16);

    p=std_sounds;
    for(i=0;i<16;i++){
        dig_sound[i]=p;
        p+=(int) header->length;
        header++;
    }

    pcstd_sound_start=res_falloc_read("PCSOUNDS");
    if(!pcstd_sound_start) return 0;
    pcstd_sounds=pcstd_sound_start;
    header=(HEADER *) pcstd_sounds;
    pcstd_sounds=pcstd_sounds+(sizeof(HEADER) * 16);

    p=pcstd_sounds;
    for(i=0;i<16;i++){
        pc_sound[i]=p;
        pc_sound[i][0]=0;
        pc_sound[i][1]=0;
        pcsound_length[i]=header->length;
        p+=(int) header->length;
        header++;
    }
    current_priority=255;
    return 1;
}
//===========================================================================
void sound_exit(void){
    return; // TODO
    /*
    FX_StopPC();
    while(FX_PCPlaying());
    SB_StopVOC();
    while(sound_playing());

    if(std_sound_start) farfree(std_sounds);
    if(pcstd_sound_start) farfree(pcstd_sounds);
    */
}
//===========================================================================
void play_sound(int index, int priority_override){
    return; // TODO
    /*
    if(index>=NUM_SOUNDS) return;
    if(setup.pc_sound){
        play_pc_sound(index,priority_override);
        return;
    }
    if(!setup.dig_sound) return;

    if(sound_playing()){
        if((!priority_override) && current_priority<sound_priority[index]) return;
        SB_StopVOC();
    }

    SB_PlayVOC((char *) dig_sound[index],1);
    current_priority=sound_priority[index];
    */
}
//===========================================================================
void play_pc_sound(int index, int priority_override){
    return; // TODO

    /*if(FX_PCPlaying()){
        if((!priority_override) && current_priority<sound_priority[index]) return;
        FX_StopPC();
    }

    FX_PlayPC((PCSound *) pc_sound[index],pcsound_length[index]);
    current_priority=sound_priority[index];*/
}
//===========================================================================
int sound_playing(void){
    return 0; // TODO
    /*
    return SB_VOCPlaying();
    */
}
