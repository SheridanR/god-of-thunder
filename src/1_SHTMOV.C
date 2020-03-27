// THOR - The God of Thunder
//Source code released to the public domain on March 27th, 2020.

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>

#include <1_define.h>
#include <1_proto.h>
//===========================================================================
extern ACTOR actor[MAX_ACTORS];
extern int thor_x1,thor_y1,thor_x2,thor_y2,thor_real_y1;
extern ACTOR *thor;
extern char apple_drop;
//===========================================================================
int shot_movement_none(ACTOR *actr);
int shot_movement_one(ACTOR *actr);
int shot_movement_two(ACTOR *actr);
int shot_movement_three(ACTOR *actr);
int shot_movement_four(ACTOR *actr);
int shot_movement_five(ACTOR *actr);
int shot_movement_six(ACTOR *actr);
int shot_movement_seven(ACTOR *actr);
int shot_movement_eight(ACTOR *actr);
int shot_movement_nine(ACTOR *actr);
int shot_movement_ten(ACTOR *actr);
int shot_movement_eleven(ACTOR *actr);
//int shot_movement_twelve(ACTOR *actr);

int (*shot_movement_func[]) (ACTOR *actr) =
    {
      shot_movement_none,
      shot_movement_one,
      shot_movement_two,
      shot_movement_three,
      shot_movement_four,
      shot_movement_five,
      shot_movement_six,
      shot_movement_seven,
      shot_movement_eight,
      shot_movement_nine,
      shot_movement_ten,
      shot_movement_eleven,
//      shot_movement_twelve,
    };
//===========================================================================
void next_shot_frame(ACTOR *actr){

if(actr->directions==4 && actr->frames==1){
  actr->next=actr->last_dir;
  actr->dir=0;
}
else{
  actr->frame_count--;
  if(actr->frame_count<=0){
    actr->next++;
    if(actr->next>3) actr->next=0;
//    if(actr->next >= actr->frames) actr->next=0;
    actr->frame_count=actr->frame_speed;
  }
}
}
//===========================================================================
int shot_movement_none(ACTOR *actr){   //boss - snake

actr->temp3--;
if(!actr->temp3){
  actor_destroyed(actr);
  if(actor[actr->creator].num_shots) actor[actr->creator].num_shots--;
}
next_shot_frame(actr);
if(actr->directions==1) return 0;
return actr->last_dir;
}
//===========================================================================
int shot_movement_one(ACTOR *actr){
int d,x1,y1;

d=actr->last_dir;

switch(d){
      case 0:
           x1=actr->x;
           y1=actr->y-2;
           break;
      case 1:
           x1=actr->x;
           y1=actr->y+2;
           break;
      case 2:
           x1=actr->x-2;
           y1=actr->y;
           break;
      case 3:
           x1=actr->x+2;
           y1=actr->y;
           break;
}
if(!check_move3(x1,y1,actr)){
  actor_destroyed(actr);
  if(actor[actr->creator].num_shots) actor[actr->creator].num_shots--;
}
else{
  next_shot_frame(actr);
  actr->last_dir=d;
}
if(actr->directions==1) return 0;
if(actr->directions==4 && actr->frames==1) return 0;
return d;
}
//===========================================================================
int shot_movement_two(ACTOR *actr){
int d,x1,y1;

d=actr->last_dir;

switch(d){
      case 0:
           x1=actr->x;
           y1=actr->y-2;
           break;
      case 1:
           x1=actr->x;
           y1=actr->y+2;
           break;
      case 2:
           x1=actr->x-2;
           y1=actr->y;
           break;
      case 3:
           x1=actr->x+2;
           y1=actr->y;
           break;
}
if(!check_move4(x1,y1,actr)){
  actor_destroyed(actr);
  if(actor[actr->creator].num_shots) actor[actr->creator].num_shots--;
}
else{
  next_shot_frame(actr);
  actr->last_dir=d;
}
if(actr->directions==1) return 0;
if(actr->directions==4 && actr->frames==1) return 0;
return d;
}
//===========================================================================
int shot_movement_three(ACTOR *actr){       //serpent fire
int d;

d=actr->last_dir;
actr->x-=2;

actr->temp3--;
actr->temp4--;
if(overlap(thor_x1-1,thor_y1-1,thor_x2+1,thor_y2+1,
   actr->x,actr->y,actr->x+15,actr->y+15)){
   actr->move=0;
   actr->speed=6;
   thor_damaged(actr);
   actr->x+=2;
}
if(!actr->temp4){
  actr->temp4=actr->temp5;
  actr->speed++;
  if(actr->speed>6) actr->move=0;
}
if(!actr->temp3){
  actor_destroyed(actr);
  if(actor[actr->creator].num_shots) actor[actr->creator].num_shots--;
}
else{
  next_shot_frame(actr);
  actr->last_dir=d;
}
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int shot_movement_four(ACTOR *actr){          //wraith balls
int x1,y1,xd,yd,d;

if(actr->temp1){
  actr->temp1--;
  if(!actr->temp1){
    actor_destroyed(actr);
    apple_drop++;
    if(apple_drop==4){
      if(_drop_obj(actr,5)) apple_drop=0;
      else apple_drop=3;
    }
    else _drop_obj(actr,3);
    return 0;
  }
}
if(overlap(thor->x-1,thor->y-1,thor_x2+1,thor_y2+1,
           actr->x,actr->y,actr->x+15,actr->y+15)){
  thor_damaged(actr);
  actor_destroyed(actr);
  return 0;
}

x1=actr->x;
y1=actr->y;
xd=yd=0;
d=actr->last_dir;

if((x1>(thor_x1)+1)) xd=-2;
else if((x1<(thor_x1)-1)) xd=2;

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
    if(check_move3(x1,y1,actr)){
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
    if(check_move3(x1,y1,actr)){
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
    if(check_move3(x1,y1,actr)){
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
    if(check_move3(x1,y1,actr)){
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
    if(check_move3(x1,y1,actr)){
      if(xd>0) d=3;
      else d=2;
      next_frame(actr);
      actr->last_dir=d;
      if(actr->directions==1) return 0;
      return d;
    }
  }
}
check_move3(actr->x,actr->y,actr);
next_frame(actr);
actr->last_dir=d;
if(actr->directions==1) return 0;
return d;
}
//===========================================================================
int shot_movement_five(ACTOR *actr){   //no move, frame cycle

next_shot_frame(actr);
if(actr->directions==1) return 0;
return actr->last_dir;
}
//===========================================================================
int shot_movement_six(ACTOR *actr){   //wraith spots

actr->temp1--;
if(!actr->temp1){
  actor_destroyed(actr);
  if(actor[actr->creator].num_shots) actor[actr->creator].num_shots--;
}
else next_shot_frame(actr);
if(actr->directions==1) return 0;
return actr->last_dir;
}
//===========================================================================
#define YV actr->i1
#define YC actr->i2
#define BC actr->i3
#define YD actr->i4
#define XD actr->i5
#define XC actr->i6
#define IV 100
#define IC 50
int shot_movement_seven(ACTOR *actr){   //skull drop

if(actr->temp3){
  actr->temp3--;
  goto done;
}
if(overlap(actr->x,actr->y,actr->x+actr->size_x,actr->y+actr->size_y,
          thor->x,thor->y+4,thor->x+15,thor->y+15))
  thor_damaged(actr);

actr->temp2++;
if(actr->temp2>2){
  if(actr->temp4) actr->temp4--;
  actr->temp2=0;
}
actr->temp3=actr->temp4;

actr->y+=2;
if(actr->y>160-36){
  actr->x+=(4-rnd(9));
  actr->move=8;
  YV=IV;
  YC=0;
  BC=IC;
  YD=0;
  XC=3;
  if(actr->x<150) XD=1;
  else XD=0;
}

done:
next_shot_frame(actr);
if(actr->directions==1) return 0;
return actr->last_dir;
}
//===========================================================================
int shot_movement_eight(ACTOR *actr){   //skull bounce
int x,y;

x=actr->x;
y=actr->y;

XC--;
if(!XC){
  XC=3;
  if(XD) x+=2;
  else x-=2;
}
YC+=YV;
if(YC>(IV-1)){
  if(!YD){
    YV-=8;
    YC-=IV;
    y-=2;
  }
  else{
    YV+=8;
    YC-=IV;
    y+=2;
  }
}
if(YV<0){
  YV=0;
  BC=1;
}
if(YV>IV){
  YV=IV;
  BC=1;
}
BC--;
if(!BC){
  BC=IC;
  if(YD) YV=IV;
  YD^=1;
}
if(y>164) y=164;
//   8       311
if(x<1 || x>(319-actr->size_x)){
  if(!actr->dead) if(actor[actr->creator].num_shots) actor[actr->creator].num_shots--;
  actor_destroyed(actr);
}
if(overlap(actr->x,actr->y,actr->x+actr->size_x,actr->y+actr->size_y,
          thor->x,thor->y+4,thor->x+15,thor->y+15))
  thor_damaged(actr);
actr->x=x;
actr->y=y;

done:
next_shot_frame(actr);
if(actr->directions==1) return 0;
return actr->last_dir;
}
//===========================================================================
int shot_movement_nine(ACTOR *actr){   //skull explode

actr->next++;
if(actr->next==3){
  actor[actr->creator].num_shots--;
  actor_destroyed(actr);
  return 0;
}
if(actr->directions==1) return 0;
return actr->last_dir;
}
//===========================================================================
int shot_movement_ten(ACTOR *actr){  //skull - stalagtite
int f;

f=0;
actr->y+=2;

if(overlap(actr->x,actr->y,actr->x+actr->size_x,actr->y+actr->size_y,
          thor->x,thor->y+4,thor->x+15,thor->y+15)){
  thor_damaged(actr);
  f=1;
}
if((actr->y>160) || f){
  if(actor[actr->creator].num_shots) actor[actr->creator].num_shots--;
  actor_destroyed(actr);
}

return 0;
}
//===========================================================================
#undef  YD
#undef  XD
#define YA actr->i1
#define XA actr->i2
#define YD actr->i3
#define XD actr->i4
#define DIR actr->i5
#define CNT actr->i6

void calc_angle(int x1,int y1,int x2,int y2,ACTOR *actr){

if(x1<x2){
  XA=-2;
  XD=x2-x1;
}
else if(x1>x2){
  XA=2;
  XD=x1-x2;
}
else{
  XA=0;
  XD=0;
}

if(y1<y2){
  YA=-2;
  YD=y2-y1;
}
else if(y1>y2){
  YA=2;
  YD=y1-y2;
}
else{
  YA=0;
  YD=0;
}

if(YD>=XD) DIR=1;
else DIR=0;
CNT=0;
}
//===========================================================================
int shot_movement_eleven(ACTOR *actr){  //angle throw
int x1,y1;

x1=actr->x;
y1=actr->y;

if(!actr->temp1){
  calc_angle(thor_x1,thor_real_y1,x1,y1,actr);
  actr->temp1=1;
}

if(DIR){
  y1+=YA;
  CNT+=XD;
  if(CNT>=YD){
    x1+=XA;
    CNT-=YD;
  }
}
else{
  x1+=XA;
  CNT+=YD;
  if(CNT>=XD){
    y1+=YA;
    CNT-=XD;
  }
}

if(!check_move3(x1,y1,actr)){
  if(actor[actr->creator].num_shots) actor[actr->creator].num_shots--;
  actor_destroyed(actr);
}
else next_frame(actr);

if(actr->directions==1) return 0;
return actr->last_dir;
}


