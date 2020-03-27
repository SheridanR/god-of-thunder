// THOR - The God of Thunder
//Source code released to the public domain on March 27th, 2020.

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>

#include <2_define.h>
#include <2_proto.h>
void boss_set(int d,int x,int y);
int bossa_movement(ACTOR *actr);
int bossb_movement(ACTOR *actr);
//===========================================================================
extern int new_level,current_level;
extern char far *bg_pics;
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
extern int thunder_flag;
extern int exit_flag,game_is_over;
extern char game_over;
extern char far *sd_data;
//===========================================================================
char exp[]={ 61, 62, 65, 66, 69, 70, 73, 74, 77, 78,
             81, 82, 85, 86, 89, 90, 93, 94, 97, 98,
            101,102,105,106,109,110,113,114,117,118,
            121,122,125,126,129,130,133,134,137,138,
            141,142,145,146,149,150,153,154,157,158,
            161,162,165,166,169,170,173,174,177,178};
char expf[60];
char num_skulls;  //hehe
char num_spikes;
//===========================================================================
int boss_movement(ACTOR *actr){    //boss - skull
int d,f,x;
static int drop_flag=0;

switch(setup.skill){
   case 0:
      num_skulls=3;
      num_spikes=5;
      break;
   case 1:
      num_skulls=6;
      num_spikes=8;
      break;
   case 2:
      num_skulls=9;
      num_spikes=11;
      break;
}
if(boss_dead) return boss_die();
if(actr->i1){
  if(actr->i1==1) return bossa_movement(actr);
  else return bossb_movement(actr);
}
d=actr->last_dir;

//if(overlap(actr->x+2,actr->y+8,actr->x+30,actr->y+30,
//          thor->x,thor->y+4,thor->x+15,thor->y+15))
//  thor_damaged(actr);

x=actr->x;

f=0;
if(actr->temp6) actr->temp6--;
if(!actr->temp6){
  drop_flag=0;
  if(actr->temp5) actr->temp5--;
  if(!actr->temp5) f=1;
  else{
    if(d==2){
      if(x>18) actr->x-=2;
      else f=1;
    }
    else if(d==3){
      if(x<272) actr->x+=2;
      else f=1;
   }
  }
  if(f){
    actr->temp5=rand1+60;
    if(d==2) d=3;
    else d=2;
  }
}

actr->frame_count--;
if(actr->frame_count<=0){
  actr->next++;
  if(actr->next>2) actr->next=0;
  actr->frame_count=actr->frame_speed;
}
x=actr->x;
if(actr->num_shots<num_skulls && !drop_flag){
  if(x==48 || x==112 || x==176 || x==240){
    drop_flag=1;
    actor[3].temp6=40;
//    actr->next=3;
    actor_always_shoots(actr,1);
    play_sound(FALL,0);
    actor[actr->shot_actor].x=actr->x+12;
    actor[actr->shot_actor].y=actr->y+32;
    actor[actr->shot_actor].temp2=0;
    actor[actr->shot_actor].temp3=4;
    actor[actr->shot_actor].temp4=4;
  }
}

boss_set(d,x,actr->y);

done:
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
void boss_set(int d,int x,int y){

actor[4].next=actor[3].next;
actor[5].next=actor[3].next;
actor[6].next=actor[3].next;
actor[3].last_dir=d;
actor[4].last_dir=d;
actor[5].last_dir=d;
actor[6].last_dir=d;
actor[4].x=x+16;
actor[4].y=y;
actor[5].x=x;
actor[5].y=y+16;
actor[6].x=x+16;
actor[6].y=y+16;
}
//===========================================================================
void check_boss_hit(ACTOR *actr,int x1,int y1,int x2,int y2,int act_num){
int rep;

if((!actor[3].vunerable)){
  actor_damaged(&actor[3],hammer->strength);
  actor[3].health-=10;
  if(actor[3].health==50){
    play_sound(BOSS12,1);
    timer_cnt=0;
    while(timer_cnt<120);
    actor[3].i1=1;
    actor[3].i2=0;
    memset(expf,0,60);
    for(rep=7;rep<MAX_ACTORS;rep++)
       if(actor[rep].used)
         actor_destroyed(&actor[rep]);
    actor[3].num_shots=0;
  }
  else play_sound(BOSS13,1);
  actor[3].speed_count=75;
  boss_status(actor[3].health);
  actor[3].vunerable=75;
  actor[3].next=1;
  for(rep=4;rep<7;rep++){
     actor[rep].next=1;
     actor[rep].speed_count=50;
  }
  if(actor[3].health==0){
    boss_dead=1;
    for(rep=7;rep<MAX_ACTORS;rep++)
       if(actor[rep].used)
         actor_destroyed(&actor[rep]);
  }
}
x1=x1;
y1=y1;
x2=x2;
y2=y2;
actr=actr;
act_num=act_num;
}
//===========================================================================
void boss_level(void){

setup_boss(2);
boss_active=1;
boss_status(-1);
music_pause();
play_sound(BOSS11,1);
timer_cnt=0;

if(setup.dig_sound || setup.pc_sound){
  while(timer_cnt<200) rotate_pal();
}
music_play(7,1);
//setup_boss(2);
//boss_active=22;
//boss_status(-1);
//music_pause();
//play_sound(BOSS11,1);
//timer_cnt=0;

//while(timer_cnt<120) rotate_pal();
//music_play(4,1);
}
//===========================================================================
int boss_die(void){
int rep,n,x,y,r,x1,y1;

hourglass_flag=0;
thunder_flag=0;
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
LEVEL far *lvl;
int rep;

music_play(6,1);
odin_speaks(1001,0);
thor_info.armor=10;
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
setup.boss_dead[1]=1;
game_over=1;
boss_active=0;
scrn.type=6;
game_is_over=1;
show_level(BOSS_LEVEL);


play_sound(ANGEL,1);
place_tile(18,10,152);
place_tile(19,10,202);
actor_visible(1);
actor_visible(2);
actor[7].x=288;
actor[7].y=160;
actor[8].x=304;
actor[8].y=160;


lvl=(LEVEL far *) (sd_data+(60*512));
lvl->icon[6][18]=152;
lvl->icon[6][19]=202;
exit_flag=0;
}
//===========================================================================
int bossa_movement(ACTOR *actr){    //boss - skull (explode)
int an,x,y,r;

next_frame(actr);
actor[4].next=actr->next;
actor[5].next=actr->next;
actor[6].next=actr->next;
actr->vunerable=20;
if(actr->num_shots) return 0;
if(actor[5].num_shots) return 0;

play_sound(EXPLODE,1);
actor_always_shoots(&actor[5],0);
an=actor[5].shot_actor;
actor[an].move=9;

r=rand1%60;
while(expf[r]){
  r++;
  if(r>59) r=0;
}
expf[r]=1;
x=(exp[r]%20)*16;
y=(exp[r]/20)*16;
actor[an].x=x;
actor[an].y=y;

xfput(x,y,display_page,(char far *) (bg_pics+(scrn.bg_color*262)));
xfput(x,y,draw_page,(char far *) (bg_pics+(scrn.bg_color*262)));
xfput(x,y,PAGE2,(char far *) (bg_pics+(scrn.bg_color*262)));
scrn.icon[y/16][x/16]=scrn.bg_color;

actor[3].i2++;
if(actor[3].i2>59){
  actor[3].i1=2;
  actor[3].i2=0;
  actor[3].num_moves=3;
}
return 0;
}
//===========================================================================
int bossb_movement(ACTOR *actr){    //boss - skull - shake
int rep,an,hx,hy,d;
char su[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

if(hammer->used && hammer->move!=5){
  hx=hammer->x;
  hy=hammer->y;
  for(rep=7;rep<15;rep++){
    if(!actor[rep].used) continue;
    if(overlap(hx+1,hy+1,hx+10,hy+10,actor[rep].x,actor[rep].y,
               actor[rep].x+actor[rep].size_x-1,actor[rep].y+actor[rep].size_y-1)){
      hammer->move=5;
      d=reverse_direction(hammer);
      hammer->dir=d;
      break;
    }
  }
}
if(actr->i4){
  actr->i4--;
  if(!actr->i4) thunder_flag=0;
}
if(!actr->i2){
  if(actr->x<144) actr->x+=2;
  else if(actr->x>144) actr->x-=2;
  else{
    actr->i2=1;
    actr->i3=0;
  }
  goto done;
}
if(actor[4].num_shots) goto done;

if(!actr->i3){
  actr->i3=rnd(2)+2;
}

if(actr->i3==2) actr->x-=2;
else actr->x+=2;

if(actr->x<20 || actr->x>270){
  thunder_flag=100;
  actr->i4=50;
  play_sound(EXPLODE,1);
  actr->i2=0;
  hx=thor->x;
  memset(su,0,18);
  actor_always_shoots(&actor[4],1);
  an=actor[4].shot_actor;
  hx=(thor->x/16);
  actor[an].x=thor->x;  //hx*16;
  actor[an].y=rnd(16);
  su[hx]=1;
  actor[an].next=rnd(4);
  REPEAT(num_spikes){
    while(1){
      hx=rnd(18);
      if(!su[hx]) break;
    }
    su[hx]=1;
    actor_always_shoots(&actor[4],1);
    an=actor[4].shot_actor;
    actor[an].next=rnd(4);
    actor[an].x=16+hx*16;
    actor[an].y=rnd(16);
  }
}

done:
next_frame(actr);
boss_set(actr->dir,actr->x,actr->y);
return 0;
}

