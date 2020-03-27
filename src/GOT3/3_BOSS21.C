// THOR - The God of Thunder
//Source code released to the public domain on March 27th, 2020.

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>

#include <3_define.h>
#include <3_proto.h>

void check_boss_hit(void);
//===========================================================================
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
extern int rand1,rand2;
extern char apple_drop;
//===========================================================================
int boss_movement(ACTOR *actr){    //boss - wraith
int d,x1,y1,f,ox,oy;

if(boss_dead) return boss_dead();
check_boss_hit();
d=actr->last_dir;
actr->num_moves=2;
actr->temp3++;

if(actr->temp2) actr->temp2--;
if(actr->temp1) actr->temp1--;
else if(actor[4].num_shots<actor[4].shots_allowed){
   actor_always_shoots(&actor[4],0);
   actor[actor[4].shot_actor].temp1=20;
   actr->temp1=20;
}
f=0;
if(actr->temp4){
  actr->temp4--;
  if(!actr->temp4){
    actr->temp3=0;
    actor[3].frame_speed=4;
    actor[3].dir=0;
    actor[3].last_dir=0;
    actor[3].next=0;
    actor[4].dir=0;
    actor[4].last_dir=0;
    actor[4].next=0;
    actor_always_shoots(actr,0);
    actor[actr->shot_actor].x=actr->x+8;
    actor[actr->shot_actor].y=actr->y;
    actor[actr->shot_actor].temp1=rnd(100)+60;
    actor[actr->shot_actor].speed=2;
  }
  goto skip_move;
}

if(actr->edge_counter) actr->edge_counter--;
else goto new_dir;

if(overlap(actr->x+2,actr->y+8,actr->x+30,actr->y+30,
          thor->x,thor->y+4,thor->x+15,thor->y+15))
  thor_damaged(actr);

ox=actr->x;
oy=actr->y;
switch(actr->temp5){
      case 0:
           x1=actor[3].x;
           y1=actor[3].y-2;
           if(!check_move2(x1,y1,&actor[3])){
             f=1;
             break;
           }
           if(!check_move2(x1+16,y1,&actor[4])) f=1;
           actr->y=oy-2;
           break;
      case 1:
           x1=actor[5].x;
           y1=actor[5].y+2;
           if(!check_move2(x1,y1,&actor[5])){
             f=1;
             break;
           }
           if(!check_move2(x1+16,y1,&actor[6])) f=1;
           actr->y=oy+2;
           break;
      case 2:
           x1=actor[3].x-2;
           y1=actor[3].y;
           if(!check_move2(x1,y1,&actor[3])){
             f=1;
             break;
           }
           if(!check_move2(x1,y1+16,&actor[5])) f=1;
           actr->x=ox-2;
           break;
      case 3:
           x1=actor[4].x+2;
           y1=actor[4].y;
           if(!check_move2(x1,y1,&actor[4])){
             f=1;
             break;
           }
           if(!check_move2(x1,y1+16,&actor[6])) f=1;
           actr->x=ox+2;
           break;
      case 4:               //ul
           x1=actor[3].x-2;
           y1=actor[3].y-2;
           if(!check_move2(x1,y1,&actor[3])){
             f=1;
             break;
           }
           actr->x=ox-2;
           actr->y=oy-2;
           break;
      case 5:
           x1=actor[4].x+2;
           y1=actor[4].y-2;
           if(!check_move2(x1,y1,&actor[4])){
             f=1;
             break;
           }
           actr->x=ox+2;
           actr->y=oy-2;
           break;
      case 6:
           x1=actor[6].x+2;
           y1=actor[6].y+2;
           if(!check_move2(x1,y1,&actor[6])){
             f=1;
             break;
           }
           actr->x=ox+2;
           actr->y=oy+2;
           break;
      case 7:
           x1=actor[5].x-2;
           y1=actor[5].y+2;
           if(!check_move2(x1,y1,&actor[5])){
             f=1;
             break;
           }
           actr->x=ox-2;
           actr->y=oy+2;
           break;
}

skip_move:
next_frame(actr);

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

if(!f) goto done;

new_dir:
if(actr->temp3<120) goto new_dir1;
actor[3].frame_speed=8;
actor[3].dir=1;
actor[3].last_dir=1;
actor[3].next=0;
actor[4].last_dir=1;
actor[4].dir=1;
actor[4].next=0;
actr->temp4=120;
play_sound(BOSS12,1);

new_dir1:
actr->temp5=rand1%8;
actr->edge_counter=rand2+60;

done:
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
void check_boss_hit(void){
int rep;

if(actor[3].magic_hit || actor[4].magic_hit ||
   actor[5].magic_hit || actor[6].magic_hit){
  if(!actor[3].temp2){
    actor_damaged(&actor[3],10);
    actor[3].health-=10;
    actor[3].speed_count=50;
    boss_status(actor[3].health);
    actor[3].vunerable=50;
    play_sound(BOSS13,1);
    for(rep=4;rep<7;rep++){
       actor[rep].magic_hit=0;
       actor[rep].next=1;
       actor[rep].speed_count=50;
    }
    if(actor[3].health==0){
      boss_dead=1;
      for(rep=7;rep<MAX_ACTORS;rep++)
         if(actor[rep].used)
           actor_destroyed(&actor[rep]);
    }
    actor[3].temp2=40;
  }
  for(rep=3;rep<7;rep++) actor[rep].magic_hit=0;
}
}
//===========================================================================
void boss_level(void){

//!!!!!!!!!!!!!!messed this up
setup_boss(2);
boss_active=1;
boss_status(-1);
music_pause();
play_sound(BOSS12,1);
timer_cnt=0;

while(timer_cnt<120) rotate_pal();
music_play(5,1);
apple_drop=0;
}
//===========================================================================
int boss_dead(void){
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
}
return actor[3].last_dir;
}
//===========================================================================
void closing_sequence(void){
int rep;

music_play(0,1);
odin_speaks(1002,0);
add_health(150);
add_magic(150);
boss_dead=0;
setup.boss_dead[0]=1;
boss_active=0;
REPEAT(16) scrn.actor_type[rep]=0;
place_tile(9,2,169);
show_level(BOSS_LEVEL);
}
