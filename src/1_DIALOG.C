// THOR - The God of Thunder
//Source code released to the public domain on March 27th, 2020.

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>

#include <g_define.h>
#include <g_proto.h>
#include <res_man.h>
//===========================================================================
extern volatile unsigned int timer_cnt;
extern LEVEL scrn;
extern ACTOR actor[MAX_ACTORS];
extern ACTOR *thor;
extern int thor_x1,thor_y1,thor_x2,thor_y2,thor_real_y1;
extern int exit_flag;
extern int switch_flag;
extern unsigned int display_page,draw_page;
extern char far *bg_pics;
extern char far objects[NUM_OBJECTS][262];
extern char object_map[240];
extern char object_index[240];
extern THOR_INFO thor_info;
extern int rand1,rand2;
extern char cheat;
extern char *options_yesno[];
extern SETUP setup;
extern char far *sd_data;

#define GOTKEY1          setup.f00
#define TROLL_SHRUB      setup.f01
#define SEEN_BRIDGE      setup.f02
#define BRIDGE_FIXED     setup.f03
#define HERMIT_HAS_DOLL  setup.f04
#define GOTKEY2          setup.f05
#define TALKED_TO_HERMIT setup.f06
#define GOTKEY3          setup.f07
#define INFO1            setup.f08
#define INFO2            setup.f09
//==========================================================================
int dialog_one(ACTOR *actr);
int dialog_two(ACTOR *actr);
int dialog_three(ACTOR *actr);
int dialog_four(ACTOR *actr);
int dialog_five(ACTOR *actr);
int dialog_six(ACTOR *actr);
int dialog_seven(ACTOR *actr);
int dialog_eight(ACTOR *actr);
int dialog_nine(ACTOR *actr);
int dialog_ten(ACTOR *actr);
int dialog_eleven(ACTOR *actr);
int dialog_twelve(ACTOR *actr);
int dialog_thirteen(ACTOR *actr);
int dialog_fourteen(ACTOR *actr);

int (*dialog_func[]) (ACTOR *actr) =
    {
      dialog_one,
      dialog_two,
      dialog_three,
      dialog_four,
      dialog_five,
      dialog_six,
      dialog_seven,
      dialog_eight,
      dialog_nine,
      dialog_ten,
      dialog_eleven,
      dialog_twelve,
      dialog_thirteen,
      dialog_fourteen,
    };
//===========================================================================
char *d_howmany="So, How Many Ya Want?";
char *d_canihave="Well, Can I have it?";
char *d_whichisit="What Needs Fixed?";
char *d_whichbridge="Which Bridge?";
char *d_whatsprob="What's the Problem?";
char *d_whatcanido="What do you want to know?";
char *d_have100jewels="Can I have 100 jewels?";
char *d_wellhowdo="Well, how we gonna do this?";
char *d_wannatrade="Well, Do you want to trade?";
char *options_buy1[]={
              "5 Apples Please",
              "Fill 'er Up",
              "Never mind",
              NULL};
char *options_bobstv[]={
              "My TV",
              "A Bridge ",
              "Forget it",
              NULL};
char *options_tv[]={
              "Bad Picture Tube",
              "Horizontal Syncopation ",
              "Nothing",
              NULL};
char *options_bridge1[]={
              "London Bridge",
              "Dental Bridge",
              "I forgot",
              NULL};
char *options_bridge2[]={
              "London Bridge",
              "Dental Bridge",
              "Creekin's Bridge",
              "I forgot",
              NULL};
char *options_hermit[]={
              "What's the doll for?",
              "How do I find Jormangund?",
              "Nothing",
              NULL};
char *options_mailphone[]={
              "I want to order by mail",
              "I want to order by phone",
              NULL};
//===========================================================================
void d_restore(void){

xdisplay_actors(&actor[MAX_ACTORS-1],draw_page);
xshowpage(draw_page);
xcopyd2d(0,0,320,192,0,0,PAGE2,display_page,320,320);
xdisplay_actors(&actor[MAX_ACTORS-1],display_page);
xshowpage(display_page);
xerase_actors(actor,draw_page);
//xcopyd2d(0,0,320,192,0,0,PAGE2,display_page,320,320);
//xdisplay_actors(&actor[MAX_ACTORS-1],display_page);
}
//===========================================================================
void buy_apples(ACTOR *actr,int num, int amount){
int flag;

flag=0;
while(num){
  if(thor_info.jewels<amount){
    actor_speaks(actr,4002,0);
    d_restore();
    flag=0;
    break;
  }
  if(thor->health>=150){
    actor_speaks(actr,4003,0);
    d_restore();
    break;
  }
  num--;
  play_sound(GULP,1);
  thor_info.jewels-=amount;
  add_health(5);
  timer_cnt=0;
  display_health();
  display_jewels();
  while(timer_cnt<20) rotate_pal();
  flag=1;
}
if(flag){
  actor_speaks(actr,4007,0);
  d_restore();
}
}
//===========================================================================
int dialog_one(ACTOR *act){     //merchant $5
int sel;

act=act;
actor_speaks(act,3001,0);
d_restore();
sel=select_option(options_buy1,d_howmany,0);
d_restore();
if(sel==2) buy_apples(act,150,5);
else if(sel==1) buy_apples(act,5,5);
return 0;
}
//===========================================================================
int dialog_two(ACTOR *act){  //merchant $2
int sel;

act=act;
actor_speaks(act,3002,0);
d_restore();
sel=select_option(options_buy1,d_howmany,0);
d_restore();
if(sel==2) buy_apples(act,150,2);
else if(sel==1) buy_apples(act,5,2);
if(sel){
  actor_speaks(act,4007,0);
  d_restore();
}
return 0;
}
//===========================================================================
int dialog_three(ACTOR *act){    //keyman/treasure

act=act;
if(GOTKEY1){
  actor_speaks(act,4004,0);
  d_restore();
}
else{
  actor_speaks(act,3003,0);
  d_restore();
  play_sound(YAH,1);
  thor_info.keys++;
  display_keys();
  GOTKEY1=1;
}
return 0;
}
//===========================================================================
int dialog_four(ACTOR *act){   //troll
int sel;

if(TROLL_SHRUB){
  actor_speaks(act,3008,0);
  d_restore();
}
else{
  if(thor_info.inventory & 64){
    actor_speaks(act,3007,0);
    d_restore();
    sel=select_option(options_yesno,d_canihave,0);
    if(sel==1){
      delete_object();
      scrn.actor_loc[0]-=2;
      scrn.actor_loc[1]-=2;
      scrn.actor_loc[2]-=2;
      scrn.actor_loc[3]-=2;
      actor[3].i1=16;
      TROLL_SHRUB=1;
      actor_speaks(act,3010,0);
      d_restore();
    }
    else if(sel==2){
      actor_speaks(act,3009,0);
      d_restore();
    }
  }
  else{
    actor_speaks(act,3006,0);
    d_restore();
  }
}
return 0;
}
//===========================================================================
int dialog_five(ACTOR *act){  //bridge sign

if(!BRIDGE_FIXED) actor_speaks(act,102,0);
else actor_speaks(act,110,0);
d_restore();
SEEN_BRIDGE=1;
return 0;
}
//===========================================================================
void d_fix_tv(ACTOR *act){
int sel;

if(!actor_speaks(act,3012,0)) return;
while(1){
  sel=select_option(options_tv,d_whatsprob,0);
  d_restore();
  if(sel==1){
    if(!actor_speaks(act,3013,0)) return;
  }
  else if(sel==2){
    if(!actor_speaks(act,3014,0)) return;
  }
  else return;
}
}
//===========================================================================
void d_fix_bridge(ACTOR *act){
int sel;
LEVEL far *lvl;

if(!actor_speaks(act,3015,0)) return;
while(1){
  if(SEEN_BRIDGE) sel=select_option(options_bridge2,d_whichbridge,0);
  else{
    sel=select_option(options_bridge1,d_whichbridge,0);
    if(sel==3) sel=0;
  }
  d_restore();
  if(!sel) break;
//  sel-=SEEN_BRIDGE;
  if(sel==3){
    actor_speaks(act,3018,0);
    lvl=(LEVEL far *) (sd_data+(52*512));
    lvl->icon[138/20][138%20]=147;
    lvl->icon[158/20][158%20]=148;
    BRIDGE_FIXED=1;
    return;
  }
  else if(sel==1){
    if(!actor_speaks(act,3016,0)) return;
  }
  else if(sel==2){
    if(!actor_speaks(act,3017,0)) return;
  }
  else break;
}
}
//===========================================================================
int dialog_six(ACTOR *act){    //carpenter
int sel;

if(!HERMIT_HAS_DOLL){
  actor_speaks(act,3020,0);
  return 0;
}
if(BRIDGE_FIXED){
  actor_speaks(act,3019,0);
  return 0;
}
if(!actor_speaks(act,3011,0)) return 0;
while(1){
  if(BRIDGE_FIXED) break;
  sel=select_option(options_bobstv,d_whichisit,0);
  d_restore();
  if(sel==1) d_fix_tv(act);
  else if(sel==2) d_fix_bridge(act);
  else break;
}
return 0;
}
//===========================================================================
int dialog_seven(ACTOR *act){      //hermit/doll
int sel;

if(HERMIT_HAS_DOLL){
  play_sound(WOOP,1);
  goto jump;
}
else{
  if(thor_info.inventory & 64){
    actor_speaks(act,3022,0);
    d_restore();
    HERMIT_HAS_DOLL=1;
    delete_object();
jump:
    while(1){
      sel=select_option(options_hermit,d_whatcanido,0);
      d_restore();
      if(sel==1){
        actor_speaks(act,3023,0);
        d_restore();
      }
      else if(sel==2){
        actor_speaks(act,3024,0);
        d_restore();
      }
      else return 0;
    }
  }
  else{
    actor_speaks(act,3021,0);
    d_restore();
    TALKED_TO_HERMIT=1;
  }
}
return 0;
}
//===========================================================================
int dialog_eight(ACTOR *act){   //hermit underground

act=act;
if(GOTKEY2){
  actor_speaks(act,4004,0);
  d_restore();
}
else{
  actor_speaks(act,3029,0);
  d_restore();
  play_sound(YAH,1);
  thor_info.keys++;
  display_keys();
  GOTKEY2=1;
}
return 0;
}
//===========================================================================
int dialog_nine(ACTOR *act){   //ending salesman
int sel;

actor_speaks(act,3026,0);
d_restore();

while(1){
  sel=select_option(options_mailphone,d_wellhowdo,0);
  d_restore();
  if(!sel) break;
  if(sel==1){
    actor_speaks(act,3027,0);
    d_restore();
  }
  else if(sel==2){
    actor_speaks(act,3028,0);
    d_restore();
  }
}
return 0;
}
//===========================================================================
int dialog_ten(ACTOR *act){   //jewels/info 1
int sel;

if(INFO1 && INFO2){
  actor_speaks(act,4008,0);
  d_restore();
  return 0;
}
play_sound(WOOP,1);
sel=select_option(options_yesno,d_have100jewels,1);
d_restore();
if(sel==1){
  if(thor_info.jewels>=100){
    add_jewels(-100);
    thor_info.last_jewels=thor_info.jewels;
    if(!INFO1){
      actor_speaks(act,3025,0);
      d_restore();
      INFO1=1;
    }
    else if(!INFO2){
      actor_speaks(act,3032,0);
      d_restore();
      INFO2=1;
    }
  }
  else{
    actor_speaks(act,4002,0);
    d_restore();
  }
}
return 0;
}
//===========================================================================
int dialog_eleven(ACTOR *act){   //jewels/thanks
int sel;

play_sound(WOOP,1);
sel=select_option(options_yesno,d_have100jewels,1);
d_restore();
if(sel==1){
  if(thor_info.jewels>=100){
    add_jewels(-100);
    thor_info.last_jewels=thor_info.jewels;
    actor_speaks(act,3005,0);
    d_restore();
  }
  else{
    actor_speaks(act,4002,0);
    d_restore();
  }
}
return 0;
}

//===========================================================================
int dialog_twelve(ACTOR *act){   //guy punch

actor_speaks(act,11,0);
d_restore();
play_sound(PUNCH1,1);
pause(30);
play_sound(OW,1);
act->strength=20;
thor_damaged(act);
act->strength=0;
return 0;
}
//===========================================================================
int dialog_thirteen(ACTOR *act){   //spill guts girl
LEVEL far *lvl;

if(TALKED_TO_HERMIT){
  actor_speaks(act,24,0);
  d_restore();
  lvl=(LEVEL far *) (sd_data+(30*512));
  lvl->icon[145/20][145%20]=224;
  lvl->icon[145/20][145%20]=224;
}
else{
  actor_speaks(act,23,0);
  d_restore();
}
return 0;
}
//===========================================================================
int dialog_fourteen(ACTOR *act){   //magic shop guy
int sel;

if(GOTKEY3){
  actor_speaks(act,3031,0);
  d_restore();
}
else{
  if(thor_info.magic<149){
    actor_speaks(act,4006,0);
    d_restore();
  }
  else{
    actor_speaks(act,3030,0);
    d_restore();
    sel=select_option(options_yesno,d_wannatrade,0);
    d_restore();
    if(sel==1){
      thor_info.magic=0;
      display_magic();
      play_sound(YAH,1);
      thor_info.keys++;
      display_keys();
      GOTKEY3=1;
    }
}
}
return 0;
}


