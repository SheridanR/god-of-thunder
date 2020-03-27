// THOR - The God of Thunder
//Source code released to the public domain on March 27th, 2020.

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>

#include <3_define.h>
#include <3_proto.h>

void check_boss_hit(void);
void boss_change_mode(void);
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
extern int rand1,rand2,exit_flag;
extern char apple_drop,game_over;
extern int boss_intro1,boss_intro2;
extern int endgame;
extern char far *bg_pics;
extern int game_is_over;
extern char cheat;

int  boss_mode;
int  num_pods,num_pods1;
char pod_speed;
char exp[]={ 46,47,48,49,50,51,52,53,
             66,67,68,69,70,71,72,73,
             86,87,88,89,90,91,92,93,
            106,107,108,109,110,111,112,113,
            126,127,128,129,130,131,132,133,
            146,147,148,149,150,151,152,153,
            166,167,168,169,170,171,172,173,
            186,187,188,189,190,191,192,193};

char expf[64];
//===========================================================================
void set_boss(ACTOR *actr){

actor[4].next=actr->next;
actor[5].next=actr->next;
actor[6].next=actr->next;

actor[4].last_dir=actr->dir;
actor[5].last_dir=actr->dir;
actor[6].last_dir=actr->dir;
actor[4].dir=actr->dir;
actor[5].dir=actr->dir;
actor[6].dir=actr->dir;

actor[4].x=actr->x+16;
actor[4].y=actr->y;
actor[5].x=actr->x;
actor[5].y=actr->y+16;
actor[6].x=actr->x+16;
actor[6].y=actr->y+16;
}
//===========================================================================
#define LFC 10
int boss_movement_one(ACTOR *actr){    //boss - loki-2
int rx,ry,i,num_pods;

actr->num_moves=2;
pod_speed=2;
switch(setup.skill){
   case 0:
      num_pods=3;
//      pod_speed=1
      break;
   case 1:
      num_pods=5;
      break;
   case 2:
      num_pods=8;
      break;
}

if(!actr->temp1){   //disappear
  actr->dir=1;
  actr->frame_count=LFC;
  actr->next=0;
  actr->temp1=1;
  actr->i6=1;
  actr->solid|=128;
  actor[4].solid|=128;
  actor[5].solid|=128;
  actor[6].solid|=128;
  play_sound(EXPLODE,1);
  goto done;
}
if(actr->i6){       //fade out
  actr->frame_count--;
  if(actr->frame_count<=0){
    actr->next++;
    if(actr->next>2){
      actr->i6=0;
      actr->temp3=160;
    }
    actr->frame_count=3;
  }
  goto done1;
}
if(actr->temp3>1){
  actr->temp3--;
  goto done1;
}
else if(actr->temp3){
    for(i=0;i<num_pods1;i++) if(actor[19+i].used) goto done1;
    while(1){
      rx=(rand()%256)+16;
      ry=(rand()%144);
      if(!overlap(rx,ry,rx+32,ry+32,thor_x1,thor_y1,thor_x2,thor_y2)) break;
    }
    actr->x=rx;
    actr->y=ry;
    actr->frame_count=LFC;
    actr->temp4=40;
    actr->temp3=0;
    play_sound(EXPLODE,1);
  goto done1;
}
if(actr->temp4){    //fade in
  actr->frame_count--;
  if(actr->frame_count<=0){
    actr->next--;
    if(actr->next>254){
      actr->next=0;
      actr->dir=0;
      actr->temp4=0;
      actr->temp5=80;
      actr->solid&=0x7f;
      actor[4].solid&=0x7f;
      actor[5].solid&=0x7f;
      actor[6].solid&=0x7f;
    }
    actr->frame_count=3;
  }
  goto done1;
}
if(actr->temp5){     //shoot
  actr->temp5--;
  if(actr->temp5==20){
    actr->next=3;
    goto done1;
  }
  if(!actr->temp5){
    if(actor[4].num_shots < actor[4].shots_allowed){
      actor_always_shoots(&actor[4],0);
      actor[actor[4].shot_actor].num_moves=pod_speed;
      actor[actor[4].shot_actor].x=actr->x+8;
      actor[actor[4].shot_actor].y=actr->y+16;
      actor[actor[4].shot_actor].temp5=0;
      for(i=0;i<num_pods;i++) memcpy(&actor[20+i],&actor[19],256);
      num_pods1=num_pods;
      actr->temp1=0;
    }
  }
  if(actr->temp5<31) goto done1;
}

done:
actr->frame_count--;
if(actr->frame_count<=0){
  actr->next++;
  if(actr->next>2) actr->next=0;
  actr->frame_count=LFC;
}
done1:
set_boss(actr);
return actr->dir;
}
//===========================================================================
int boss_movement(ACTOR *actr){    //boss - loki-1
int d,x1,y1,f,ox,oy;

if(actr->temp2) actr->temp2--;
if(boss_dead) return boss_die();
check_boss_hit();

if(!boss_mode) return boss_movement_one(actr);
num_pods1=10;
switch(setup.skill){
   case 0:
      actr->num_moves=3;
      actr->speed=2;
      break;
   case 1:
      actr->num_moves=2;
      actr->speed=1;
      break;
   case 2:
      actr->num_moves=5;
      actr->speed=2;
      break;
}

d=actr->last_dir;
actr->temp3++;

f=0;
if(actr->temp4){
  actr->temp4--;
  if(!actr->temp4){
    actr->temp3=0;
    actor[3].frame_speed=4;
    actor[3].dir=0;
    actor[3].last_dir=0;
    actor[3].next=3;
    actor[4].dir=0;
    actor[4].last_dir=0;
    actor[4].next=3;
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
actr->frame_count--;
if(actr->frame_count<=0){
  actr->next++;
  if(actr->next>2) actr->next=0;
  actr->frame_count=30;
}

skip_move:

set_boss(actr);
if(!f) goto done;

new_dir:
if(actr->temp3<120) goto new_dir1;
actor[3].frame_speed=8;
actor[3].next=3;
actor[4].next=3;
actr->temp4=120;
actor_always_shoots(actr,0);
actor[actr->shot_actor].x=actr->x+8;
actor[actr->shot_actor].y=actr->y-8;
actor[actr->shot_actor].temp1=rnd(100)+90;
actor[actr->shot_actor].temp5=30;
actor[actr->shot_actor].speed=2;
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

if(actor[3].solid & 128){
 for(rep=3;rep<7;rep++) actor[rep].magic_hit=0;
 return;
}
if(actor[3].magic_hit || actor[4].magic_hit ||
   actor[5].magic_hit || actor[6].magic_hit){
  if(!actor[3].temp2){
    actor_damaged(&actor[3],10);
    if(cheat && key_flag[_Z]) actor[3].health-=50;
    else actor[3].health-=10;
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
    if(actor[3].health==50){
      boss_change_mode();
      actor[3].temp1=0;
      actor[3].temp2=0;
      actor[3].temp3=0;
      actor[3].temp4=0;
      actor[3].temp5=0;
      actor[3].i6=0;
      actor[3].speed_count=2;
    }
    else actor[3].temp2=40;
  }
  for(rep=3;rep<7;rep++) actor[rep].magic_hit=0;
}
}
//===========================================================================
void boss_change_mode(void){
char *pic;

if(!boss_intro2){
   pic=malloc(1048);
  if(pic){
    res_read("FACE18",(char far*)pic);
    execute_script(1003,pic);
    d_restore();
    free(pic);
    boss_intro2=1;
  }
}
boss_mode=0;
}
//===========================================================================
void boss_level(void){
char *pic;

setup_boss(3);
boss_active=1;
boss_status(-1);
music_pause();
play_sound(BOSS11,1);
timer_cnt=0;

while(timer_cnt<180) rotate_pal();

if(!boss_intro1){
  pic=malloc(1048);
  if(pic){
    res_read("FACE18",(char far*)pic);
    execute_script(1002,pic);
    d_restore();
    free(pic);
    boss_intro1=1;
  }
}
music_play(7,1);
apple_drop=0;
boss_mode=1;
}
//===========================================================================
int boss_die(void){
int rep,n,x,y,r,x1,y1;

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

music_play(6,1);
odin_speaks(1001,0);
fill_score(20);
score_for_inv();

fill_health();
fill_magic();

REPEAT(16) scrn.actor_type[rep]=0;
boss_dead=0;
setup.boss_dead[2]=1;
game_over=1;
boss_active=0;
scrn.type=6;
show_level(BOSS_LEVEL);
exit_flag=0;
music_pause();
fade_out();
new_level=106;
thor->x=152;
thor->y=160;
thor->dir=1;
game_is_over=1;
}
//===========================================================================
void ending_screen(void){
int i;

for(i=3;i<MAX_ACTORS;i++) actor[i].move=1;
fade_in();
music_play(6,1);
timer_cnt=0;
while(timer_cnt<180) rotate_pal();
memset(expf,0,64);
endgame=1;
memcpy(&actor[34],&explosion,sizeof(ACTOR));
actor[34].used=0;
actor[34].speed=2;
actor[34].speed_count=actor[34].speed;
actor[34].num_shots=3;  //used to reverse explosion
actor[34].vunerable=255;
actor[34].i2=6;
}
//===========================================================================
int endgame_movement(void){    //explode
int x,y,r;

if(actor[34].i2){
  actor[34].i2--;
  return 0;
}
actor[34].i2=6;
play_sound(EXPLODE,1);

r=rand1%64;
while(expf[r]){
  r++;
  if(r>63) r=0;
}
expf[r]=1;
x=(exp[r]%20)*16;
y=(exp[r]/20)*16;
actor[34].x=x;
actor[34].y=y;
actor[34].used=1;
actor[34].next=0;
actor[34].num_shots=3;

xfput(x,y,display_page,(char far *) (bg_pics+(scrn.bg_color*262)));
xfput(x,y,draw_page,(char far *) (bg_pics+(scrn.bg_color*262)));
xfput(x,y,PAGE2,(char far *) (bg_pics+(scrn.bg_color*262)));
scrn.icon[y/16][x/16]=scrn.bg_color;

endgame++;
if(endgame>64){
  actor[34].used=0;
  endgame=0;
}
return 1;
}
