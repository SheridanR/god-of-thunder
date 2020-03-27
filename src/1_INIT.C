// THOR - The God of Thunder
//Source code released to the public domain on March 27th, 2020.

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <alloc.h>
#include <string.h>
#include <dir.h>
#include <conio.h>
#include <ctype.h>

#include <res_man.h>
#include <1_define.h>
#include <1_proto.h>


int setup_level(void);
int setup_player(void);
void ask_joystick(void);
void display_copyright(void);
//===========================================================================
extern volatile unsigned int timer_cnt,extra_cnt;
extern char far text[94][72];
extern union REGS in,out;
extern char far *bg_pics;
extern char far *sd_data;
extern struct sup setup;
extern char far *mask_buff;
extern char far *mask_buff_start;
extern char *ami_buff;
extern char abuff[AMI_LEN];

extern volatile char key_flag[100];
extern volatile char joy_flag[100];
extern volatile char tmp_flag[100];
extern char break_code,scan_code,last_scan_code;
extern char slow_mode;
extern unsigned int page3_offset;
extern ACTOR actor[MAX_ACTORS];
extern ACTOR *thor;
extern char boss_loaded;

extern char far *std_sound_start;
extern char far *pcstd_sound_start;
extern char far *std_sound;
extern char far *pcstd_sound;
extern char far *object_sound[26];

extern char far *music_start;
extern char far *music;
extern char far *music_buffer;
extern char far *song;
extern int  rnd_array[];
extern char demo_key[];

extern THOR_INFO thor_info;
extern unsigned int joy_x,joy_y;
extern char joy_b1,joy_b2;
extern int  joystick,joylx,joyly,joyhx,joyhy;

extern char *tmp_buff;
extern int  key_fire,key_up,key_down,key_left,key_right,key_magic,key_select;
extern char level_type;
extern char far *lzss_buff;

//globals
static void interrupt far (*old_keyboard_int)(void);   // interrupt func pointer
extern void interrupt far (*old_timer_int)(void);      // interrupt function pointer
void interrupt far timer_int(void);
extern char far *bleep;
extern char far *boss_sound[3];
extern char far *boss_pcsound[3];
extern char res_file[];
extern char far *pc_sound[NUM_SOUNDS];
extern char far *dig_sound[NUM_SOUNDS];
extern int current_level;
extern char odin[4][262];
extern char hampic[4][262];
extern int load_game_flag;
extern int music_flag,sound_flag,pcsound_flag;
extern long pcsound_length[NUM_SOUNDS];
extern char demo_key[];
extern int  demo_cnt;
extern char demo,record;
extern char demo_enable;
extern int exit_flag;
extern char story_flag;
extern unsigned int display_page;
extern char music_current;
extern char tempstr[];
extern char area,cheat;
extern char pbuff[768];

//char scanc [100];
char spic1,spic2;
char byte;
unsigned int word;
void print_mem(void);
//===========================================================================
char *err_msg[]={"(null)","Can't Load GOTRES.DAT","Can't Read Font",
                 "Can't Load Palette","Can't Load Sounds","am Out Of Memory",
                 "Can't Load Status","Can't Find Sound Card","Can't INIT Sound",
                 "Can't INIT Music","Can't INIT Player","Can't INIT Level",
                 "Don't Know Where I Am","Can't Find Your Hammer",
                 "Can't Load Resource File","Can't Demo Game",
                 "Can't Randomize",
                 };

//===========================================================================
int initialize(void){
int i;

lzss_buff=(char far*) 0;
bg_pics=(char far*) 0;
sd_data=(char far*) 0;
mask_buff_start=(char far*) 0;
tmp_buff=(char *) 0;

song=(char far*) 0;
for(i=0;i<3;i++){
   boss_sound[i]=(char far*) 0;
   boss_pcsound[i]=(char far*) 0;
}
//commandeer the KB int
old_keyboard_int=getvect(0x09);
setvect(0x09,keyboard_int);
break_code=1;

//setup default values
memset(&setup,0,sizeof(SETUP));
setup.music=music_flag;
setup.dig_sound=sound_flag;
setup.pc_sound=pcsound_flag;
if(sound_flag) setup.pc_sound=0;
setup.scroll_flag=1;
setup.speed=slow_mode;
setup.skill=1;

memset(key_flag,0,100);
memset(joy_flag,0,100);
memset(tmp_flag,0,100);
sd_data=(char far*) 0;

key_fire=ALT;
key_up=UP;
key_down=DOWN;
key_left=LEFT;
key_right=RIGHT;
key_magic=CTRL;
key_select=SPACE;

bg_pics=(char far *) 0;
sd_data=(char far *) 0;
memset(&actor[0],0,(sizeof(ACTOR) * MAX_ACTORS));
boss_sound[0]=0;
boss_sound[1]=0;
boss_sound[2]=0;
boss_pcsound[0]=0;
boss_pcsound[1]=0;
boss_pcsound[2]=0;

std_sound_start=0;
pcstd_sound_start=0;

tmp_buff=malloc(TMP_SIZE);
if(!tmp_buff) return 5;

lzss_buff=farmalloc(18000lu);
if(!lzss_buff) return 5;

mask_buff=farmalloc(15300lu);
mask_buff_start=mask_buff;
if(!mask_buff) return 5;

memset(abuff,0,AMI_LEN);
ami_buff=(char *) abuff;

//setup_filenames(0);   //out

//init game
//ultoa(coreleft(),ts,10);
//printf("\r\n%s<-",ts);
//wait_response();

res_init(lzss_buff);
if(res_open("GOTRES.DAT")<0) return 1;

if(res_read("RANDOM",(char far*)rnd_array)<0) return 15;
if(res_read("DEMO",(char far*)demo_key)<0) return 16;
if(res_read("TEXT",(char far*)text)<0) return 2;
if(res_read("ODINPIC",(char far*)odin)<0) return 12;
if(res_read("HAMPIC",(char far*) hampic)<0) return 13;

if(!load_palette()) return 5;

xsetmode();
xshowpage(PAGE1);

sbfx_init();  //) return 7;
if(setup.dig_sound || setup.pc_sound){
  if(!sound_init()){
    setup.dig_sound=0;
    setup.pc_sound=0;
    sound_flag=0;
    pcsound_flag=0;
  }
}
if(!setup.music){
  if(music_init()){
   setup.music=0;
   music_flag=0;
  }
}
song=farmalloc(20000lu);
if(!song) return 5;

//if(res_read("OPENBACK",(char far*)back)<0) return 12;
if(story_flag && !cheat) story();
//story();
fade_out();

//for(y=0;y<15;y++){
//   for(i=0;i<20;i++) xput(i*16,y*16,PAGE1,back[0]);
//}
xprintx(13*8,13*8,"God Of Thunder",PAGE1,14);
xprintx(8*8,15*8,"Part I: Serpent Surprise",PAGE1,32);

fade_in();
timer_cnt=0;

//split_screen();
if(!init_status_panel()) return 6;
load_standard_actors();
page3_offset=PAGE3;

//res_abort();
//setup_filenames(area);

if(!load_objects()) return 11;
if(!setup_player()) return 10;
if(!setup_level()) return 11;
while(timer_cnt<240);
fade_out();
split_screen();
return 0;
}
//===========================================================================
void exit_code(int ex_flag){
int i;

res_close();
sound_exit();
sbfx_exit();

setvect(0x09,old_keyboard_int);
if(lzss_buff) farfree(lzss_buff);
if(bg_pics) farfree(bg_pics);
if(sd_data) farfree(sd_data);
if(mask_buff_start) farfree(mask_buff_start);
if(tmp_buff) free(tmp_buff);

if(song) farfree(song);
for(i=0;i<3;i++){
   if(boss_sound[i]) farfree(boss_sound[i]);
   if(boss_pcsound[i]) farfree(boss_pcsound[i]);
}

in.x.ax=0x0003;
int86(0x10,&in,&out);

if(ex_flag>0){
  printf("\r\n\r\nOdin Says: Verily, I %s\r\n\r\n",err_msg[ex_flag]);
  exit(ex_flag);
}
}
//===========================================================================
void interrupt keyboard_int(){
char flag;
int key;
//static int num=0;

scan_code=inportb(0x60);
byte=inportb(0x61);
byte |= 0x80;
outportb(0x61,byte);
byte &= 0x7f;
outportb(0x61,byte);
//scanc[num++]=scan_code;
//if(scan_code==0xe0) goto done;

if(scan_code==last_scan_code) goto done;
last_scan_code=scan_code;

if(record){
  if(demo_cnt<DEMO_LEN){
    if(!demo_key[demo_cnt]) demo_key[demo_cnt]=scan_code;
    else demo_key[demo_cnt+1]=scan_code;
  }
}
if(!demo && !record){
  if(scan_code & 0x80) flag=0;
  else flag=1;
  key=(scan_code &0x7f);
  if(key==key_up || key==key_down || key==key_left || key==key_right ||
     key==key_fire || key==key_magic){
     if(joystick) tmp_flag[key]=flag;
     else key_flag[key]=flag;
  }
  else key_flag[key]=flag;
}
else if(demo) exit_flag=5;

done:
outportb(0x20,0x20);
}
//===========================================================================
void demo_key_set(void){
char sc,flag;

if(demo_key[demo_cnt]){
  sc=demo_key[demo_cnt];
  if(sc & 0x80) flag=0;
  else flag=1;
  sc&=0x7f;
  if(sc==key_up || sc==key_down || sc==key_left || sc==key_right ||
     sc==key_fire || sc==key_magic){
     if(joystick) tmp_flag[sc]=flag;
     else key_flag[sc]=flag;
  }
  else key_flag[sc]=flag;
}
}
//===========================================================================
void wait_not_response(void){

while(key_flag[key_fire] || key_flag[ENTER] || key_flag[SPACE]
       || key_flag[key_magic]  || key_flag[ESC]){
     if(cheat && key_flag[_K]) screen_dump();
     rotate_pal();
     if(joystick){
       joy_key();
       merge_keys();
     }
}
}
//===========================================================================
int wait_response(void){

wait_not_response();
while(1){
     if(cheat && key_flag[_K]) screen_dump();
     if(key_flag[key_fire])   return key_fire;
     else if(key_flag[key_magic])  return key_magic;
     else if(key_flag[ENTER]) return ENTER;
     else if(key_flag[SPACE]) return SPACE;
     else if(key_flag[ESC])   return ESC;
     rotate_pal();
     if(joystick){
       joy_key();
       merge_keys();
     }
}
}
//===========================================================================
int get_response(void){

if(joystick){
  joy_key();
  merge_keys();
}
if(cheat && key_flag[_K]) screen_dump();
if(key_flag[key_fire])   return key_fire;
else if(key_flag[ENTER]) return ENTER;
else if(key_flag[SPACE]) return SPACE;
else if(key_flag[key_magic])  return key_magic;
else if(key_flag[ESC])   return ESC;
return 0;
}
//===========================================================================
void wait_not_key(int index){

while(key_flag[index]){
     if(cheat && key_flag[_K]) screen_dump();
     if(key_flag[ESC]) return;
     rotate_pal();
     if(joystick){
       joy_key();
       merge_keys();
     }
}
}
//===========================================================================
void joy_key(void){

if(!joystick) return;
read_joystick();

if(joy_x<joylx) joy_flag[key_left]=1;
else joy_flag[key_left]=0;

if(joy_x>joyhx) joy_flag[key_right]=1;
else joy_flag[key_right]=0;

if(joy_y<joyly) joy_flag[key_up]=1;
else joy_flag[key_up]=0;

if(joy_y>joyhy) joy_flag[key_down]=1;
else joy_flag[key_down]=0;

if(joy_b1) joy_flag[key_fire]=1;
else joy_flag[key_fire]=0;

if(joy_b2) joy_flag[key_magic]=1;
else joy_flag[key_magic]=0;
}
//===========================================================================
void merge_keys(void){

key_flag[key_up]=tmp_flag[key_up] | joy_flag[key_up];
key_flag[key_down]=tmp_flag[key_down] | joy_flag[key_down];
key_flag[key_left]=tmp_flag[key_left] | joy_flag[key_left];
key_flag[key_right]=tmp_flag[key_right] | joy_flag[key_right];
key_flag[key_fire]=tmp_flag[key_fire] | joy_flag[key_fire];
key_flag[key_magic]=tmp_flag[key_magic] | joy_flag[key_magic];
}
//===========================================================================
int setup_level(void){

if(!load_bg_data()) return 0;
if(load_game_flag!=1) if(!load_sd_data()) return 0;

return 1;
}
//===========================================================================
int setup_player(void){

memset(&thor_info,0,sizeof(THOR_INFO));
thor_info.inventory=0;  //511;
if(area>1) thor_info.inventory |=APPLE_MAGIC + LIGHTNING_MAGIC;
if(area>2) thor_info.inventory |=BOOTS_MAGIC + WIND_MAGIC;

thor->health=150;
thor_info.magic=0;
thor_info.jewels=0;
thor_info.score=0;
thor_info.keys=0;
thor_info.last_item=0;
thor_info.object=0;
thor_info.object_name=NULL;
thor->x=152;
thor->y=96;
thor->last_x[0]=thor->x;
thor->last_x[1]=thor->x;
thor->last_y[0]=thor->y;
thor->last_y[1]=thor->y;
thor_info.last_icon=(6*20)+8;
thor_info.last_screen=23;
thor->dir=1;
display_health();
display_magic();
display_jewels();
display_keys();
display_score();

return 1;
}
//===========================================================================
int setup_boss(int num){
int rep;
char s[21];
char str[21];
char ress[21];

if(boss_loaded==num) return 1;
if(boss_loaded){
  REPEAT(3){
    if(boss_sound[rep]) farfree(boss_sound[rep]);
    if(boss_pcsound[rep]) farfree(boss_pcsound[rep]);
  }
}

itoa(num,s,10);
strcpy(str,"BOSSV");
strcat(str,s);

strcpy(ress,str);
strcat(ress,"1");

boss_sound[0]=res_falloc_read(ress);
if(!boss_sound[0]) return 0;
dig_sound[NUM_SOUNDS-3]=boss_sound[0];

strcpy(ress,str);
strcat(ress,"2");
boss_sound[1]=res_falloc_read(ress);
if(!boss_sound[1]) return 0;
dig_sound[NUM_SOUNDS-2]=boss_sound[1];

strcpy(ress,str);
strcat(ress,"3");
boss_sound[2]=res_falloc_read(ress);
if(!boss_sound[2]) return 0;
dig_sound[NUM_SOUNDS-1]=boss_sound[2];

itoa(num,s,10);
strcpy(str,"BOSSP");
strcat(str,s);
if(num==2) strcpy(str,"BOSSP1");

strcpy(ress,str);
strcat(ress,"1");
boss_pcsound[0]=res_falloc_read(ress);
if(!boss_pcsound[0]) return 0;
pc_sound[NUM_SOUNDS-3]=boss_pcsound[0];
pc_sound[NUM_SOUNDS-3][0]=0;
pc_sound[NUM_SOUNDS-3][1]=0;
num=res_find_name(ress);
if(num<0) return 0;
pcsound_length[NUM_SOUNDS-3]=res_header[num].length;

strcpy(ress,str);
strcat(ress,"2");
boss_pcsound[1]=res_falloc_read(ress);
if(!boss_pcsound[1]) return 0;
pc_sound[NUM_SOUNDS-2]=boss_pcsound[1];
pc_sound[NUM_SOUNDS-2][0]=0;
pc_sound[NUM_SOUNDS-2][1]=0;
num=res_find_name(ress);
if(num<0) return 0;
pcsound_length[NUM_SOUNDS-2]=res_header[num].length;

strcpy(ress,str);
strcat(ress,"3");
boss_pcsound[2]=res_falloc_read(ress);
if(!boss_pcsound[2]) return 0;
pc_sound[NUM_SOUNDS-1]=boss_pcsound[2];
pc_sound[NUM_SOUNDS-1][0]=0;
pc_sound[NUM_SOUNDS-1][1]=0;
num=res_find_name(ress);
if(num<0) return 0;
pcsound_length[NUM_SOUNDS-1]=res_header[num].length;
boss_loaded=num;
return 1;
}
//===========================================================================
void display_copyright(void){

}
//===========================================================================
void ask_joystick(void){

}
//===========================================================================
void set_joy(void){
char *p;

p=strtok(tempstr, ",:");
joylx=atoi(p);

p=strtok(NULL, ",");
joyhx=atoi(p);

p=strtok(NULL, ",");
joyly=atoi(p);

p=strtok(NULL, ",");
joyhy=atoi(p);

joystick=1;

}
//===========================================================================
void story_animate(void){
//static int cnt=0;

return;
/*
if(extra_cnt>3){
  cnt++;
  extra_cnt=0;
}
if(cnt==3 || cnt==6){
  spic2++;
  if(spic2>3) spic2=0;
  xfillrectangle(24,96,40,112,38400u,28);
  xput(24,96,38400u,hampic[spic2]);
}
if(cnt==6){
  spic1++;
  if(spic1>3) spic1=1;
  xfillrectangle(146,62,162,78,19200u,28);
  xput(146,62,19200u,odin[spic1]);
  cnt=0;
}
*/
}
//===========================================================================
int story_wait(void){
int key;

while(1){
     story_animate();
     key=get_response();
     if(key){
       while(get_response());
       break;
     }
}
return key;
}
//===========================================================================
void set_palette(void){
int i;
char r,g,b,n;
#define DAC_READ_INDEX	03c7h
#define DAC_WRITE_INDEX 03c8h
#define DAC_DATA	03c9h


for(i=0;i<256;i++){
   n=i;
   r=pbuff[i*3];
   g=pbuff[(i*3)+1];
   b=pbuff[(i*3)+2];
   asm mov  al,n
   asm mov  dx,DAC_WRITE_INDEX  // Tell DAC what colour index to
   asm out  dx,al               // write to
   asm mov  dx,DAC_DATA

   asm mov  bl,r
   asm mov  al,bl              // Set the red component
   asm out  dx,al

   asm mov  ch,g
   asm mov  al,ch              // Set the green component
   asm out  dx,al

   asm mov  cl,b
   asm mov  al,cl              // Set the blue component
   asm out  dx,al
}
}
//===========================================================================
void story(void){
int i,x,y,color;
char s[21];
char str[40];
char back[4][262];
char *p;
unsigned int pg;
unsigned int add;
int key;
char far *buff;

fade_out();
xfillrectangle(0,0,320,240,38400u,0);
xshowpage(38400u);
if(res_read("OPENSONG",(char far*)song)<0) return;
if(res_read("STORY1",(char far*)tmp_buff)<0) return;
if(res_read("OPENBACK",(char far*)back)<0) return;
if(res_read("STORYPIC",(char far*)back)<0) return;
p=tmp_buff;
pg=0u;

buff=farmalloc(15000u);
if(!buff) return;
pg=0u;
for(i=0;i<12;i++){
   if(i==6){
     pg=19200u;
   }
   itoa(i+1,s,10);
   strcpy(str,"OPENP");
   strcat(str,s);
   res_read(str,buff);
   if(i<6) xfarput(0,i*40,pg,buff);
   else xfarput(0,(i-6)*40,pg,buff);
}


farfree(buff);
pg=0u;

res_read("STORYPAL",pbuff);

pbuff[2]=0;
pbuff[1]=0;
pbuff[0]=0;

set_palette();

i=0;
x=8;
y=2;
color=72;

pg=0u;
while(i<46){
   if(*p==13){
     x=8;
     y+=10;
     i++;
     if(i==23){
       pg=19200u;
       y=2;
     }
     if(i==48){
       pg=38400u;
       y=2;
     }
   }
   else if(*p=='/' && *(p+4)=='/'){
     p++;
     s[0]=*p++;
     s[1]=*p++;
     s[2]=*p++;
     s[3]=0;
     color=atoi(s);
   }
   else if(*p!=10){
     xtextx(x-1,y-1,pg,text[(*p-32)],255);
     xtextx(x+1,y+1,pg,text[(*p-32)],255);
     xtextx(x-1,y+1,pg,text[(*p-32)],255);
     xtextx(x+1,y-1,pg,text[(*p-32)],255);
     xtextx(x,y-1,pg,text[(*p-32)],255);
     xtextx(x,y+1,pg,text[(*p-32)],255);
     xtextx(x-1,y,pg,text[(*p-32)],255);
     xtextx(x+1,y,pg,text[(*p-32)],255);
     xtext(x,y,pg,text[(*p-32)],color);
     x+=8;
   }
   p++;
}
xput(24,88,19200u,back[1]);
xput(146,64,0u,back[0]);

pg=0u;
fade_out();
xshowpage(pg);
music_current=0;
music_play(0,1);
fade_in();

timer_cnt=0;
add=0;
while(timer_cnt<60);
i=0;
add=0;
extra_cnt=0;
spic1=0;
spic2=0;
while(i<800){
  display_page=add;
  if(add==0u || add==19200u){
    if(story_wait()==ESC) break;
    if(add==19200u) break;
  }
  key=get_response();
  if(key){
    while(get_response()) story_animate();
    if(key==ESC) break;
    else story_wait();
  }
  story_animate();
  xshowpage(add);
  add+=80;
  i++;
}
story_flag=0;
fade_out();

xfillrectangle(0,0,320,240,0u,0);
xfillrectangle(0,0,320,240,19200u,0);
xfillrectangle(0,0,320,240,38400u,0);
xfillrectangle(0,0,320,240,57600u,0);
load_palette();
music_pause();
music_current=-1;

xshowpage(PAGE1);
}
