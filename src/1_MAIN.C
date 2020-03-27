// THOR - The God of Thunder
//Source code released to the public domain on March 27th, 2020.

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <string.h>
#include <time.h>
#include <alloc.h>
#include <process.h>

#include <1_define.h>
#include <1_proto.h>
#include <mu_man.h>

//========================= Global Declarations ==============================
unsigned int page[3]={PAGE0,PAGE1,PAGE2};
unsigned int display_page,draw_page;
unsigned int page3_offset;
char pge;
int exit_flag;

volatile char key_flag[100];
volatile char joy_flag[100];
volatile char tmp_flag[100];
char break_code;
char scan_code,last_scan_code;
char diag;
char slow_mode,startup;
char shot_ok;
int thor_x1,thor_y1,thor_x2,thor_y2,thor_real_y1;
int thor_pos;
int max_shot;

volatile unsigned int timer_cnt,vbl_cnt,magic_cnt,extra_cnt;

char far text[94][72];
union REGS in,out;
struct SREGS seg;
char far *bg_pics;
//char bg_header[200];
char far objects[NUM_OBJECTS][262];
int ox,oy,of;
char object_map[240];
char object_index[240];
char far *bleep;
char thor_icon1,thor_icon2,thor_icon3,thor_icon4;
char level_type;
char far *song;
char music_current;
char boss_loaded;
char apple_drop;
char cheat;
char area;
char last_setup[32];

LEVEL scrn;
char *scrnp;

char far *sd_data;
int current_level,new_level,new_level_tile,current_area;
//char file_str[10];

SETUP setup;
char *tmp_buff;
int reps;

char far *mask_buff;
char far *mask_buff_start;
char abuff[AMI_LEN];
char *ami_buff;
ACTOR actor[MAX_ACTORS];   //current actors
ACTOR enemy[MAX_ENEMIES];  //current enemies
ACTOR shot[MAX_ENEMIES];   //current shots
char enemy_type[MAX_ENEMIES];

ACTOR magic_item[2];
char magic_pic[2][1024];

//ACTOR enemy[4];   //4 different available actors
//char enemy_used[4];
//char enemy_type[4];
//char enemy_complex[4];
//unsigned int enemy_buff[4];
//int enemy_mask_offset[4];

//ACTOR shot[4];    //4 shots
//unsigned int shot_buff[4];
//int shot_mask_offset[4];
char warp_scroll;

//char far *enemy_data;     //points to four actors/sounds

ACTOR *thor;
ACTOR *hammer;
ACTOR explosion;
ACTOR sparkle;
THOR_INFO thor_info;
int key_fire,key_up,key_down,key_left,key_right,key_magic,key_select;
int boss_dead;

int warp_flag;

char far *std_sound_start;
char far *pcstd_sound_start;
char far *std_sound;
char far *pcstd_sounds;
char far *boss_sound[3];
char far *boss_pcsound[3];
long pcsound_length[NUM_SOUNDS];
int rand1,rand2;
int restore_screen;
int last_oracle;
int hourglass_flag,thunder_flag,shield_on,lightning_used,tornado_used;
int apple_flag,bomb_flag;
int switch_flag;
unsigned int joy_x,joy_y;
char joy_b1,joy_b2;
int joystick,joylx,joyly,joyhx,joyhy;
char res_file[16];
char odin[4][262];
char hampic[4][262];
int load_game_flag;
int music_flag,sound_flag,pcsound_flag;
int cash1_inform,cash2_inform,door_inform,magic_inform,carry_inform;
int killgg_inform;
char dialog_color[]={14,54,120,138,15,0,0,0,0,0,0,0,0,0,0,0};
//norm,good,bad,sign,white

char far *std_sounds;
char far *pc_sound[NUM_SOUNDS];
char far *dig_sound[NUM_SOUNDS];
int  boss_active;
char story_flag;
char *save_filename="XXXXXXXX.XXX";
char far *scr;
char demo_key[DEMO_LEN];
int  demo_cnt;
char demo,record;
char demo_enable;
int  rnd_index;
int  rnd_array[100];
char rdemo;
char test_sdf[80];
char *options_yesno[]={"Yes","No",NULL};
char far *lzss_buff;
char game_over;
char noal,nosb,ret;
char tempstr[80];
char auto_load;
char ide_run,fast_exit,nojoy,gr,xdos;
char main_loop;
char end_tile;

void interrupt far (*old_timer_int)(void);   // interrupt function pointer
void interrupt far timer_int(void);

void xdisplay_actors(ACTOR *act,unsigned int page);
void xerase_actors(ACTOR *act,unsigned int page);
void setup_load(void);
//============================================================================
void main(int argc, char *argv[]){
int err,i;
char s[21];
int vbl_flag;
int ma,rp;
int po[4]={-1,1,-80,80};
int loop,vl;
int opt;
FILE *fp;

//chdir("\\gottest");
//for(loop=0;loop<argc;loop++) printf("%s\r\n",argv[loop]);
//getch();
startup=1;
hourglass_flag=0;
thunder_flag=0;
lightning_used=0;
tornado_used=0;
switch_flag=0;
boss_dead=0;
load_game_flag=0;
warp_scroll=0;
boss_loaded=0;
demo_enable=0;
game_over=0;

music_flag=1;
sound_flag=1;
pcsound_flag=1;

slow_mode=0;

cash1_inform=0;
cash2_inform=0;
door_inform=0;
magic_inform=0;
carry_inform=0;
killgg_inform=0;
story_flag=1;
opt=0;

current_level=23;
area=1;
cheat=0;
demo=0;
rdemo=0;
record=0;
noal=0;
nosb=0;
ide_run=0;
fast_exit=0;
nojoy=0;
gr=0;
joystick=0;
xdos=0;
main_loop=0;
end_tile=0;

//ultoa(coreleft(),ts,10);
//printf("\r\n%s",ts);
//getch();

if(argc>1){                        //parse the command line
  for(loop=0;loop<argc;loop++){
     strupr(argv[loop]);
     if(argv[loop][0]=='/' || argv[loop][0]=='\\'){
       strcpy(&argv[loop][0],&argv[loop][1]);
       if(strstr(argv[loop],"SAVEGAME:")){
            strcpy(save_filename,(strchr(argv[loop],':')+1));
       }
       else if(strstr(argv[loop],"JOY:")){
         strcpy(tempstr,strchr(argv[loop],':'));
         set_joy();
       }
       else if(!strcmp(argv[loop],"VOLSTAGG")) cheat=1;
       else if(!strcmp(argv[loop],"NOJOY")) nojoy=1;
       else if(!strcmp(argv[loop],"GR")) gr=1;
       else if(!strcmp(argv[loop],"DEMO")){ demo=1;cheat=1;}
       else if(!strcmp(argv[loop],"NOAL")){
         noal=1;
         music_flag=0;
         sound_flag=0;
       }
       else if(!strcmp(argv[loop],"NOSB")){
         nosb=1;
         sound_flag=0;
         pcsound_flag=1;
       }
       else if(!strcmp(argv[loop],"RDEMO")){ rdemo=1;demo=1;cheat=1;}
       else if(!strcmp(argv[loop],"IDE")) ide_run=1;
       else if(!strcmp(argv[loop],"RECORD")) {record=1;cheat=1;}
       else if(argv[loop][0]=='S'){
         argv[loop][0]=' ';
         current_level=atoi(argv[loop]);
       }
    }
  }
}
if(!strstr(save_filename,"SAV") && !demo){
  ret=0;
  run_gotm();
}
ret=1;
auto_load=0;
if(file_size(save_filename)>32){
  auto_load=1;
  story_flag=0;
}

setup.area=area;

if(demo || record){
  if(record) demo=0;
  area=1;
  setup.area=1;
  cash1_inform=1;
  cash2_inform=1;
  door_inform=1;
  magic_inform=1;
  carry_inform=1;
  story_flag=0;
}

if(current_level!=23) story_flag=0;

err=initialize();
if(err) exit_code(err);

if(rdemo){
  fp=fopen("demo.got","rb");
  if(fp){
    fread(&demo_key[0],1,DEMO_LEN,fp);
    fclose(fp);
  }
}

#ifdef DEBUG
  ultoa(coreleft(),ts,10);
  xprint(272,40,ts,PAGES,14);
#endif

warp_flag=0;
display_page=PAGE0;
draw_page=PAGE1;
xshowpage(PAGE0);

if(record) memset(demo_key,0,DEMO_LEN);
if(demo || record){
  thor->health=100;
  thor_info.magic=100;
  thor_info.jewels=463;
  thor_info.score=12455;
  setup.skill=0;
  thor_info.inventory=1+2;
  current_level=54;
  thor_info.item=2;
  display_item();
  display_health();
  display_magic();
  display_score();

}
new_level=current_level;
movedata(FP_SEG(sd_data+(new_level*512)),FP_OFF(sd_data+(new_level*512)),
        FP_SEG(&scrn),FP_OFF(&scrn),sizeof(LEVEL));
show_level(current_level);
exit_flag=0;
pge=0;

timer_cnt=0;
vbl_cnt=0;
vl=0;

thor->speed_count=6;

demo_cnt=0;
demo_enable=1;
rnd_index=0;
music_current=-1;

if(auto_load){
  if(load_game(0)){
    setup_load();
  }
  auto_load=0;
  if(GAME1 && current_area==59 && !setup.game_over){
    auto_load=1;
    fade_in();
    boss_level1();
  }
}
else if(!cheat) key_flag[ESC]=1;
if(!auto_load){
  fade_in();
  music_play(level_type,1);
}
auto_load=0;

startup=0;
if(record) memset(demo_key,0,DEMO_LEN);

//------------------------------------------------------------------------
//ultoa(farcoreleft(),s,10);
//xprint(0,0,s,PAGES,14);
//ultoa(coreleft(),s,10);
//xprint(0,10,s,PAGES,14);

srand(1234);
main_loop=1;
while(1){
  if(demo || record) demo_key_set();
  demo_cnt++;
  if(record){
//    if(demo_cnt<DEMO_LEN){
//      if(!demo_key[demo_cnt]) demo_key[demo_cnt]=scan_code;
//      else demo_key[demo_cnt+1]=scan_code;
//    }
//    if(cheat){
      itoa(demo_cnt,s,10);
      xfillrectangle(0,40,296+24,40+8,PAGES,0);
      xprint(0,39,s,PAGES,14);
//    }
  }
  rand1=rnd(100);
  rand2=rnd(100);
  display_page=page[pge];
  pge=pge^1;
  pge=pge;
  draw_page=page[pge];

if(cheat){
  if(key_flag[_THREE]){
    asm	mov	dx,status_Reg1
    asm	in	al,dx
    asm	mov	dx,atr_Index
    asm	mov	al,atr_Overscan
    asm	out	dx,al
    asm	mov	al,10	// green
    asm	out	dx,al
    asm	mov	al,20h	// normal
    asm	out	dx,al
    vbl_flag=1;
  }
  else vbl_flag=0;
}

if(thunder_flag){
  rp=rand1/25;
  xshowpage(display_page+po[rp]);
  thunder_flag--;
  if((thunder_flag<MAX_ACTORS) && thunder_flag>2)
    if(actor[thunder_flag].used){
      actor[thunder_flag].vunerable=0;
      actor_damaged(&actor[thunder_flag],20);
    }
}
else xshowpage(display_page);

if(cheat){
  if(vbl_flag){
    asm	mov	dx,status_Reg1
    asm	in	al,dx
    asm	mov	dx,atr_Index
    asm	mov	al,atr_Overscan
    asm	out	dx,al
    asm	mov	al,0	// black
    asm	out	dx,al
    asm	mov	al,20h	// normal
    asm	out	dx,al
  }
}
  if(restore_screen){
    xcopyd2d(0,0,320,192,0,0,PAGE2,draw_page,320,320);
    restore_screen=0;
  }
  else xerase_actors(actor,draw_page);
  if(of){  //replace tile after object is picked up
    xcopyd2d(ox,oy,ox+16,oy+16,ox,oy,PAGE2,draw_page,320,320);
    of=0;
  }
  if(exit_flag==2){
    thor_dies();
    exit_flag=0;
  }
  if(switch_flag){
    switch(switch_flag){
      case 1:{switch_icons(); break;}
      case 2:{rotate_arrows(); break;}
    }
    switch_flag=0;
  }
  thor_pos=((thor->x+7)/16)+(((thor->y+8)/16)*20);
  if(object_map[thor_pos]) pick_up_object(thor_pos);
  shot_ok=1;
  ma=MAX_ACTORS;
  if(slow_mode){
    while(vbl_cnt<4);
    vbl_cnt=0;
    vl=2;
  }
  else vl=1;
  if(joystick){
    joy_key();
    merge_keys();
  }
  if(cheat){
    if(key_flag[_K]) screen_dump();
    if(key_flag[_TWO]) delay(2500);
    if(key_flag[_Z]) add_magic(150);
    if(key_flag[45]){fast_exit=1; break;}   //X to exit fast
  }
  if(key_flag[_S]) save_game();
  else if(key_flag[_L]){
     if(load_game(1)) setup_load();
  }
  if(opt){
    if(opt==1){
      if(select_sound()){
        d_restore();
        select_music();
      }
    }
    else if(opt==2) select_skill();
    else if(opt==3) save_game();
    else if(opt==4){
      if(load_game(1)) setup_load();
    }
    else if(opt==5){
      if(!game_over) thor_dies();
    }
    else if(opt==6) select_fastmode();
    else if(opt==7) help();
    else if(opt==8){
      xdos=ask_exit();
      if(xdos==2 || xdos==3) break;
    }
    opt=0;
  }
  if(!boss_dead){
    if(key_flag[_F1]) if(!thunder_flag) help();
    if(key_flag[_D]) if(!thunder_flag && !game_over) thor_dies();
    if(key_flag[ESC]) if(!thunder_flag){
      key_flag[ESC]=0;
      opt=option_menu();
    }
  }
  for(loop=0;loop<vl;loop++){
    for(i=0;i<ma;i++){
       if(actor[i].used){
         if(hourglass_flag)
           if((i>2) && (!pge) && (!(actor[i].magic_hurts & HOURGLASS_MAGIC)) ) continue;
         actor[i].move_count=actor[i].num_moves;
         while(actor[i].move_count--) move_actor(&actor[i]);
         if(i==0) set_thor_vars();
         if(new_level!=current_level) goto brk_loop;
       }
    }
  }
  if(end_tile) break;
  brk_loop:
  if(exit_flag==2){
    thor_dies();
    exit_flag=0;
  }
  thor->center_x=thor_pos%20;
  thor->center_y=thor_pos/20;
//  save_d=thor->dir;
//  if(scrn.icon[thor->center_y][thor->center_x]==154) thor->dir=0;
  xdisplay_actors(&actor[MAX_ACTORS-1],draw_page);
//  thor->dir=save_d;
  if(current_level!=new_level){
      i=level_type;
      thor->show=0;
      hammer->used=0;
      show_level(new_level);
      tornado_used=0;
      if(cheat && !demo){
        xfillrectangle(296,0,296+24,10,PAGES,0);
        itoa(current_level,s,10);
        xprint(296,0,s,PAGES,14);
      }
  }
  use_item();
  if(boss_dead){
    for(loop=3;loop<7;loop++) if(actor[loop].used) break;
    if(loop==7){
      xerase_actors(actor,display_page);
      xdisplay_actors(&actor[MAX_ACTORS-1],display_page);
      exit_flag=0;
      if(boss_active==1){
        closing_sequence1();  //serpent
        boss_active=0;
//        exit_flag=1;
      }
      if(exit_flag) break;
    }
  }
  if(demo || record){
    if(demo_cnt>=(DEMO_LEN-1)) break;
    if(exit_flag==5) break;
  }
}
//chdir("\\bc");
if(record){
 fp=fopen("demo.got","wb");
 if(fp){
   fwrite(&demo_key[0],1,DEMO_LEN,fp);
   fclose(fp);
 }
}
if(ret && !fast_exit && !demo && !record){
  d_restore();
  save_game();
}
music_pause();
fade_out();
exit_code(0);
//if(xdos==3 && cheat) exit(0);
if((!cheat) || demo){
  _AX=0x0100;
  _CX=0x2000;
  geninterrupt(0x10);
}
run_gotm();
exit(0);
}
extern int SBResetCount;
//===========================================================================
void run_gotm(void){
int num;
char s[36];
char *args[]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,
              NULL,NULL,NULL,NULL,NULL};

args[0]="GOT.EXE";

num=1;
if(xdos==3){
  args[num]="/EXIT";
  num++;
}
if(ret){
  args[num]="/RETURN";
  num++;
}
if(demo && exit_flag==5){
  args[num]="/KEYRETURN";
  num++;
}
if(noal){
  args[num]="/NOAL";
  num++;
}
if(gr){
  args[num]="/GR";
  num++;
}
if(nojoy){
  args[num]="/NOJOY";
  num++;
}
if(nosb){
  args[num]="/NOSB";
  num++;
}
if(cheat && !demo){
  args[num]="/VOLSTAGG";
  num++;
}
if(game_over){
  ltoa(thor_info.score,s,10);
  strcpy(res_file,"/SCORE:");
  strcat(res_file,s);
  args[num]=res_file;
  num++;
  strcpy(s,"/SAVEGAME:");
  strcat(s,save_filename);
  args[num]=s;
  num++;
}
//num=0;
//while(args[num]){
//  printf("%s\r\n",args[num]);
//  num++;
//}
//printf("%d\r\n",SBResetCount);
//getch();

if(!ide_run) execv("GOT.EXE",args);
exit(0);
}
//===========================================================================
void printt(int val){
char s[21];

itoa(val,s,10);
xfillrectangle(200,16,224,40,PAGES,20);
xprint(200,16,s,PAGES,12);
}
//===========================================================================
void thor_dies(void){
int li,ln;

for(li=0;li<MAX_ACTORS;li++) actor[li].show=0;

xdisplay_actors(&actor[MAX_ACTORS-1],draw_page);

li=thor_info.item;
ln=thor_info.inventory;
play_sound(DEAD,1);
thor_spins(0);
xfput(thor->x,thor->y,display_page,objects[10]);
thor->used=1;
timer_cnt=0;
while(timer_cnt<60) rotate_pal();
new_level=thor_info.last_screen;
thor->x=(thor_info.last_icon%20)*16;
thor->y=((thor_info.last_icon/20)*16)-1;
if(thor->x<1) thor->x=1;
if(thor->y<0) thor->y=0;
thor->last_x[0]=thor->x;
thor->last_x[1]=thor->x;
thor->last_y[0]=thor->y;
thor->last_y[1]=thor->y;
thor->dir=thor_info.last_dir;
thor->last_dir=thor_info.last_dir;
thor->health=thor_info.last_health;
thor_info.magic=thor_info.last_magic;
thor_info.jewels=thor_info.last_jewels;
thor_info.keys=thor_info.last_keys;
thor_info.score=thor_info.last_score;
thor_info.object=thor_info.last_object;
thor_info.object_name=thor_info.last_object_name;
if(ln==thor_info.last_inventory){
  thor_info.item=li;
}
else{
  thor_info.item=thor_info.last_item;
  thor_info.inventory=thor_info.last_inventory;
}
memcpy(&setup,last_setup,32);
thor->num_moves=1;
thor->vunerable=60;
thor->show=60;
hourglass_flag=0;
apple_flag=0;
bomb_flag=0;
thunder_flag=0;
lightning_used=0;
tornado_used=0;
shield_on=0;
music_resume();
actor[1].used=0;
actor[2].used=0;
thor->speed_count=6;
movedata(FP_SEG(sd_data+(new_level*512)),FP_OFF(sd_data+(new_level*512)),
        FP_SEG(&scrn),FP_OFF(&scrn),sizeof(LEVEL));
display_health();
display_magic();
display_jewels();
display_keys();
display_score();
display_item();
show_level(new_level);
set_thor_vars();
}
//===========================================================================
void thor_spins(int flag){
int i,d,c;
unsigned int dr,di,sw;
char da[]={0,2,1,3};

if(of){  //replace tile after object is picked up
  xcopyd2d(ox,oy,ox+16,oy+16,ox,oy,PAGE2,display_page,320,320);
  of=0;
}
actor[2].used=0;
dr=draw_page;
di=display_page;
d=0;
c=0;
thor->next=0;
for(i=0;i<60;i++){
   thor->dir=da[d];
   thor->last_dir=da[d];
   c++;
   if(c>4){
     d++;
     d=d%4;
     c=0;
   }
   xerase_actors(actor,dr);
   if(shield_on) actor[2].used=0;
   if(i==59) thor->used=0;
   xdisplay_actors(&actor[MAX_ACTORS-1],dr);
   if(shield_on) actor[2].used=1;
   xshowpage(dr);
   sw=dr;
   dr=di;
   di=sw;
}
flag=flag;
}
//===========================================================================
void setup_load(void){

thor->used=1;
new_level=thor_info.last_screen;
thor->x=(thor_info.last_icon%20)*16;
thor->y=((thor_info.last_icon/20)*16)-1;
if(thor->x<1) thor->x=1;
if(thor->y<0) thor->y=0;
thor->last_x[0]=thor->x;
thor->last_x[1]=thor->x;
thor->last_y[0]=thor->y;
thor->last_y[1]=thor->y;
thor->dir=thor_info.last_dir;
thor->last_dir=thor_info.last_dir;
thor->health=thor_info.last_health;
thor_info.magic=thor_info.last_magic;
thor_info.jewels=thor_info.last_jewels;
thor_info.keys=thor_info.last_keys;
thor_info.score=thor_info.last_score;
thor_info.item=thor_info.last_item;
thor_info.inventory=thor_info.last_inventory;
thor_info.object=thor_info.last_object;
thor_info.object_name=thor_info.last_object_name;
thor->num_moves=1;
thor->vunerable=60;
thor->show=60;
hourglass_flag=0;
apple_flag=0;
bomb_flag=0;
thunder_flag=0;
lightning_used=0;
tornado_used=0;
shield_on=0;
actor[1].used=0;
actor[2].used=0;
thor->speed_count=6;
movedata(FP_SEG(sd_data+(new_level*512)),FP_OFF(sd_data+(new_level*512)),
        FP_SEG(&scrn),FP_OFF(&scrn),sizeof(LEVEL));
display_health();
display_magic();
display_jewels();
display_keys();
display_score();
display_item();
current_level=new_level;
show_level(new_level);
}
//===========================================================================
void pause(int delay){

timer_cnt=0;
while(timer_cnt<delay) rotate_pal();
}
//===========================================================================
void rotate_pal(void){


if(slow_mode){
  while(vbl_cnt<4);
  vbl_cnt=0;
}
xshowpage(display_page);
}
//===========================================================================
int rnd(int max){
int r;

if(demo || rdemo || record){
  r=rnd_array[rnd_index] % max;
  rnd_index++;
  if(rnd_index>99) rnd_index=0;
  return r;
}
return rand() % max;
}
