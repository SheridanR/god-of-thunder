// THOR - The God of Thunder
//Source code released to the public domain on March 27th, 2020.

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>

#include <2_define.h>
#include <2_proto.h>
//===========================================================================
extern ACTOR actor[MAX_ACTORS];  //current actors
extern ACTOR *thor;
extern int thor_x1,thor_y1,thor_x2,thor_y2,thor_real_y1;
extern int rand1,rand2;
extern int thor_pos;
extern SETUP setup;
//===========================================================================
int shot_pattern_none(ACTOR *actr);
int shot_pattern_one(ACTOR *actr);
int shot_pattern_two(ACTOR *actr);
int shot_pattern_three(ACTOR *actr);
int shot_pattern_four(ACTOR *actr);
int shot_pattern_five(ACTOR *actr);
int shot_pattern_six(ACTOR *actr);
int shot_pattern_seven(ACTOR *actr);
int shot_pattern_eight(ACTOR *actr);

int (*shot_pattern_func[]) (ACTOR *actr) =
    { shot_pattern_none,
      shot_pattern_one,
      shot_pattern_two,
      shot_pattern_three,
      shot_pattern_four,
      shot_pattern_five,
      shot_pattern_six,
      shot_pattern_seven,
      shot_pattern_eight,
    };
//===========================================================================
int shot_pattern_none(ACTOR *actr){   //no shooting

actr=actr;
return 0;
}
//===========================================================================
int shot_pattern_one(ACTOR *actr){   //uni-directional seek

switch(actr->last_dir){
      case 0:
           if(abs(thor_x1-actr->x)<8){
             if(actr->y>thor_real_y1){  //r
               if(actor_shoots(actr,0)) return 1;
             }
           }
           break;
      case 1:
           if(abs(thor_x1-actr->x)<8){
             if(actr->y<thor_real_y1){  //r
               if(actor_shoots(actr,1)) return 1;
             }
           }
           break;
      case 2:
           if(abs(thor_real_y1-actr->y)<8){ //r
             if(actr->x>thor_x1){
               if(actor_shoots(actr,2)) return 1;
             }
           }
           break;
      case 3:
           if(abs(thor_real_y1-actr->y)<8){ //r
             if(actr->x<thor_x1){
               if(actor_shoots(actr,3)) return 1;
             }
           }
           break;
}
return 0;
}
//===========================================================================
int shot_pattern_two(ACTOR *actr){   //omni directional

if(abs(thor->x-actr->x)<8){
  if(thor->y > actr->y) actor_shoots(actr,1);
  else if(thor->y < actr->y) actor_shoots(actr,0);
  else return 0;
}
else if(abs(thor->y-actr->y)<8){
  if(thor->x > actr->x) actor_shoots(actr,3);
  else if(thor->x < actr->x) actor_shoots(actr,2);
}
else return 0;

return 1;
}
//===========================================================================
int shot_pattern_three(ACTOR *actr){   //uni directional (backwards)
int ld;

ld=actr->last_dir;
if(shot_pattern_one(actr)){
  actr->last_dir=reverse_direction(actr);
  return 1;
}
actr->last_dir=reverse_direction(actr);
shot_pattern_one(actr);
actr->last_dir=ld;
return 1;
}
//===========================================================================
int shot_pattern_four(ACTOR *actr){   //omni-directional not solid shot

if(abs(thor->x-actr->x)<8){
  if(thor->y > actr->y) actor_always_shoots(actr,1);
  else if(thor->y < actr->y) actor_always_shoots(actr,0);
  else return 0;
}
else if(abs(thor->y-actr->y)<8){
  if(thor->x > actr->x) actor_always_shoots(actr,3);
  else if(thor->x < actr->x) actor_always_shoots(actr,2);
}
else return 0;

return 1;
}
//===========================================================================
int shot_pattern_five(ACTOR *actr){   //boss - snake
int num;

if(rand1<15){
  if((actr->temp1==0) && (actr->temp2==0)){
     actr->y+=16;
     actr->shots_allowed=3+setup.skill;
     actor_shoots(actr,2);
     play_sound(BOSS12,0);
//     num=actr->creator;
     num=actr->shot_actor;
     actr->shot_cnt=50;
     actor[num].temp3=120;
     actor[num].temp4=5+(rand2%17);
     actor[num].temp5=actor[num].temp4;
     actr->y-=16;
     return 1;
  }
}
return 0;
}
//===========================================================================
int shot_pattern_six(ACTOR *actr){   //4 surrounding squares
int pos;

pos=((actr->x)/16)+(((actr->y)/16)*20);

if(thor_pos==pos-20) actor_shoots(actr,0);
else if(thor_pos==pos+20) actor_shoots(actr,1);
else if(thor_pos==pos-1)  actor_shoots(actr,2);
else if(thor_pos==pos+1)  actor_shoots(actr,3);
else return 0;

actr->frame_sequence[3]=3;
actr->next=3;
return 1;
}
//===========================================================================
int shot_pattern_seven(ACTOR *actr){   //none

actr=actr;
return 0;
}
//===========================================================================
int shot_pattern_eight(ACTOR *actr){   //random

if(!actr->i2){
  actr->i1=actr->func_pass;
  actr->i2=1;
}

if(actr->i1) actr->i1--;
else{
  if(rand1<10){
    actr->i1=thor_x1;
    actr->i2=thor_real_y1;
    actr->i1=actr->func_pass;
    actor_shoots(actr,0);
    return 1;
  }
}
return 0;
}
