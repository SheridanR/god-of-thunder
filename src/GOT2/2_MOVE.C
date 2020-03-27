// THOR - The God of Thunder
//Source code released to the public domain on March 27th, 2020.

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>

#include <2_define.h>
#include <2_proto.h>
//===========================================================================
extern unsigned int draw_page,display_page;
extern char pge;
extern int exit_flag;
extern volatile char key_flag[100];
extern char break_code;
extern char scan_code;
extern int new_level,current_level;
extern LEVEL scrn;
extern char *scrnp;

extern char far *sd_data;
extern char far *bg_pics;
extern char diag;
extern ACTOR *thor;
extern int thor_x1,thor_y1,thor_x2,thor_y2;
extern ACTOR *hammer;
extern ACTOR actor[MAX_ACTORS];
extern ACTOR enemy[MAX_ENEMIES];  //current enemies
extern ACTOR shot[MAX_ENEMIES];   //current shots
extern char enemy_type[MAX_ENEMIES];

extern ACTOR explosion;
extern ACTOR sparkle;
extern char shot_ok;
extern char far *std_sound_start;
extern int max_shot;
extern THOR_INFO thor_info;
extern char object_map[240];
extern int hourglass_flag,thunder_flag,shield_on,lightning_used,tornado_used;
extern char cheat;
extern int killgg_inform;
extern SETUP setup;
extern int thunder_flag;

extern int (*movement_func[]) (ACTOR *actr);
extern int (*shot_movement_func[]) (ACTOR *actr);
extern int (*shot_pattern_func[]) (ACTOR *actr);
//===========================================================================
void next_frame(ACTOR *actr){

actr->frame_count--;
if(actr->frame_count<=0){
  actr->next++;
//  if(actr->next >= actr->frames) actr->next=0;
  if(actr->next>3) actr->next=0;
  actr->frame_count=actr->frame_speed;
}
}
/*=========================================================================*/
int point_within(int x,int y,int x1,int y1,int x2,int y2){

if((x>=x1) && (x<=x2) && (y>=y1) && (y<=y2)) return 1;
return 0;
}
//===========================================================================
int  overlap(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4){

if((x1>=x3) && (x1<=x4) && (y1>=y3) && (y1<=y4)) return 1;
if((x2>=x3) && (x2<=x4) && (y2>=y3) && (y2<=y4)) return 1;
if((x1>=x3) && (x1<=x4) && (y2>=y3) && (y2<=y4)) return 1;
if((x2>=x3) && (x2<=x4) && (y1>=y3) && (y1<=y4)) return 1;
if((x3>=x1) && (x3<=x2) && (y3>=y1) && (y3<=y2)) return 1;
if((x4>=x1) && (x4<=x2) && (y4>=y1) && (y4<=y2)) return 1;
if((x3>=x1) && (x3<=x2) && (y4>=y1) && (y4<=y2)) return 1;
if((x4>=x1) && (x4<=x2) && (y3>=y1) && (y3<=y2)) return 1;
return 0;
}
//===========================================================================
int reverse_direction(ACTOR *actr){

if(actr->dir==1) return 0;
if(actr->dir==2) return 3;
if(actr->dir==3) return 2;
return 1;
}
//===========================================================================
void thor_shoots(void){

if((hammer->used!=1) && (!hammer->dead) && (!thor->shot_cnt)){
  play_sound(SWISH,0);
  thor->shot_cnt=20;
  hammer->used=1;
  hammer->dir=thor->dir;
  hammer->last_dir=thor->dir;
  hammer->x=thor->x;
  hammer->y=thor->y+2;
  hammer->move=2;
  hammer->next=0;
  hammer->last_x[0]=hammer->x;
  hammer->last_x[1]=hammer->x;
  hammer->last_y[0]=hammer->y;
  hammer->last_y[1]=hammer->y;
}
}
//===========================================================================
int kill_good_guy(void){

if(!killgg_inform && !thunder_flag){
    odin_speaks(2010,0);
    killgg_inform=1;
}
add_score(-1000);
return 0;
}
//===========================================================================
void actor_damaged(ACTOR *actr,int damage){
int d;

if(!thunder_flag && !lightning_used && !tornado_used){
  if(!setup.skill) damage*=2;
  else if(setup.skill==2) damage/=2;
  d=damage/4;  //for armor
  damage+=d;
}

if(!actr->vunerable && actr->type!=3 && (actr->solid & 0x7f) !=2){
  actr->vunerable=STAMINA;
  if(damage >= actr->health){
    if(actr->type!=4){
      add_score(actr->init_health*10);
      display_score();
    }
    else kill_good_guy();
    actor_destroyed(actr);
  }
  else{
    actr->show=10;
    actr->health-=damage;
    actr->speed_count+=8;
  }
}
else if(!actr->vunerable){
  actr->vunerable=STAMINA;
  if(actr->func_num==4){
    switch_icons();
  }
  if(actr->func_num==7){
    rotate_arrows();
  }
}

}
//===========================================================================
void thor_damaged(ACTOR *actr){
int damage,t,d;

actr->hit_thor=1;
if(cheat) if(key_flag[_FOUR]) return;
if(actr->func_num==11){
  if(actr->talk_counter){
    actr->talk_counter--;
    return;
  }
  t=actr->type;
  actr->type=4;
  actor_speaks(actr,0,0);
  actr->type=t;
  actr->talk_counter=30;
  return;
}
damage=actr->strength;

if(damage<150){
  if(!setup.skill) damage/=2;
  else if(setup.skill==2) damage*=2;
  d=damage/4;
  damage-=d;
}
if(damage==150 && shield_on) return;
if((!thor->vunerable && !shield_on) || damage==255 ){
  if(damage >= thor->health){
    thor->vunerable=40;
    thor->show=0;
    thor->health=0;
    display_health();
    exit_flag=2;
  }
  else if(damage){
    thor->vunerable=40;
    play_sound(OW,0);
    thor->show=10;
    thor->health-=damage;
    display_health();
  }
}
}
//===========================================================================
void actor_destroyed(ACTOR *actr){
int x,y,x1,y1,r,n,t;

if(actr->actor_num>2){
  x=actr->last_x[pge^1];
  y=actr->last_y[pge^1];
  x1=actr->last_x[pge];
  y1=actr->last_y[pge];
  r=actr->rating;
  n=actr->actor_num;
  t=actr->type;
  if(actr->func_num==255) memcpy(actr,&explosion,sizeof(ACTOR));
  else memcpy(actr,&sparkle,sizeof(ACTOR));
  actr->type=t;
  actr->actor_num=n;
  actr->rating=r;
  actr->x=x;
  actr->y=y;
  actr->last_x[pge]=x1;
  actr->last_x[pge^1]=x;
  actr->last_y[pge]=y1;
  actr->last_y[pge^1]=y;
  actr->speed_count=actr->speed;
  actr->used=1;
  actr->num_shots=3;  //used to reverse explosion
  actr->vunerable=255;
}
else{
  actr->dead=2;
  actr->used=0;
}
}
//===========================================================================
int _actor_shoots(ACTOR *actr,int dir){
int t,i,cx,cy;
ACTOR *act;

t=actr->shot_type-1;
for(i=MAX_ENEMIES+3;i<MAX_ACTORS;i++){
   if((!actor[i].used) && (!actor[i].dead)){
     act=&actor[i];
     memcpy(act,&shot[t],sizeof(ACTOR));
     if(actr->size_y<act->size_y) cy=actr->y-((act->size_y-actr->size_y)/2);
     else cy=actr->y+((actr->size_y-act->size_y)/2);
     if(actr->size_x<act->size_x) cx=actr->x-((act->size_x-actr->size_x)/2);
     else cx=actr->x+((actr->size_x-act->size_x)/2);
//     if(actr->size_y<act->size_y) cy=actr->y-(act->size_y-actr->size_y);
//     else cy=actr->y+(actr->size_y-act->size_y);
//     if(actr->size_x<act->size_x) cx=actr->x-(act->size_x-actr->size_x);
//     else cx=actr->x+(actr->size_x-act->size_x);
     if(cy>174) cy=174;
     if(cx>304) cx=304;
     act->x=cx;
     act->y=cy;
     act->last_dir=dir;
     act->next=0;
     act->dir=dir;
     if(act->directions==1) act->dir=0;
     else if(act->directions==4 && act->frames==1){
       act->next=dir;
       act->dir=0;
     }
     act->frame_count=act->frame_speed;
     act->speed_count=act->speed;
     act->last_x[0]=cx;
     act->last_x[1]=cx;
     act->last_x[0]=actr->x;
     act->last_x[1]=actr->x;
     act->last_y[0]=cy;
     act->last_y[1]=cy;
     act->used=1;
     act->creator=actr->actor_num;
     act->move_count=act->num_moves;
     act->dead=0;
     actr->shot_actor=i;
     actr->num_shots++;
     actr->shot_cnt=20;
     shot_ok=0;
     return 1;
   }
}
return 0;
}
//===========================================================================
void actor_always_shoots(ACTOR *actr,int dir){


_actor_shoots(actr,dir);
}
//===========================================================================
int actor_shoots(ACTOR *actr,int dir){
int i,cx,cy,tx,ty;
int icn;

cx=(actr->x+(actr->size_x/2)) >> 4;
cy=((actr->y+actr->size_y)-2) >> 4;

tx=thor->center_x;
ty=thor->center_y;

icn=140;
if(shot[actr->shot_type-1].flying==1) icn=80;

switch(dir){
      case 0:
           for(i=ty+1;i<=cy;i++)
              if(scrn.icon[i][cx]<icn) return 0;
           break;
      case 1:
           for(i=cy;i<=ty;i++)
              if(scrn.icon[i][cx]<icn) return 0;
           break;
      case 2:
           for(i=tx;i<cx;i++)
              if(scrn.icon[cy][i]<icn) return 0;
           break;
      case 3:
           for(i=cx;i<tx;i++)
              if(scrn.icon[cy][i]<icn) return 0;
           break;
}
return _actor_shoots(actr,dir);
}
//===========================================================================
void move_actor(ACTOR *actr){
int i;

if(actr->vunerable!=0) actr->vunerable--;
if(actr->shot_cnt!=0) actr->shot_cnt--;
if(actr->show!=0) actr->show--;

if(!actr->shot_cnt && shot_ok){
  if(actr->shots_allowed){
    if(actr->num_shots < actr->shots_allowed){
      shot_pattern_func[actr->shot_pattern] (actr);
    }
  }
}

actr->speed_count--;
if(actr->speed_count<=0){
  if(!actr->move_counter) actr->speed_count=actr->speed;
  else actr->speed_count=(actr->speed << 1);
  if(actr->type==3) i=shot_movement_func[actr->move] (actr);
  else i=movement_func[actr->move] (actr);
  if(actr->directions==2) i&=1;
  if(i!=actr->dir) actr->dir=i;

  if(actr->move==0 && current_level!=new_level && shield_on){
    actor[2].x=actr->x-2;
    if(actor[2].x<0) actor[2].x=0;
    actor[2].y=actr->y;
    actor[2].last_x[0]=actor[2].x;
    actor[2].last_x[1]=actor[2].x;
    actor[2].last_y[0]=actor[2].y;
    actor[2].last_y[1]=actor[2].y;
  }
}
else i=actr->dir;

actr->x &= 0xfffe;
}


