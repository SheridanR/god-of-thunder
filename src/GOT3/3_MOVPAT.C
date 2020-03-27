// THOR - The God of Thunder
//Source code released to the public domain on March 27th, 2020.

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>

#include <3_define.h>
#include <3_proto.h>

#define TILE_SOLID   80
#define TILE_FLY     140
#define TILE_SPECIAL 200
//===========================================================================
extern volatile unsigned int timer_cnt;
extern int new_level,current_level;
extern LEVEL scrn;
extern ACTOR actor[MAX_ACTORS];
extern ACTOR *thor;
extern int thor_x1,thor_y1,thor_x2,thor_y2,thor_real_y1;
extern int exit_flag;
extern volatile char key_flag[100];
extern char diag;
extern ACTOR *hammer;
extern int key_fire,key_up,key_down,key_left,key_right,key_magic,key_select;
extern int lightning_used,tornado_used,hourglass_flag;
extern int switch_flag;
extern unsigned int display_page,draw_page;
extern char far *bg_pics;
extern char far objects[NUM_OBJECTS][262];
extern char object_map[240];
extern char object_index[240];
extern char thor_icon1,thor_icon2,thor_icon3,thor_icon4;
extern THOR_INFO thor_info;
extern int boss_dead;
extern int boss_active;
extern int rand1,rand2;
extern int thor_x1,thor_y1,thor_x2,thor_y2,thor_real_y1;
extern int bomb_flag,shield_on;
extern ACTOR magic_item[];
extern char cheat;
extern char *options_yesno[];
extern SETUP setup;
extern int thunder_flag;
extern char odin[4][262];
extern char area;
extern int endgame;
char diag_flag;
char thor_special_flag;
int  bomb_x[]={0,-16,32,-32,32,-16,-16, 32,-16};
int  bomb_y[]={0,-16,16,  0,16,-32, 32,-32, 32};
char rotate_pat[]={0,3,1,2};
//===========================================================================
int check_special_move1(int x,int y, ACTOR *actr);

int  check_move0(int x,int y, ACTOR *actr);
int  check_move1(int x,int y, ACTOR *actr);
int  check_move2(int x,int y, ACTOR *actr);
int  check_move3(int x,int y, ACTOR *actr);
int  check_move4(int x,int y, ACTOR *actr);

int movement_zero(ACTOR *actr);
int movement_one(ACTOR *actr);
int movement_two(ACTOR *actr);
int movement_three(ACTOR *actr);
int movement_four(ACTOR *actr);
int movement_five(ACTOR *actr);
int movement_six(ACTOR *actr);
int movement_seven(ACTOR *actr);
int movement_eight(ACTOR *actr);
int movement_nine(ACTOR *actr);
int movement_ten(ACTOR *actr);
int movement_eleven(ACTOR *actr);
int movement_twelve(ACTOR *actr);         //horz straight until bump
int movement_thirteen(ACTOR *actr);       //pause-seek (mushroom)
int movement_fourteen(ACTOR *actr);       //move-bump-stop (boulder)
int movement_fifteen(ACTOR *actr);        //no move, no frame cycle
int movement_sixteen(ACTOR *actr);        //tornado 1
int movement_seventeen(ACTOR *actr);      //tornado 2
int movement_eighteen(ACTOR *actr);       //random-seek-bite-run
int movement_nineteen(ACTOR *actr);       //tornado 2
int movement_twenty(ACTOR *actr);         //tornado 2
int movement_twentyone(ACTOR *actr);      //eyeball
int movement_twentytwo(ACTOR *actr);      //spear
int movement_twentythree(ACTOR *actr);    //spinball cw
int movement_twentyfour(ACTOR *actr);     //spinball ccw
int movement_twentyfive(ACTOR *actr);     //
int movement_twentysix(ACTOR *actr);      //
int movement_twentyseven(ACTOR *actr);    //
int movement_twentyeight(ACTOR *actr);    //tree boss
int movement_twentynine(ACTOR *actr);     //horz or vert (pass_val)
int movement_thirty(ACTOR *actr);         //vert straight
int movement_thirtyone(ACTOR *actr);      //drop (stalagtite)
int movement_thirtytwo(ACTOR *actr);      //bomb 1
int movement_thirtythree(ACTOR *actr);    //bomb 2
int movement_thirtyfour(ACTOR *actr);     //gun (4-dir)
int movement_thirtyfive(ACTOR *actr);     //gun (4-dir)
int movement_thirtysix(ACTOR *actr);      //acid drop
int movement_thirtyseven(ACTOR *actr);    //4-way rnd,rnd len
int movement_thirtyeight(ACTOR *actr);    //timed darting
int movement_thirtynine(ACTOR *actr);    //troll 1
int movement_forty(ACTOR *actr);         //troll 2

int (*movement_func[]) (ACTOR *actr) =
    { movement_zero,
      movement_one,
      movement_two,
      movement_three,
      movement_four,
      movement_five,
      movement_six,
      movement_seven,
      movement_eight,
      movement_nine,
      movement_ten,
      movement_eleven,
      movement_twelve,
      movement_thirteen,
      movement_fourteen,
      movement_fifteen,
      movement_sixteen,
      movement_seventeen,
      movement_eighteen,
      movement_nineteen,
      movement_twenty,
      movement_twentyone,
      movement_twentytwo,
      movement_twentythree,
      movement_twentyfour,
      movement_twentyfive,
      movement_twentysix,
      movement_twentyseven,
      movement_twentyeight,
      movement_twentynine,
      movement_thirty,
      movement_thirtyone,
      movement_thirtytwo,
      movement_thirtythree,
      movement_thirtyfour,
      movement_thirtyfive,
      movement_thirtysix,
      movement_thirtyseven,
      movement_thirtyeight,
      movement_thirtynine,
      movement_forty,
    };

int special_movement_one(ACTOR *actr);
int special_movement_two(ACTOR *actr);
int special_movement_three(ACTOR *actr);
int special_movement_four(ACTOR *actr);
int special_movement_five(ACTOR *actr);
int special_movement_six(ACTOR *actr);
int special_movement_seven(ACTOR *actr);
int special_movement_eight(ACTOR *actr);
int special_movement_nine(ACTOR *actr);
int special_movement_ten(ACTOR *actr);
int special_movement_eleven(ACTOR *actr);

int (*special_movement_func[]) (ACTOR *actr) =
    { NULL,
      special_movement_one,
      special_movement_two,
      special_movement_three,
      special_movement_four,
      special_movement_five,
      special_movement_six,
      special_movement_seven,
      special_movement_eight,
      special_movement_nine,
      special_movement_ten,
      special_movement_eleven,
    };
//==========================================================================
int check_move0(int x,int y, ACTOR *actr){   //check thor move
int x1,x2,y1,y2;
int x3,x4,y3,y4;
int i,ti;
ACTOR *act;
char icn1,icn2,icn3,icn4;

thor_icon1=0;
thor_icon2=0;
thor_icon3=0;
thor_icon4=0;

if(x<0){
  if(current_level>0){
    new_level=current_level-1;
    actr->x=304;
    actr->last_x[0]=304;
    actr->last_x[1]=304;
    actr->show=0;
    actr->move_count=0;
    set_thor_vars();
    return 1;
  }
  else return 0;
}
if(x>306){
  if(current_level<119){
    new_level=current_level+1;
    actr->x=0;
    actr->last_x[0]=0;
    actr->last_x[1]=0;
    actr->show=0;
    actr->move_count=0;
    set_thor_vars();
    return 1;
  }
  else return 0;
}
if(y<0){
  if(current_level>9){
    new_level=current_level-10;
    actr->y=175;
    actr->last_y[0]=175;
    actr->show=0;
    actr->last_y[1]=175;
    actr->move_count=0;
    set_thor_vars();
    return 1;
  }
  else return 0;
}
if(y>175){
  if(current_level<110){
    new_level=current_level+10;
    actr->y=0;
    actr->last_y[0]=0;
    actr->last_y[1]=0;
    actr->show=0;
    actr->move_count=0;
    set_thor_vars();
    return 1;
  }
  else return 0;
}
x1=(x+1) >> 4;
y1=(y+8) >> 4;
if(thor->dir>1) x2=(x+12) >> 4;
else x2=(x+10) >> 4;
y2=(y+15) >> 4;

//check for solid or fly over

if((cheat+key_flag[_ONE])<2){
  icn1=scrn.icon[y1][x1];
  icn2=scrn.icon[y2][x1];
  icn3=scrn.icon[y1][x2];
  icn4=scrn.icon[y2][x2];
  ti=0;
  if(icn1<TILE_FLY){ thor_icon1=1; ti=1;}
  if(icn2<TILE_FLY){ thor_icon2=1; ti=1;}
  if(icn3<TILE_FLY){ thor_icon3=1; ti=1;}
  if(icn4<TILE_FLY){ thor_icon4=1; ti=1;}
  if(ti) return 0;

  if(icn1>TILE_SPECIAL){
    if(!special_tile_thor(y1,x1,icn1)) return 0;
    icn2=scrn.icon[y2][x1];
    icn3=scrn.icon[y1][x2];
    icn4=scrn.icon[y2][x2];
  }
  if(icn2>TILE_SPECIAL){
    if(!special_tile_thor(y2,x1,icn2)) return 0;
    icn3=scrn.icon[y1][x2];
    icn4=scrn.icon[y2][x2];
  }
  if(icn3>TILE_SPECIAL){
    if(!special_tile_thor(y1,x2,icn3)) return 0;
    icn4=scrn.icon[y2][x2];
  }
  if(icn4>TILE_SPECIAL) if(!special_tile_thor(y2,x2,icn4)) return 0;
}

x1=x+1;
y1=y+8;
if(thor->dir>1) x2=x+12;
else x2=x+12;
y2=y+15;

thor_special_flag=0;
for(i=3;i<MAX_ACTORS;i++){   //max_actors
   act=&actor[i];
   if(act->solid & 128) continue;
   if(!act->used) continue;
   x3=act->x+1;
   if((abs(x3-x1)) > 16) continue;
   y3=act->y+1;
   if((abs(y3-y1)) > 16) continue;
   x4=act->x+act->size_x-1;
   y4=act->y+act->size_y-1;
   if(overlap(x1,y1,x2,y2,x3,y3,x4,y4)){
     if(act->func_num>0){ //255=explosion
       if(act->func_num==255) return 0;
       act->temp1=x;
       act->temp2=y;
       thor_special_flag=1;
       return special_movement_func[act->func_num](act);
     }
     else{
       thor_special_flag=0;
       thor_damaged(act);
       if(act->solid<2){
         if(!act->vunerable && (!(act->type & 1))) play_sound(PUNCH1,0);
         if(!hammer->used && key_flag[key_fire])
           actor_damaged(act,hammer->strength);
         else actor_damaged(act,thor->strength);
       }
     }
     return 1;
   }
}
actr->x=x;
actr->y=y;
return 1;
}
//==========================================================================
int check_move1(int x,int y, ACTOR *actr){   //check hammer move
int  x1,x2,y1,y2,i;
int  x3,y3,x4,y4;
int  icn,f;
char icn1,icn2,icn3,icn4;

ACTOR *act;

if(x<0 || x>306 || y<0 || y>177) return 0;

x1=(x+1) >> 4;
y1=(y+6) >> 4;
x2=(x+10) >> 4;
y2=(y+10) >> 4;

//check for solid or fly over
icn=TILE_FLY;
if(actr->flying) icn=TILE_SOLID;

icn1=scrn.icon[y1][x1];
icn2=scrn.icon[y2][x1];
icn3=scrn.icon[y1][x2];
icn4=scrn.icon[y2][x2];
if(icn1<icn || icn2<icn || icn3<icn ||icn4<icn){
  if(actr->actor_num==1 && actr->move==2) play_sound(CLANG,0);
  return 0;
}

if(icn1>TILE_SPECIAL) if(!special_tile(actr,y1,x1,icn1)) return 0;
if(icn2>TILE_SPECIAL) if(!special_tile(actr,y2,x1,icn2)) return 0;
if(icn3>TILE_SPECIAL) if(!special_tile(actr,y1,x2,icn3)) return 0;
if(icn4>TILE_SPECIAL) if(!special_tile(actr,y2,x2,icn4)) return 0;

x1=x+1;
y1=y+1;
x2=x+10;
y2=y+10;

f=0;
for(i=3;i<MAX_ACTORS;i++){       //max_actors
   act=&actor[i];
   if(!act->used) continue;
   if(boss_active && (act->solid & 128)) continue;
   if(act->type==3) continue;  //continue
   x3=act->x;
   if((abs(x3-x1)) > 16) continue;
   y3=act->y;
   if((abs(y3-y1)) > 16) continue;
   x4=act->x+act->size_x-1;
   y4=act->y+act->size_y-1;
   if(overlap(x1,y1,x2,y2,x3,y3,x4,y4)){
//     if(boss_active){
//       check_boss_hit();
//       break;
//     }
//     else{
       if(act->solid==2 && (actr->move==16 || actr->move==17)) return 0;
       actor_damaged(act,actr->strength);
//       if(act->solid==2 && (actr->move==16 || actr->move==17)) return 0;
//     }
     f++;
   }
}
if(f && actr->move==2) return 0;
actr->x=x;
actr->y=y;
return 1;
}
//==========================================================================
int check_move2(int x,int y, ACTOR *actr){   //check enemy move
int x1,x2,y1,y2,i;
int x3,y3,x4,y4;
int icn;
char icn1,icn2,icn3,icn4;

ACTOR *act;

if(actr->actor_num<3) return check_move1(x,y,actr);

if(x<0 || x>(319-actr->size_x) || y<0 || y>175) return 0;

x1=(x+1) >> 4;
if(!actr->func_num) y1=(y+(actr->size_y/2)) >> 4;
else y1=(y+1) >> 4;
x2=((x+actr->size_x)-1) >> 4;
y2=((y+actr->size_y)-1) >> 4;

//check for solid or fly over

icn=TILE_FLY;
if(actr->flying) icn=TILE_SOLID;


icn1=scrn.icon[y1][x1];
icn2=scrn.icon[y2][x1];
icn3=scrn.icon[y1][x2];
icn4=scrn.icon[y2][x2];
if(icn1<icn || icn2<icn || icn3<icn ||icn4<icn) return 0;

if(icn1>TILE_SPECIAL) if(!special_tile(actr,y1,x1,icn1)) return 0;
if(icn2>TILE_SPECIAL) if(!special_tile(actr,y2,x1,icn2)) return 0;
if(icn3>TILE_SPECIAL) if(!special_tile(actr,y1,x2,icn3)) return 0;
if(icn4>TILE_SPECIAL) if(!special_tile(actr,y2,x2,icn4)) return 0;

x1=x+1;
y1=y+1;
x2=(x+actr->size_x)-1;
y2=(y+actr->size_y)-1;

for(i=0;i<MAX_ACTORS;i++){   //max_actors
   act=&actor[i];
   if(act->actor_num==actr->actor_num) continue;
   if(act->actor_num==1) continue;
   if(!act->used) continue;
   if(act->type==3) continue;   //shot
   if(i==0){
     if(overlap(x1,y1,x2,y2,thor_x1,thor_y1,thor_x2,thor_y2)){
       thor_damaged(actr);
       return 0;
     }
   }
   else{
     x3=act->x;
     if((abs(x3-x1)) > 16) continue;
     y3=act->y;
     if((abs(y3-y1)) > 16) continue;
     x4=act->x+act->size_x;
     y4=act->y+act->size_y;
     if(overlap(x1,y1,x2,y2,x3,y3,x4,y4)){
       if(actr->move==38){
         if(act->func_num==4) switch_flag=1;
         else if(act->func_num==7) switch_flag=2;
       }
       return 0;
     }
   }
}
actr->x=x;
actr->y=y;
return 1;
}
//==========================================================================
int check_move3(int x,int y, ACTOR *actr){   //check enemy shot move
int x1,x2,y1,y2;
int x3,x4,y3,y4,i;
char icn1,icn2,icn3,icn4;
ACTOR *act;

int icn;

if(x<0 || x>(319-actr->size_x) || y<0 || y>175) return 0;

x1=(x+1) >> 4;
y1=(y+(actr->size_y/2)) >> 4;
x2=((x+actr->size_x)-1) >> 4;
y2=((y+actr->size_y)-1) >> 4;

//check for solid or fly over

icn=TILE_FLY;
if(actr->flying) icn=TILE_SOLID;

icn1=scrn.icon[y1][x1];
icn2=scrn.icon[y2][x1];
icn3=scrn.icon[y1][x2];
icn4=scrn.icon[y2][x2];
if(icn1<icn || icn2<icn || icn3<icn ||icn4<icn) return 0;

if(icn1>TILE_SPECIAL) if(!special_tile(actr,y1,x1,icn1)) return 0;
if(icn2>TILE_SPECIAL) if(!special_tile(actr,y2,x1,icn2)) return 0;
if(icn3>TILE_SPECIAL) if(!special_tile(actr,y1,x2,icn3)) return 0;
if(icn4>TILE_SPECIAL) if(!special_tile(actr,y2,x2,icn4)) return 0;

//check for solid or fly over
x1=x+1;
y1=y+1;
x2=(x+actr->size_x)-1;
y2=(y+actr->size_y)-1;

if(overlap(x1,y1,x2,y2,thor_x1,thor_real_y1,thor_x2,thor_y2)){
  thor_damaged(actr);
  return 0;
}
for(i=3;i<MAX_ACTORS;i++){   //max_actors
   if(i==actr->actor_num) continue;
   act=&actor[i];
   if(!act->used) continue;
   if(act->solid<2) continue;
   if(act->type==3) continue;   //shot
   if(act->actor_num==actr->creator) continue;
   x3=act->x;
   if((abs(x3-x1)) > 16) continue;
   y3=act->y;
   if((abs(y3-y1)) > 16) continue;
   x4=x3+15;
   y4=y3+15;
   if(overlap(x1,y1,x2,y2,x3,y3,x4,y4)) return 0;
}
actr->x=x;
actr->y=y;
return 1;
}
//==========================================================================
int check_move4(int x,int y, ACTOR *actr){   //flying enemies

if(x<0 || x>(319-actr->size_x) || y<0 || y>175) return 0;
if(overlap(x,y,x+actr->size_x-1,y+actr->size_y-1,
           thor_x1,thor_y1,thor_x2,thor_y2)){
  if(actr->type==3) thor_damaged(actr);
  return 0;
}
actr->x=x;
actr->y=y;
return 1;
}
//===========================================================================
#define THOR_PAD1 2
#define THOR_PAD2 4
int  check_thor_move(int x,int y, ACTOR *actr){

if(check_move0(x,y,actr)) return 1;
if(diag_flag || thor_special_flag) return 0;
if((thor_icon1+thor_icon2+thor_icon3+thor_icon4) > 1) return 0;
switch(actr->dir){
      case 0:
           if(thor_icon1){
             actr->dir=3;
             if(check_move0(x+THOR_PAD1,y+2,actr)){
               actr->dir=0;
               return 1;
             }
           }
           else if(thor_icon3){
             actr->dir=2;
             if(check_move0(x-THOR_PAD1,y+2,actr)){
               actr->dir=0;
               return 1;
             }
           }
           actr->dir=0;
           break;
      case 1:
           if(thor_icon2){
             actr->dir=3;
             if(check_move0(x+THOR_PAD1,y-2,actr)){
               actr->dir=1;
               return 1;
             }
           }
           else if(thor_icon4){
             actr->dir=2;
             if(check_move0(x-THOR_PAD1,y-2,actr)){
               actr->dir=1;
               return 1;
             }
           }
           actr->dir=1;
           break;
      case 2:
           if(thor_icon1){
             if(check_move0(x+2,y+THOR_PAD1,actr)) return 1;
           }
           else if(thor_icon2){
             if(check_move0(x+2,y-THOR_PAD1,actr)) return 1;
           }
           break;
      case 3:
           if(thor_icon3){
             if(check_move0(x-2,y+THOR_PAD1,actr)) return 1;
           }
           else if(thor_icon4){
             if(check_move0(x-2,y-THOR_PAD1,actr)) return 1;
           }
           break;
}
return 0;
}
//===========================================================================
int movement_zero(ACTOR *actr){       //player control
int d,x,y,od;
d=actr->dir;
od=d;

set_thor_vars();

if(key_flag[key_fire]) thor_shoots();
if(key_flag[key_select]) select_item();
if((hammer->used) && (hammer->move==5)){
  if(overlap(thor_x1,thor_y1,thor_x2,thor_y2,
             hammer->x,hammer->y,hammer->x+13,hammer->y+13)){
    actor_destroyed(hammer);
  }
}
x=actr->x;
y=actr->y;
diag_flag=0;
if(actr->move_counter) actr->move_counter--;
if(key_flag[key_up] && key_flag[key_left]){
    d=2;
    actr->dir=d;
    diag=1;
    diag_flag=1;
    if(check_thor_move(x-2,y-2,actr)){
       next_frame(actr);
       return d;
    }
}
else if(key_flag[key_up] && key_flag[key_right]){
    d=3;
    actr->dir=d;
    diag=2;
    diag_flag=1;
    if(check_thor_move(x+2,y-2,actr)){
      next_frame(actr);
      return d;
    }
}
else if(key_flag[key_down] && key_flag[key_left]){
    d=2;
    actr->dir=d;
    diag=4;
    diag_flag=1;
    if(check_thor_move(x-2,y+2,actr)){
      next_frame(actr);
      return d;
    }
}
else if(key_flag[key_down] && key_flag[key_right]){
    d=3;
    actr->dir=d;
    diag=3;
    diag_flag=1;
    if(check_thor_move(x+2,y+2,actr)){
      next_frame(actr);
      return d;
    }
}
diag=0;
if(key_flag[key_right]){
    if(!key_flag[key_left]){
      d=3;
      actr->dir=d;
      if(check_thor_move(x+2,y,actr)){
        next_frame(actr);
        return d;
      }
    }
}
if(key_flag[key_left]){
    if(!key_flag[key_right]){
      d=2;
      actr->dir=d;
      if(check_thor_move(x-2,y,actr)){
        next_frame(actr);
        return d;
      }
    }
}
if(key_flag[key_down]){
    if(!key_flag[key_up]){
      d=1;
      actr->dir=d;
      if(check_thor_move(x,y+2,actr)){
        next_frame(actr);
        return d;
      }
    }
}
if(key_flag[key_up]){
    if(!key_flag[key_down]){
      d=0;
      actr->dir=d;
      if(check_thor_move(x,y-2,actr)){
        next_frame(actr);
        return d;
      }
    }
}
actr->move_counter=5;
actr->next=0;
actr->dir=od;
return d;
}
//==========================================================================
int check_special_move1(int x,int y, ACTOR *actr){
int x1,x2,y1,y2,i;
int x3,y3,x4,y4;
int icn;
char icn1,icn2,icn3,icn4;

ACTOR *act;

if(actr->actor_num<3) return check_move1(x,y,actr);

if(x<0 || x>304 || y<0 || y>176) return 0;

x1=x >> 4;
y1=y >> 4;
x2=(x+15) >> 4;
y2=(y+15) >> 4;

//check for solid or fly over

icn=TILE_FLY;
if(actr->flying) icn=TILE_SOLID;

icn1=scrn.icon[y1][x1];
icn2=scrn.icon[y2][x1];
icn3=scrn.icon[y1][x2];
icn4=scrn.icon[y2][x2];
if(icn1<icn || icn2<icn || icn3<icn ||icn4<icn) return 0;

if(icn1>TILE_SPECIAL) if(!special_tile(actr,y1,x1,icn1)) return 0;
if(icn2>TILE_SPECIAL) if(!special_tile(actr,y2,x1,icn2)) return 0;
if(icn3>TILE_SPECIAL) if(!special_tile(actr,y1,x2,icn3)) return 0;
if(icn4>TILE_SPECIAL) if(!special_tile(actr,y2,x2,icn4)) return 0;

x1=x;
y1=y;
x2=(x+15);
y2=(y+15);

for(i=3;i<MAX_ACTORS;i++){   //max_actors
   act=&actor[i];
   if(act->actor_num==actr->actor_num) continue;
   if(!act->used) continue;
   if(act->type==3) continue;   //shot
   x3=act->x;
   if((abs(x3-x1)) > 16) continue;
   y3=act->y;
   if((abs(y3-y1)) > 16) continue;
   x4=act->x+act->size_x;
   y4=act->y+15;
   if(overlap(x1,y1,x2,y2,x3,y3,x4,y4)) return 0;
}
for(i=3;i<MAX_ACTORS;i++){   //max_actors
   act=&actor[i];
   if(act->actor_num==actr->actor_num) continue;
   if(!act->used) continue;
   if(act->type==3) continue;   //shot
   x3=act->x;
   if((abs(x3-x1)) > 16) continue;
   y3=act->y;
   if((abs(y3-y1)) > 16) continue;
   x4=act->x+act->size_x;
   y4=act->y+act->size_y;
   if(overlap(thor_x1,thor_y1,thor_x2,thor_y2,x3,y3,x4,y4)) return 0;
}
actr->x=x;
actr->y=y;
return 1;
}
//===========================================================================
//*==========================================================================
//===========================================================================
int special_movement_one(ACTOR *actr){   //block
int d,x1,y1,sd;

if(diag_flag) return 0;
d=thor->dir;
x1=actr->x;
y1=actr->y;
sd=actr->last_dir;
actr->last_dir=d;

switch(d){
      case 0:
           y1-=2;
           if(!check_special_move1(x1,y1,actr)){
             actr->last_dir=sd;
             return 0;
           }
           break;
      case 1:
           y1+=2;
           if(!check_special_move1(x1,y1,actr)){
             actr->last_dir=sd;
             return 0;
           }
           break;
      case 2:
           x1-=2;
           if(!check_special_move1(x1,y1,actr)){
             actr->last_dir=sd;
             return 0;
           }
           break;
      case 3:
           x1+=2;
           if(!check_special_move1(x1,y1,actr)){
             actr->last_dir=sd;
             return 0;
           }
           break;
}
next_frame(actr);
actr->last_dir=d;
return 1;
}
//===========================================================================
int special_movement_two(ACTOR *actr){   //angle
int x1,y1,x2,y2,i;
int x3,y3,x4,y4;
ACTOR *act;

x1=actr->temp1;       //calc thor pos
y1=actr->temp2;
x2=x1+13;
y2=y1+14;

for(i=3;i<MAX_ACTORS;i++){   //max_actors
   act=&actor[i];
   if(actr->actor_num==act->actor_num) continue;
   if(!act->used) continue;
   x3=act->x;
   if((abs(x3-x1)) > 16) continue;
   y3=act->y;
   if((abs(y3-y1)) > 16) continue;
   x4=act->x+act->size_x;
   y4=act->y+act->size_y;
   if(overlap(x1,y1,x2,y2,x3,y3,x4,y4)) return 0;
}
if(!actr->func_pass){
  if(thor->health<150){
    if(!sound_playing()) play_sound(ANGEL,0);
    thor->health+=1;
    display_health();
  }
}
else{
  if(thor_info.magic<150){
    if(!sound_playing()) play_sound(ANGEL,0);
    thor_info.magic+=1;
    display_magic();
  }
}
return 1;
}
//===========================================================================
int special_movement_three(ACTOR *actr){  //yellow globe
long lind;

if(thunder_flag) return 0;
lind=(long)current_level;
lind=lind*1000;
lind+=(long) actr->actor_num;
execute_script(lind,(char *) odin);
return 0;
}
//===========================================================================
int special_movement_four(ACTOR *actr){   //peg switch

if(actr->shot_cnt!=0) return 0;
actr->shot_cnt=30;
actr=actr;
switch_flag=1;
return 0;
}
void kill_enemies(int ix,int iy);
void remove_objects(int iy,int ix);
//===========================================================================
int special_movement_five(ACTOR *actr){    //boulder roll
int d;

d=thor->dir;

if(diag_flag){
  switch(diag){
    case 1:
      if(thor_x1<(actr->x+15)) d=0;
      else d=2;
      break;
    case 2:
      if(thor_x2<actr->x) d=3;
      else d=0;
      break;
    case 3:
      if(thor_x2>(actr->x)) d=1;
      else d=3;
      break;
    case 4:
      if(thor_x1>(actr->x+15)) d=2;
      else d=1;
      break;
  }
}

actr->last_dir=d;
actr->move=14;
return 0;
}
//===========================================================================
int special_movement_six(ACTOR *actr){

thor_damaged(actr);
return 0;
}
//===========================================================================
int special_movement_seven(ACTOR *actr){

if(actr->shot_cnt!=0) return 0;
actr->shot_cnt=30;
actr=actr;
switch_flag=2;
return 0;
}
//===========================================================================
int special_movement_eight(ACTOR *actr){

if(thor->dir<2 || diag_flag) return 0;
actr->last_dir=thor->dir;
actr->move=14;
return 0;
}
//===========================================================================
int special_movement_nine(ACTOR *actr){

if(thor->dir>1 || diag_flag) return 0;
actr->last_dir=thor->dir;
actr->move=14;
return 0;
}
//===========================================================================
int special_movement_ten(ACTOR *actr){

if(actr->talk_counter){
  actr->talk_counter--;
  return 0;
}
if(thunder_flag) return 0;
if(!actr->talk_counter){
  actr->talk_counter=10;
  actor_speaks(actr,0-actr->pass_value,0);
}
return 0;
}
//===========================================================================
int special_movement_eleven(ACTOR *actr){  //red guard
int t;

if(actr->talk_counter){
  actr->talk_counter--;
  return 0;
}
t=actr->type;
actr->type=4;
actor_speaks(actr,0,0);
actr->type=t;
actr->talk_counter=10;
return 0;
}
//===========================================================================
//*==========================================================================
//===========================================================================
int movement_one(ACTOR *actr){    //no movement - frame cycle

next_frame(actr);
return actr->dir;
}
//===========================================================================
int movement_two(ACTOR *actr){     //hammer only
int d,x1,y1;

d=actr->last_dir;
x1=actr->x;
y1=actr->y;

switch(d){
      case 0:
           y1-=2;
           break;
      case 1:
           y1+=2;
           break;
      case 2:
           x1-=2;
           break;
      case 3:
           x1+=2;
           break;
}
if(!check_move2(x1,y1,actr)){
  if(actr->actor_num==1){
    hammer->move=5;
    d=reverse_direction(hammer);
    hammer->dir=d;
  }
  if(actr->actor_num==2){
    actr->used=0;
    actr->dead=2;
    lightning_used=0;
    tornado_used=0;
  }
}
next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_three(ACTOR *actr){         //walk-bump-random turn
int d,x1,y1;

d=actr->last_dir;
x1=actr->x;
y1=actr->y;

switch(d){
      case 0:
           y1-=2;
           if(!check_move2(x1,y1,actr)){
             d=rnd(4);
           }
           break;
      case 1:
           y1+=2;
           if(!check_move2(x1,y1,actr)){
             d=rnd(4);
           }
           break;
      case 2:
           x1-=2;
           if(!check_move2(x1,y1,actr)){
             d=rnd(4);
           }
           break;
      case 3:
           x1+=2;
           if(!check_move2(x1,y1,actr)){
             d=rnd(4);
           }
           break;
}
next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_four(ACTOR *actr){         //simple tracking
int d,x1,y1,f;

d=actr->last_dir;

x1=actr->x;
y1=actr->y;

f=0;
if(x1>thor_x1-1) {x1-=2;d=2;f=1;}
else if(x1<thor_x1-1) {x1+=2;d=3;f=1;}

if(f) f=check_move2(x1,y1,actr);

if(!f){
  if(y1<(thor_real_y1)){
    d=(thor_real_y1)-y1;
    if(d>2) d=2;
    y1+=d;
    d=1;
    f=1;
  }
  else if(y1>(thor_real_y1)){
    d=y1-(thor_real_y1);
    if(d>2) d=2;
    y1-=d;
    d=0;
    f=1;
  }
  if(f) f=check_move2(actr->x,y1,actr);
  if(!f) check_move2(actr->x,actr->y,actr);
}
next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_five(ACTOR *actr){
int x1,y1,xd,yd,d;

x1=actr->x;
y1=actr->y;
xd=yd=0;
d=actr->last_dir;

if(x1>(thor->x+1)) xd=-2;  //+1
else if(x1<(thor->x-1)) xd=2;

if(actr->actor_num==1){
  if(y1<(thor_y1-6)) yd=2;
  else if(y1>(thor_y1-6)) yd=-2;
}
else{
  if(y1<(thor_real_y1-1)) yd=2;
  else if(y1>(thor_real_y1+1)) yd=-2;
}

if(xd && yd){
  if(xd==-2 && yd==-2) d=2;
  else if(xd==-2 && yd==2) d=2;
  else if(xd==2 && yd==-2) d=3;
  else if(xd==2 && yd==2) d=3;
    x1+=xd;
    y1+=yd;
    if(check_move2(x1,y1,actr)){
      next_frame(actr);
      actr->last_dir=d;
      if(actr->directions==1) return 0;
      return d;
    }
}
else{
  if(xd==0 && yd==2) d=1;
  else if(xd==0 && yd==-2) d=0;
  else if(xd==2 && yd==0) d=3;
  else if(xd==-2 && yd==0) d=2;
}
x1=actr->x;
y1=actr->y;
actr->toggle^=1;

if(actr->toggle){
  if(xd){
    x1+=xd;
    if(check_move2(x1,y1,actr)){
      if(xd>0) d=3;
      else d=2;
      next_frame(actr);
      actr->last_dir=d;
      if(actr->directions==1) return 0;
      return d;
    }
    x1=actr->x;
  }
  if(yd){
    y1+=yd;
    if(check_move2(x1,y1,actr)){
      if(yd>0) d=1;
      else d=0;
      next_frame(actr);
      actr->last_dir=d;
      if(actr->directions==1) return 0;
      return d;
    }
  }
  y1=actr->y;
}
else{
  if(yd){
    y1+=yd;
    if(check_move2(x1,y1,actr)){
      if(yd>0) d=1;
      else d=0;
      next_frame(actr);
      actr->last_dir=d;
      if(actr->directions==1) return 0;
      return d;
    }
    y1=actr->y;
  }
  if(xd){
    x1+=xd;
    if(check_move2(x1,y1,actr)){
      if(xd>0) d=3;
      else d=2;
      next_frame(actr);
      actr->last_dir=d;
      if(actr->directions==1) return 0;
      return d;
    }
  }
}
check_move2(actr->x,actr->y,actr);
next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_six(ACTOR *actr){   //explosion only

if(actr->num_shots>0){
  actr->next++;
  if(actr->next>2){
    actr->next=0;
    if(boss_dead) play_sound(EXPLODE,0);
  }
  actr->num_shots--;
}
else{
  actr->dead=2;
  actr->used=0;
  if(!boss_dead && !endgame) if(actr->type==2) drop_object(actr);

}
next_frame(actr);
return 0;
}
//===========================================================================
int movement_seven(ACTOR *actr){         //walk-bump-random turn (pause also)

if(actr->next==0 && actr->frame_count==actr->frame_speed){
  actr->speed_count=12;
  actr->last_dir=rnd(4);
}
return movement_three(actr);
}
//==========================================================================
int movement_eight(ACTOR *actr){         //follow thor

if(thor->x>0) actr->x=thor->x-1;
else actr->x=thor->x;
actr->y=thor->y;
next_frame(actr);
return 0;
}
//===========================================================================
int movement_nine(ACTOR *actr){         //4-way straight (random length) change
int d,x1,y1,f;

d=actr->last_dir;
x1=actr->x;
y1=actr->y;

f=0;
if(actr->counter){
  actr->counter--;
  switch(d){
        case 0:
             y1-=2;
             if(!check_move4(x1,y1,actr)) f=1;
             break;
        case 1:
             y1+=2;
             if(!check_move4(x1,y1,actr)) f=1;
             break;
        case 2:
             x1-=2;
             if(!check_move4(x1,y1,actr)) f=1;
             break;
        case 3:
             x1+=2;
             if(!check_move4(x1,y1,actr)) f=1;
             break;
  }
}
else f=1;

if(f==1){
  actr->counter=rnd(90)+10;
  d=rnd(4);
}
next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_ten(ACTOR *actr){         //vert straight (random length) change
int d,x1,y1,f;

d=actr->last_dir;
x1=actr->x;
y1=actr->y;

f=0;
if(actr->counter){
  if(actr->pass_value!=1) actr->counter--;
  switch(d){
        case 0:
             y1-=2;
             if(!check_move2(x1,y1,actr)) f=1;
             break;
        case 1:
             y1+=2;
             if(!check_move2(x1,y1,actr)) f=1;
             break;
        case 2:
             y1-=2;
             if(!check_move2(x1,y1,actr)) f=1;
             break;
        case 3:
             y1+=2;
             if(!check_move2(x1,y1,actr)) f=1;
             break;
  }
}
else f=1;

if(f==1){
  actr->counter=rnd(90)+10;
  d=rnd(2);
}
if(d>1) d-=2;
next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_eleven(ACTOR *actr){         //horz only (bats)
int d;

d=actr->last_dir;

  switch(d){
        case 0:
             if(check_move2(actr->x-2,actr->y-2,actr)) break;
             d=1;
             if(check_move2(actr->x-2,actr->y+2,actr)) break;
             d=2;
             break;
        case 1:
             if(check_move2(actr->x-2,actr->y+2,actr)) break;
             d=0;
             if(check_move2(actr->x-2,actr->y-2,actr)) break;
             d=3;
             break;
        case 2:
             if(check_move2(actr->x+2,actr->y-2,actr)) break;
             d=3;
             if(check_move2(actr->x+2,actr->y+2,actr)) break;
             d=0;
             break;
        case 3:
             if(check_move2(actr->x+2,actr->y+2,actr)) break;
             d=2;
             if(check_move2(actr->x+2,actr->y-2,actr)) break;
             d=1;
             break;
  }

next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_twelve(ACTOR *actr){         //horz straight until bump
int d;

d=actr->last_dir;

switch(d){
      case 0:
      case 2:
           if(check_move2(actr->x-2,actr->y,actr)) break;
           d=3;
           break;
      case 1:
      case 3:
           if(check_move2(actr->x+2,actr->y,actr)) break;
           d=2;
           break;
}
next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_thirteen(ACTOR *actr){         //pause-seek (mushroom)
int d;

d=actr->last_dir;

if(actr->counter==0 && actr->pause==0){
  actr->pause=60;
  return d;
}
if(actr->pause>0){
  actr->pause--;
  if(!actr->pause) actr->counter=60;
  actr->vunerable=5;
  actr->strength=0;
  return d;
}
if(actr->counter>0){
  actr->counter--;
  if(!actr->counter) actr->pause=60;
  actr->strength=10;
  return movement_five(actr);
}
return d;
}
//===========================================================================
int movement_fourteen(ACTOR *actr){         //move-bump-stop (boulder)
int d,x1,y1;

d=actr->last_dir;
actr->dir=d;
x1=actr->x;
y1=actr->y;

switch(d){
      case 0:
           y1-=2;
           if(!check_move2(x1,y1,actr)){
             actr->move=15;
             return 0;
           }
           break;
      case 1:
           y1+=2;
           if(!check_move2(x1,y1,actr)){
             actr->move=15;
             return 0;
           }
           break;
      case 2:
           x1-=2;
           if(!check_move2(x1,y1,actr)){
             actr->move=15;
             return 0;
           }
           break;
      case 3:
           x1+=2;
           if(!check_move2(x1,y1,actr)){
             actr->move=15;
             return 0;
           }
           break;
}
next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_fifteen(ACTOR *actr){    //no movement - no frame cycle

return actr->dir;
}
//===========================================================================
int movement_sixteen(ACTOR *actr){     //tornado 1
int d,x1,y1;

d=actr->last_dir;
x1=actr->x;
y1=actr->y;

switch(d){
      case 0:
           y1-=2;
           break;
      case 1:
           y1+=2;
           break;
      case 2:
           x1-=2;
           break;
      case 3:
           x1+=2;
           break;
}
if(!check_move1(x1,y1,actr)){
  actr->move=17;
  d=rnd(4);
}

next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_seventeen(ACTOR *actr){         //tornado 2
int d;

d=actr->last_dir;

  switch(d){
        case 0:
             if(check_move1(actr->x-2,actr->y-2,actr)) break;
             d=1;
             if(check_move1(actr->x-2,actr->y+2,actr)) break;
             d=2;
             break;
        case 1:
             if(check_move1(actr->x-2,actr->y+2,actr)) break;
             d=0;
             if(check_move1(actr->x-2,actr->y-2,actr)) break;
             d=3;
             break;
        case 2:
             if(check_move1(actr->x+2,actr->y-2,actr)) break;
             d=3;
             if(check_move1(actr->x+2,actr->y+2,actr)) break;
             d=0;
             break;
        case 3:
             if(check_move1(actr->x+2,actr->y+2,actr)) break;
             d=2;
             if(check_move1(actr->x+2,actr->y-2,actr)) break;
             d=1;
             break;
  }
next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_eighteen(ACTOR *actr){    //no movement - frame cycle
int d;

if(actr->temp5){
  actr->temp5--;
  if(!actr->temp5) actr->num_moves=1;
}
if(actr->temp1){
  d=movement_five(actr);
  actr->rand--;
  if(actr->hit_thor || !actr->rand){
    if(actr->hit_thor){
      actr->temp5=50;
      actr->num_moves=2;
      actr->hit_thor=0;
      actr->dir=d;
      d=reverse_direction(actr);
    }
    actr->temp1=0;
    actr->rand=rnd(100)+50;
  }
}
else{
  d=movement_three(actr);
  actr->rand--;
  if(!actr->rand){
    actr->temp5=0;
    actr->temp1=1;
    actr->rand=rnd(100)+50;
  }
  if(actr->hit_thor){
    actr->temp5=50;
    actr->num_moves=2;
    actr->hit_thor=0;
    actr->dir=d;
    d=reverse_direction(actr);
  }
}
next_frame(actr);
return d;
}
//===========================================================================
int movement_nineteen(ACTOR *actr){    //no movement - frame cycle

return movement_seven(actr);
}
//===========================================================================
int movement_twenty(ACTOR *actr){    //

return movement_one(actr);
}
//===========================================================================
int movement_twentyone(ACTOR *actr){    //no movement - frame cycle

return movement_three(actr);
}
//===========================================================================
int movement_twentytwo(ACTOR *actr){    //spear
int d;

d=actr->last_dir;
if(actr->directions==1) d=0;

redo:

switch(actr->temp2){
  case 0:
    if(bgtile(actr->x,actr->y)>=TILE_SOLID) actr->next=1;
    else{
      actr->temp2=6;
      actr->temp1=1;
      goto redo;
    }
    actr->temp2++;
    break;
  case 1:
    actr->next=2;
    actr->temp2++;
    break;
  case 2:
    actr->next=3;
    actr->strength=255;
    actr->temp2++;
    actr->temp1=10;
    break;
  case 3:
    check_move2(actr->x,actr->y,actr);
    actr->temp1--;
    if(actr->temp1) break;
    actr->temp2++;
    actr->next=2;
    break;
  case 4:
    actr->strength=0;
    actr->temp2++;
    actr->next=1;
    break;
  case 5:
    actr->temp2++;
    actr->next=0;
    actr->temp1=10;
    break;
  case 6:
    actr->temp1--;
    if(actr->temp1) break;
    actr->temp2=0;
    actr->next=0;
    switch(d){
      case 0:
        actr->x+=16;
        actr->y+=16;
        d=3;
        if(bgtile(actr->x,actr->y)<TILE_SOLID) goto redo;
        break;
      case 1:
        actr->x-=16;
        actr->y-=16;
        d=2;
        if(bgtile(actr->x,actr->y)<TILE_SOLID) goto redo;
        break;
      case 2:
        actr->x+=16;
        actr->y-=16;
        d=0;
        if(bgtile(actr->x,actr->y)<TILE_SOLID) goto redo;
        break;
      case 3:
        actr->x-=16;
        actr->y+=16;
        d=1;
        if(bgtile(actr->x,actr->y)<TILE_SOLID) goto redo;
        break;
    }
    actr->dir=d;
    actr->last_dir=d;
    break;
}
return d;
}
//===========================================================================
int movement_twentythree(ACTOR *actr){    //spinball counter-clockwise
int d;

d=actr->last_dir;
next_frame(actr);
if(actr->pass_value & 2) actr->num_moves=2;

switch(d){
    case 0:
      if(bgtile(actr->x-2,actr->y)>=TILE_FLY &&
        bgtile(actr->x-2,actr->y+actr->size_y-1)>=TILE_FLY){
        d=2;
        actr->x-=2;
      }
      else{
        if(bgtile(actr->x,actr->y-2)<TILE_FLY ||
           bgtile(actr->x+actr->size_x-1,actr->y-2)<TILE_FLY){
          if(bgtile(actr->x+actr->size_x+1,actr->y)>=TILE_FLY &&
            bgtile(actr->x+actr->size_x+1,actr->y+actr->size_y-1)>=TILE_FLY){
            d=3;
            actr->x+=2;
          }
          else{
            d=1;
            break;
          }
        }
        else actr->y-=2;
      }
      break;
    case 1:
      if(bgtile(actr->x+actr->size_x+1,actr->y)>=TILE_FLY &&
         bgtile(actr->x+actr->size_x+1,actr->y+actr->size_y-1)>=TILE_FLY){
        d=3;
        actr->x+=2;
      }
      else{
        if(bgtile(actr->x,actr->y+actr->size_y+1)<TILE_FLY ||
          bgtile(actr->x+actr->size_x-1,actr->y+actr->size_y+1)<TILE_FLY){
          if(bgtile(actr->x-2,actr->y)>=TILE_FLY &&
            bgtile(actr->x-2,actr->y+actr->size_y-1)>=TILE_FLY){
            d=2;
            actr->x-=2;
          }
          else{
            d=0;
            break;
          }
        }
        else actr->y+=2;
      }
      break;
    case 2:
      if(bgtile(actr->x,actr->y+actr->size_y+1)>=TILE_FLY &&
         bgtile(actr->x+actr->size_x-1,actr->y+actr->size_y+1)>=TILE_FLY){
        d=1;
        actr->y+=2;
      }
      else{
        if(bgtile(actr->x-2,actr->y)<TILE_FLY ||
          bgtile(actr->x-2,actr->y+actr->size_y-1)<TILE_FLY){
          if(bgtile(actr->x,actr->y-2)>=TILE_FLY &&
            bgtile(actr->x+actr->size_x-1,actr->y-2)>=TILE_FLY){
            d=0;
            actr->y-=2;
          }
          else{
            d=3;
            break;
          }
        }
        else actr->x-=2;
      }
      break;
    case 3:
      if(bgtile(actr->x,actr->y-2)>=TILE_FLY &&
         bgtile(actr->x+actr->size_x-1,actr->y-2)>=TILE_FLY){
        d=0;
        actr->y-=2;
      }
      else{
        if(bgtile(actr->x+actr->size_x+1,actr->y)<TILE_FLY ||
          bgtile(actr->x+actr->size_x+1,actr->y+actr->size_y-1)<TILE_FLY){
          if(bgtile(actr->x,actr->y+actr->size_y+1)>=TILE_FLY &&
            bgtile(actr->x+actr->size_x-1,actr->y+actr->size_y+1)>=TILE_FLY){
            d=1;
            actr->y+=2;
          }
          else{
            d=2;
            break;
          }
        }
        else actr->x+=2;
      }
      break;
}
check_move2(actr->x,actr->y,actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_twentyfour(ACTOR *actr){    //spinball  clockwise
int d;

d=actr->last_dir;
next_frame(actr);
if(actr->pass_value & 2) actr->num_moves=2;

switch(d){
    case 0:
      if(bgtile(actr->x+actr->size_x+1,actr->y)>=TILE_FLY &&
         bgtile(actr->x+actr->size_x+1,actr->y+actr->size_y-1)>=TILE_FLY){
        d=3;
        actr->x+=2;
      }
      else{
        if(bgtile(actr->x,actr->y-2)<TILE_FLY ||
           bgtile(actr->x+actr->size_x-1,actr->y-2)<TILE_FLY){
          if(bgtile(actr->x-2,actr->y)>=TILE_FLY &&
            bgtile(actr->x-2,actr->y+actr->size_y-1)>=TILE_FLY){
            d=2;
            actr->x-=2;
          }
          else{
            d=1;
            break;
          }
        }
        else actr->y-=2;
      }
      break;
    case 1:
      if(bgtile(actr->x-2,actr->y)>=TILE_FLY &&
         bgtile(actr->x-2,actr->y+actr->size_y-1)>=TILE_FLY){
        d=2;
        actr->x-=2;
      }
      else{
        if(bgtile(actr->x,actr->y+actr->size_y+1)<TILE_FLY ||
          bgtile(actr->x+actr->size_x-1,actr->y+actr->size_y+1)<TILE_FLY){
          if(bgtile(actr->x+actr->size_x+1,actr->y)>=TILE_FLY &&
            bgtile(actr->x+actr->size_x+1,actr->y+actr->size_y-1)>=TILE_FLY){
            d=3;
            actr->x+=2;
          }
          else{
            d=0;
            break;
          }
        }
        else actr->y+=2;
      }
      break;
    case 2:
      if(bgtile(actr->x,actr->y-2)>=TILE_FLY &&
         bgtile(actr->x+actr->size_x-1,actr->y-2)>=TILE_FLY){
        d=0;
        actr->y-=2;
      }
      else{
        if(bgtile(actr->x-2,actr->y)<TILE_FLY ||
          bgtile(actr->x-2,actr->y+actr->size_y-1)<TILE_FLY){
          if(bgtile(actr->x,actr->y+actr->size_y+1)>=TILE_FLY &&
            bgtile(actr->x+actr->size_x-1,actr->y+actr->size_y+1)>=TILE_FLY){
            d=1;
            actr->y+=2;
          }
          else{
            d=3;
            break;
          }
        }
        else actr->x-=2;
      }
      break;
    case 3:
      if(bgtile(actr->x,actr->y+actr->size_y+1)>=TILE_FLY &&
         bgtile(actr->x+actr->size_x-1,actr->y+actr->size_y+1)>=TILE_FLY){
        d=1;
        actr->y+=2;
      }
      else{
        if(bgtile(actr->x+actr->size_x+1,actr->y)<TILE_FLY ||
          bgtile(actr->x+actr->size_x+1,actr->y+actr->size_y-1)<TILE_FLY){
          if(bgtile(actr->x,actr->y-2)>=TILE_FLY &&
            bgtile(actr->x+actr->size_x-1,actr->y-2)>=TILE_FLY){
            d=0;
            actr->y-=2;
          }
          else{
            d=2;
            break;
          }
        }
        else actr->x+=2;
      }
      break;
}
check_move2(actr->x,actr->y,actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_twentyfive(ACTOR *actr){         //acid puddle
int i,ret;

if(actr->temp2){
  actr->temp2--;
  return movement_one(actr);
}
if(!actr->temp1){
//  actr->temp2=12;
  actr->last_dir=rnd(4);
  i=4;
  while(i--){
    ret=movement_three(actr);
    if(ret) return ret;
    actr->last_dir++;
    if(actr->last_dir>3) actr->last_dir=0;
  }
  actr->temp1=16;
}
actr->temp1--;
return movement_three(actr);
}
//===========================================================================
int movement_twentysix(ACTOR *actr){         //loki

return boss_movement(actr);
}
//===========================================================================
int movement_twentyseven(ACTOR *actr){         //

return movement_one(actr);
}
//===========================================================================
void set_thor_vars(void){

thor_x1=thor->x+1;
thor_y1=thor->y+8;
thor_real_y1=thor->y;
if(thor->dir>1) thor_x2=(thor->x+12);
else thor_x2=(thor->x+12);
thor_y2=thor->y+15;
}
//===========================================================================
int movement_twentyeight(ACTOR *actr){         //fish
int d,x1,y1,ret;

d=actr->last_dir;
x1=actr->x;
y1=actr->y;

//actr->solid=2;
if(actr->i1) actr->i1--;
else{
  if(!actr->temp3){
    if(!actr->next){
      actr->frame_count=1;
      actr->frame_speed=4;
    }
//    else actr->solid=1;
    next_frame(actr);
    if(actr->next==3){
      if(actr->num_shots < actr->shots_allowed) actor_shoots(actr,0);
      actr->temp3=1;
    }
  }
  else{
    actr->frame_count--;
    if(actr->frame_count<=0){
      actr->next--;
      actr->frame_count=actr->frame_speed;
      if(!actr->next){
        actr->temp3=0;
        actr->frame_speed=4;
        actr->i1=rnd(100)+60;
      }
//      else actr->solid=1;
    }
  }
  goto done;
}
switch(actr->temp2){
  case 0:
    y1-=2;
    break;
  case 1:
    y1+=2;
    break;
  case 2:
    x1-=2;
    break;
  case 3:
    x1+=2;
    break;
}
ret=bgtile(x1,y1);
if(ret!=100 && ret!=106 && ret!=110 && ret!=111 && ret!=113) goto chg_dir;
ret=bgtile((x1+actr->size_x)-1,y1);
if(ret!=100 && ret!=106 && ret!=110 && ret!=111 && ret!=113) goto chg_dir;
ret=bgtile(x1,(y1+actr->size_y)-1);
if(ret!=100 && ret!=106 && ret!=110 && ret!=111 && ret!=113) goto chg_dir;
ret=bgtile((x1+actr->size_x)-1,(y1+actr->size_y)-1);
if(ret!=100 && ret!=106 && ret!=110 && ret!=111 && ret!=113) goto chg_dir;

actr->x=x1;
actr->y=y1;

goto done;

chg_dir:
actr->temp2=rand1%4;

done:
if(actr->next){
  x1=actr->x;
  y1=actr->y;
  actr->solid=1;
  check_move2(x1,y1,actr);
  actr->x=x1;
  actr->y=y1;
}
else actr->solid=2;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_twentynine(ACTOR *actr){  //horz or vert (pass_val)

//if(actr->actor_num==3) printt(actr->last_dir);
if(!actr->pass_value) return movement_thirty(actr);
else return movement_twelve(actr);
}
//===========================================================================
int movement_thirty(ACTOR *actr){         //vert straight
int d,x1,y1;

d=actr->last_dir;
x1=actr->x;
y1=actr->y;

switch(d){
      case 0:
           y1-=2;
           break;
      case 1:
           y1+=2;
           break;
}
if(!check_move2(x1,y1,actr)) d^=1;

next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_thirtyone(ACTOR *actr){         //drop (stalagtite)
int d,x1,y1,cx,cy,ty,i;

d=actr->last_dir;
x1=actr->x;
y1=actr->y;

if(actr->temp1){
  y1+=2;
  if(!check_move2(x1,y1,actr)) actor_destroyed(actr);
}
else{
  if(thor_y1>y1){
    if(abs(x1-thor_x1)<16){
      cx=(actr->x+(actr->size_x/2)) >> 4;
      cy=((actr->y+actr->size_y)-2) >> 4;
      ty=thor->center_y;
      for(i=cy;i<=ty;i++) if(scrn.icon[i][cx]<TILE_SOLID) goto done;
      actr->num_moves=actr->pass_value+1;
      actr->temp1=1;
    }
  }
}

done:
next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_thirtytwo(ACTOR *actr){      //unused

actr=actr;
return 0;
}
//===========================================================================
int movement_thirtythree(ACTOR *actr){    //unused

actr=actr;
return 0;
}
//===========================================================================
int movement_thirtyfour(ACTOR *actr){    //unused

actr=actr;
return 0;
}
//===========================================================================
int movement_thirtyfive(ACTOR *actr){    //gun (single)

actr->next=actr->last_dir;
return actr->dir;
}
//===========================================================================
int movement_thirtysix(ACTOR *actr){    //acid drop

actr->speed=actr->pass_value;
next_frame(actr);
if(actr->next==0 && actr->frame_count==actr->frame_speed){
  actor_always_shoots(actr,1);
  actor[actr->shot_actor].x-=2;
}
return 0;
}
//===========================================================================
//4-way straight (random length) change
int movement_thirtyseven(ACTOR *actr){
int d,x1,y1,f;

d=actr->last_dir;
x1=actr->x;
y1=actr->y;

f=0;
if(actr->counter){
  actr->counter--;
  switch(d){
        case 0:
             y1-=2;
             if(!check_move2(x1,y1,actr)) f=1;
             break;
        case 1:
             y1+=2;
             if(!check_move2(x1,y1,actr)) f=1;
             break;
        case 2:
             x1-=2;
             if(!check_move2(x1,y1,actr)) f=1;
             break;
        case 3:
             x1+=2;
             if(!check_move2(x1,y1,actr)) f=1;
             break;
  }
}
else f=1;

if(f==1){
  actr->counter=rnd(90)+10;
  d=rnd(4);
}
next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
//timed darting
#define TIMER     actr->i1
#define INIT_DIR  actr->temp1
#define OTHER_DIR actr->temp2
#define FLAG      actr->temp3
#define OX        actr->i2
#define OY        actr->i3
#define CNT       actr->i4
int movement_thirtyeight(ACTOR *actr){
int d,x1,y1;

d=actr->last_dir;
x1=actr->x;
y1=actr->y;


if(!FLAG){
  FLAG=1;
  if(actr->pass_value) TIMER=actr->pass_value*15;
  else TIMER=5+rnd(360);
  INIT_DIR=actr->last_dir;
  OX=x1;
  OY=y1;
  CNT=0;
  switch(INIT_DIR){
    case 0:
      OTHER_DIR=1;
      break;
    case 1:
      OTHER_DIR=0;
      break;
    case 2:
      OTHER_DIR=3;
      break;
    case 3:
      OTHER_DIR=2;
      break;
  }
}
if(TIMER) {TIMER--; goto done;}

switch(d){
  case 0:
    y1-=2;
    break;
  case 1:
    y1+=2;
    break;
  case 2:
    x1-=2;
    break;
  case 3:
    x1+=2;
    break;
}

switch(FLAG){
  case 1:
    if(!check_move2(x1,y1,actr)){
      if(CNT){
        d=OTHER_DIR;
        actr->last_dir=d;
        FLAG=2;
      }
      else{
       actr->next=0;
       FLAG=0;
       goto done;
     }
    }
    else CNT++;
    break;
  case 2:
    check_move2(x1,y1,actr);
    if(x1==OX && y1==OY){
      FLAG=0;
      d=INIT_DIR;
      actr->last_dir=d;
      actr->next=0;
      goto done;
    }
}
next_frame(actr);

done:
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int movement_thirtynine(ACTOR *actr){    //troll 1

if(setup.skill==0){actr->speed=3;actr->num_moves=1;}
else if(setup.skill==1){actr->speed=2;actr->num_moves=1;}
else if(setup.skill==2){actr->speed=1;actr->num_moves=1;}


if(actr->pass_value<5) return movement_forty(actr);
if(actr->pass_value==10){
  if(overlap(actr->x+1,actr->y+1,actr->x+actr->size_x-1,
             actr->y+actr->size_y-1,thor_x1,thor_y1,thor_x2,thor_y2)){
    actr->strength=255;
    thor_damaged(actr);
  }
  return actr->dir;
}
if(actr->actor_num!=3) return actr->dir;

if(actr->i1){
  actr->i1--;
  actr->x-=2;
  check_move2(actr->x,actr->y,actr);
  actor[4].x-=2;
  actor[5].x-=2;
  actor[6].x-=2;
}
next_frame(actr);
if(actr->next==3) actr->next=0;
actor[4].next=actor[3].next;
actor[5].next=0;
actor[6].next=0;
return actr->dir;
}
//===========================================================================
int movement_forty(ACTOR *actr){    //troll 2
int x1,x2,a,d,f;

if(overlap(actr->x+1,actr->y+1,actr->x+actr->size_x+3,
           actr->y+actr->size_y-1,thor_x1,thor_y1,thor_x2,thor_y2)){
  actr->strength=150;
  thor_damaged(actr);
}
a=5+(actr->pass_value*4);
x1=actr->x;
x2=actor[a+1].x;
d=actr->last_dir;

if(actr->last_dir==2){
  f=1;
  if(bgtile(x1-2,actr->y)>=TILE_SOLID){
    actor[a].x-=2;
    actor[a-1].x-=2;
    actor[a-2].x-=2;
    actor[a+1].x-=2;
  }
  else f=0;
  if(!f)
   d=3;
}
else{
  f=1;
  if(bgtile(actor[a+1].x+14,actor[a+1].y)>=TILE_SOLID){
    actor[a].x+=2;
    actor[a-1].x+=2;
    actor[a-2].x+=2;
    actor[a+1].x+=2;
  }
  else f=0;
  if(!f) d=2;
}
if(!f){
  actr->x=x1;
  actor[a+1].x=x2;
}

if(actr->next==3 && !actr->num_shots && actr->frame_count==actr->frame_speed){
  actor_always_shoots(actr,1);
  actor[actr->shot_actor].x+=6;
}

next_frame(actr);
actor[a-2].next=actr->next;
actor[a-1].next=actr->next;
actor[a+1].next=actr->next;
actor[a-2].last_dir=d;
actor[a-1].last_dir=d;
actor[a+1].last_dir=d;
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
