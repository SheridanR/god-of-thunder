// THOR - The God of Thunder
//Source code released to the public domain on March 27th, 2020.

//
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>

#include <1_define.h>
#include <1_proto.h>
//===========================================================================
extern char *tmp_buff;
extern int new_level,current_level;
extern LEVEL scrn;
extern ACTOR actor[MAX_ACTORS];
extern ACTOR *thor;
extern int thor_x1,thor_y1,thor_x2,thor_y2,thor_real_y1;
extern ACTOR *hammer;
extern int key_fire,key_up,key_down,key_left,key_right,key_magic,key_select;
extern int lightning_used,tornado_used,hourglass_flag;
extern unsigned int display_page,draw_page;
extern THOR_INFO thor_info;
extern int boss_dead;
extern int boss_active;
extern char pge;
extern ACTOR explosion;
extern volatile char key_flag[100];
extern volatile unsigned int timer_cnt;
extern SETUP setup;
extern char far *sd_data;
extern char game_over;
extern char cheat;
extern int exit_flag;
//===========================================================================
int boss1_movement(ACTOR *actr){    //boss - snake
int d,x1,y1,f;

if(boss_dead) return boss_dead1();
d=actr->last_dir;
if(actr->edge_counter){
  actr->edge_counter--;
  goto done;
}
x1=actr->x;
y1=actr->y;

if(overlap(actr->x+2,actr->y+8,actr->x+30,actr->y+30,
          thor->x,thor->y+8,thor->x+15,thor->y+15)){
  thor_damaged(actr);
}
if(actr->temp3){  //start striking
  actr->temp3--;
  if(!actr->temp3) play_sound(BOSS11,0);
  if(hourglass_flag) actr->num_moves=3;
  else actr->num_moves=6;
  goto done0;
}
if(actr->temp1){  //strike
  actr->temp1--;
  if(actr->x<(thor_x1+12)) actr->temp1=0;
  actr->temp2=1;
  d=2;
  actr->x-=2;
  if(overlap(actr->x+2,actr->y+8,actr->x+32,actr->y+30,
             thor->x,thor->y+8,thor->x+15,thor->y+15)){
    actr->temp1=0;
    goto done1;
  }
  actr->next=3;
  actr->num_moves=setup.skill+2;
  goto done1;
}
if(actr->temp2){          //retreating
  if(actr->x<256){
    d=3;
    actr->x+=2;
    if(overlap(actr->x+2,actr->y+8,actr->x+32,actr->y+30,
               thor->x,thor->y+8,thor->x+15,thor->y+15)){
      goto done1;
    }
    actr->num_moves=setup.skill+1;
    goto done0;
  }
  else actr->temp2=0;
}
if(abs((thor_y1)-(actr->y+20))<8){
  if(!actr->temp1){
    if(actr->x>thor_x1){
      actr->temp3=75;
      actr->temp1=130;
      actr->temp2=0;
    }
  }
}
f=0;
if(actr->counter){
  actr->counter--;
  switch(d){
        case 1:
             x1=actor[5].x;
             y1=actor[5].y;
             y1+=2;
             if(!check_move2(x1,y1,&actor[5])) f=1;
             else{
               actr->x=actor[5].x;
               actr->y=actor[5].y-16;
             }
             break;
        case 0:
             y1-=2;
             if(!check_move2(x1,y1,actr)) f=1;
             break;
        case 3:
             x1=actor[5].x;
             y1=actor[5].y;
             y1+=2;
             if(!check_move2(x1,y1,&actor[5])) f=1;
             else{
               actr->x=actor[5].x;
               actr->y=actor[5].y-16;
             }
             break;
        case 2:
             y1-=2;
             if(!check_move2(x1,y1,actr)) f=1;
             break;
  }
}
else f=1;

if(f==1){
//  if(key_flag[_Z]){
//   f=5;
//  }
  actr->counter=rnd(90)+10;
//  if(BP) printt(actr->counter);
  d=rnd(2);
  actr->edge_counter=20;
}

done:
if(d>1) d-=2;

done0:
next_frame(actr);
if(actr->next==3) actr->next=0;

done1:
actr->last_dir=d;

actor[4].next=actr->next;
actor[5].next=actr->next;
actor[6].next=actr->next;

actor[4].last_dir=d;
actor[5].last_dir=d;
actor[6].last_dir=d;

actor[4].x=actr->x+16;
actor[4].y=actr->y;
actor[5].x=actr->x;
actor[5].y=actr->y+16;
actor[6].x=actr->x+16;
actor[6].y=actr->y+16;
actor[4].num_moves=actr->num_moves;
actor[5].num_moves=actr->num_moves;
actor[6].num_moves=actr->num_moves;

if(actr->directions==1) return 0;
return d;
}
//===========================================================================
void check_boss1_hit(ACTOR *actr,int x1,int y1,int x2,int y2,int act_num){
int rep;

if(actr->move==15 && act_num==4){
  if((!actor[3].vunerable) && (actor[3].next!=3) &&
    overlap(x1,y1,x2,y2,actr->x+6,actr->y+4,actr->x+14,actr->y+20)){
    actor_damaged(&actor[3],hammer->strength);
    if(cheat && key_flag[_Z]) actor[3].health=0;
    else actor[3].health-=10;
    actor[3].speed_count=50;
    boss_status(actor[3].health);
    actor[3].vunerable=100;
    play_sound(BOSS13,1);
    actor[3].next=1;
    for(rep=4;rep<7;rep++){
       actor[rep].next=1;
       actor[rep].speed_count=50;
    }
    if(actor[3].health==0) boss_dead=1;
  }
}
}
//===========================================================================
void boss_level1(void){

setup_boss(1);
boss_active=1;
boss_status(-1);
music_pause();
play_sound(BOSS11,1);
pause(120);
music_play(5,1);
}
//===========================================================================
int boss_dead1(void){
int rep,n,x,y,r,x1,y1;

hourglass_flag=0;
if(boss_dead==1){
  REPEAT(4){
    x1=actor[3+rep].last_x[pge];
    y1=actor[3+rep].last_y[pge];
    x=actor[3+rep].x;
    y=actor[3+rep].y;
    n=actor[3+rep].actor_num;
    r=actor[3+rep].rating;
    memcpy(&actor[3+rep],&explosion,sizeof(ACTOR));
    actor[3+rep].actor_num=n;
    actor[3+rep].rating=r;
    actor[3+rep].x=x;
    actor[3+rep].y=y;
    actor[3+rep].last_x[pge]=x1;
    actor[3+rep].last_x[pge^1]=x;
    actor[3+rep].last_y[pge]=y1;
    actor[3+rep].last_y[pge^1]=y;
    actor[3+rep].used=1;
    actor[3+rep].vunerable=255;
    actor[3+rep].move=6;
    actor[3+rep].next=rep;
    actor[3+rep].speed=rnd(3)+6;
    actor[3+rep].num_shots=(10-actor[3+rep].speed)*10;
    actor[3+rep].speed_count=actor[3+rep].speed;
  }
  play_sound(EXPLODE,1);
  boss_dead++;
  for(rep=7;rep<MAX_ACTORS;rep++)
     if(actor[rep].used) actor_destroyed(&actor[rep]);
}
return actor[3].last_dir;
}
//===========================================================================
void closing_sequence1(void){
LEVEL far *lvl;
int rep;

music_play(4,1);
odin_speaks(1001,13);
thor_info.armor=1;
load_new_thor();
thor->dir=1;
thor->next=0;
xerase_actors(actor,display_page);
xdisplay_actors(&actor[MAX_ACTORS-1],display_page);
fill_score(20);
score_for_inv();

fill_health();
fill_magic();
odin_speaks(1002,0);

REPEAT(16) scrn.actor_type[rep]=0;
boss_dead=0;
setup.boss_dead[0]=1;
game_over=1;
boss_active=0;
scrn.type=4;
show_level(BOSS_LEVEL1);

play_sound(ANGEL,1);
place_tile(18,6,148);
place_tile(19,6,202);
actor_visible(1);
actor_visible(2);

lvl=(LEVEL far *) (sd_data+(59*512));
lvl->icon[6][18]=148;
lvl->icon[6][19]=202;
exit_flag=0;
}
