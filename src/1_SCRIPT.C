//Source code released to the public domain on March 27th, 2020.

//=========================== Include Files ===============================
#include <stdlib.h>
#include <stdio.h>
#include <alloc.h>
#include <ctype.h>
#include <bios.h>
#include <dos.h>
#include <string.h>
#include <dir.h>
#include <conio.h>

#include <1_define.h>
#include <1_proto.h>
//====================== Functions Declarations============================
int  read_script_file(void);
void script_error(int err_num);
int  get_command(void);
int  skip_colon(void);
int  calc_value(void);
int  get_next_val(void);
int  calc_string(int mode);
void get_str(void);
int  get_internal_variable(void);
int  exec_command(int num);
void script_entry(void);
void script_exit(void);
//============================ Externals ==================================
extern ACTOR *thor;
extern THOR_INFO thor_info;
extern int current_level;
extern char odin[4][262];
extern char  *tmp_buff;
extern char far *sd_data;
extern char cheat;
extern volatile char key_flag[100];
extern int key_magic;
extern char *object_names[];
extern SETUP setup;
extern char area;
extern unsigned int display_page,draw_page;
extern ACTOR actor[MAX_ACTORS];
extern int new_level,current_level,new_level_tile;
extern LEVEL scrn;
extern char far objects[NUM_OBJECTS][262];
extern char object_map[240];
extern char object_index[240];
extern int thunder_flag;
//============================= Globals ==================================
long  far num_var[26];        //numeric variables
char  far str_var[26][81];    //string vars
char  far line_label[32][9];  //line label look up table
char  far *line_ptr[32];      //line label pointers
char  far *new_ptr;
int   num_labels;             //number of labels
char  far *gosub_stack[32];   //stack for GOSUB return addresses
char  gosub_ptr;              //GOSUB stack pointer
char  far *for_stack[10];     //FOR stack
long  for_val[10];            //current FOR value
char  for_var[10];            //ending FOR value (target var)
char  for_ptr;	              //FOR stack pointer
char  far *buff_ptr;          //pointer to current command
char  far *buff_end;	      //pointer to end of buffer
char  far *buffer;            //buffer space (malloc'ed)
long  scr_index;
char  *scr_pic;
long  lvalue;
long  ltemp;
char  far temps[255];

#define SCR_BUFF_SIZE 5000
char  far *scr_command[]={"!@#$%","END","GOTO","GOSUB","RETURN","FOR","NEXT",
                          "IF","ELSE","RUN",
                          "ADDJEWELS","ADDHEALTH","ADDMAGIC","ADDKEYS",
                          "ADDSCORE","SAY","ASK","SOUND","PLACETILE",
                          "ITEMGIVE","ITEMTAKE","ITEMSAY","SETFLAG","LTOA",
                          "PAUSE","TEXT","EXEC","VISIBLE","RANDOM",
                          NULL};

char  far *internal_variable[]={"@JEWELS","@HEALTH","@MAGIC","@SCORE",
                                "@SCREEN","@KEYS",
                                "@OW","@GULP","@SWISH","@YAH","@ELECTRIC",
                                "@THUNDER","@DOOR","@FALL","@ANGEL","@WOOP",
                                "@DEAD","@BRAAPP","@WIND","@PUNCH","@CLANG",
                                "@EXPLODE","@FLAG","@ITEM","@THORTILE",
                                "@THORPOS",NULL};

char  far *scr_error[]={"!@#$%","Out of Memory","Can't Read Script",
                        "Too Many Labels","No END",
                        "Syntax","Out of Range","Undefined Label",
                        "RETURN Without GOSUB","Nesting",
                        "NEXT Without FOR",NULL};
#define ERROR_MAX 10
//============================ Functions ==================================
void execute_script(long index,char *pic){
int i,ret,re_execute;


for(i=0;i<MAX_ACTORS;i++) actor[i].show=0;
xdisplay_actors(&actor[MAX_ACTORS-1],draw_page);
xshowpage(draw_page);
xerase_actors(actor,display_page);
xdisplay_actors(&actor[MAX_ACTORS-1],display_page);
xshowpage(display_page);
xerase_actors(actor,draw_page);

scr_index=index;
scr_pic=pic;

_fmemset(&num_var,0,4*26);
_fmemset(&str_var,0,81*26);

re_execute=0;
run_script:                            //jump point for RUN command

_fmemset(line_label,0,32*9);            //clear line label buffer
_fmemset(line_ptr,0,4*32);               //clear line ptrs

memset(gosub_stack,0,52);              //clear gosub stack
gosub_ptr=0;
for(i=0;i<11;i++){
   for_var[i]=0;
   for_val[i]=0;
   for_stack[i]=0;
}
for_ptr=0;

i=read_script_file();
if(i!=0){
  script_error(i);
  script_exit();
  return;
}
if(!re_execute) script_entry();

buff_ptr=buffer;
while(1){
     if(cheat && key_flag[_B]) break;
     ret=get_command();
     if(ret==-1) break;       //ignore NO END error
     else if(ret==-2){
       script_error(5);       // syntax error
       break;
     }
     else if(ret>0) {
       ret=exec_command(ret);
       if(ret==-100){         //RUN command
         re_execute=1;
         if(buffer) farfree(buffer);
         goto run_script;
       }
      if(!ret) break;
     }
}
script_exit();
return;
}
//=========================================================================
void script_entry(void){

}
//=========================================================================
void script_exit(void){

//xshowpage(display_page);
if(buffer) farfree(buffer);
}
//=========================================================================
int  skip_colon(void){

while(*buff_ptr==0 || *buff_ptr==':'){
  buff_ptr++;
  if(buff_ptr>buff_end) return 0;
}
return 1;
}
//=========================================================================
int get_command(void){
int ret,i,len;

if(!skip_colon()) return -1;

i=0;
while(1){
     if(!scr_command[i]) break;           //lookup command
     len=_fstrlen(scr_command[i]);
     if(!_fstrncmp(buff_ptr,(char far *) scr_command[i],len)){
       buff_ptr+=len;
       return i;
     }
     i++;
}
if(isalpha(*buff_ptr)){
  if(*(buff_ptr+1)=='='){           //num var assignment
    i=(*buff_ptr)-65;
    buff_ptr+=2;
    ret=calc_value();
    if(!ret) return -2;
    else{num_var[i]=lvalue;return 0;}
  }
  else if(*(buff_ptr+1)=='$' && *(buff_ptr+2)=='='){
    i=(*buff_ptr)-65;
    buff_ptr+=3;
    ret=calc_string(0);                 //string var assignment
    if(ret==0) return -2;
    else if(ret==-1) return -3;
    if(_fstrlen(temps)>80) return -3;
    _fstrcpy(str_var[i],temps);
    return 0;
  }
}
return -2;
}
//=========================================================================
int  calc_string(int mode){   //if mode==1 stop at comma
char varstr[255];
char varnum;

strcpy(varstr,"");

if(!skip_colon()) return 0;

strloop:
if(*buff_ptr=='"'){
  get_str();
  if(strlen(varstr)+_fstrlen(temps) < 255)
    _fstrcat((char far *) varstr,temps);
  goto nextstr;
}
if(isalpha(*buff_ptr)){
  if(*(buff_ptr+1)=='$'){
    varnum=(*buff_ptr)-65;
    if(strlen(varstr)+_fstrlen(str_var[varnum]) < 255)
      _fstrcat((char far *) varstr,str_var[varnum]);
    buff_ptr+=2;
    goto nextstr;
  }
}
return 0;

nextstr:
if(*buff_ptr==0 || *buff_ptr==58){
  buff_ptr++;
  goto strdone;
}
if(*buff_ptr==',' && mode==1) goto strdone;

if(*buff_ptr=='+'){
  buff_ptr++;
  goto strloop;
}
return 0;

strdone:
if(strlen(varstr)>255) return -1;
_fstrcpy(temps,(char far *) varstr);
return 1;
}
//=========================================================================
void get_str(void){
int t;

buff_ptr++;
t=0;
while(1){
  if(*buff_ptr=='"' || *buff_ptr==0){
    temps[t]=0;
    if(*buff_ptr=='"') buff_ptr++;
    return;
  }
  temps[t++]=*buff_ptr;
  buff_ptr++;
}
}
//=========================================================================
int calc_value(void){
long tmpval2;
char exptype;
char ch;

tmpval2=0;
exptype=1;

while(1){
if(!get_next_val()) return 0;
switch(exptype){
      case 0:
           tmpval2=tmpval2*ltemp;
           break;
      case 1:
           tmpval2=tmpval2+ltemp;
           break;
      case 2:
           tmpval2=tmpval2-ltemp;
           break;
      case 3:
           if(ltemp!=0) tmpval2=tmpval2/ltemp;
           break;
}
ch=*buff_ptr;
switch(ch){
      case 42:
           exptype=0;                       /* multiply */
           break;
      case 43:
           exptype=1;                       /* add */
           break;
      case 45:
           exptype=2;                       /* minus */
           break;
      case 47:
           exptype=3;                       /* divide */
           break;
      default:
           lvalue=tmpval2;
           return 1;
}
buff_ptr++;
}
}
//=========================================================================
int get_next_val(void){
char ch;
char tmpstr[25];
int t;

ch=*buff_ptr;
if(ch==0 || ch==':') return 0;
if(ch==64) return get_internal_variable();

if(isalpha(ch)){
  buff_ptr++;
  ltemp=num_var[ch-65];
  return 1;
}
t=0;
if(strchr("0123456789-",ch)){
  tmpstr[0]=ch;
  t++;
  buff_ptr++;
  while(strchr("0123456789",*buff_ptr) && *buff_ptr!=0){
       tmpstr[t]=*buff_ptr;
       buff_ptr++;
       t++;
  }
  tmpstr[t]=0;
  if(t>10) return 0;
  ltemp=atol(tmpstr);
  return 1;
}
return 0;
}
//=========================================================================
int get_internal_variable(void){
int i,len;
char b;
char *sp;

i=0;
while(1){
     if(!internal_variable[i]) return 0;         //lookup internal variable
     len=_fstrlen(internal_variable[i]);
     if(!_fstrncmp(buff_ptr,internal_variable[i],len)){
       buff_ptr+=len;
       break;
     }
     i++;
}
switch(i){
      case 0:
           ltemp=thor_info.jewels;
           break;
      case 1:
           ltemp=thor->health;
           break;
      case 2:
           ltemp=thor_info.magic;
           break;
      case 3:
           ltemp=thor_info.score;
           break;
      case 4:
           ltemp=current_level;
           break;
      case 5:
           ltemp=thor_info.keys;
           break;
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
           ltemp=(long) (i-5l);
           break;
      case 22:
           if(!calc_value()) return 0;
           i=(int) lvalue;
           if(i<1 || i>64) return 0;
           sp=(char *) &setup;
           sp+=(i/8);
           b=1;
           b=b << (i%8);
           if(*sp & b) ltemp=1;
           else ltemp=0;
           break;
      case 23:
           if(thor_info.inventory & 64) ltemp=thor_info.object;
           else ltemp=0;
           break;
      case 24:
           ltemp=scrn.icon[(thor->y+8)/16][(thor->x+7)/16];
           break;
      case 25:
           ltemp=(((thor->y+8)/16)*20)+((thor->x+7)/16);
           break;
      default:
           return 0;
}
return 1;
}
//=========================================================================
int get_line(char far *src,char far *dst){
int cnt;

cnt=0;
while(*src!=13){
   if(*src!=10){
     *dst=*src;
     dst++;
   }
   cnt++;
   src++;
}
*dst=0;
cnt++;
src++;
return cnt;
}
//=========================================================================
int read_script_file(void){
char temp_buff[255];
char quote_flag;
int  i,len,p,ret,cnt;
char ch;
char tmps[255];
char far *sb;
char far *sbuff;
char str[21];

buffer=farmalloc(SCR_BUFF_SIZE);
if(!buffer) {ret=1;goto done;};
buff_ptr=buffer;
_fmemset(buffer,0,SCR_BUFF_SIZE);

sbuff=farmalloc(25000l);
if(!sbuff) {ret=1;goto done;};
sb=sbuff;

itoa(area,tmps,10);
strcpy(str,"SPEAK");
strcat(str,tmps);
if(res_read(str,sb)<0) {ret=6;goto done;}

ltoa(scr_index,str,10);
strcpy(temp_buff,"|");
strcat(temp_buff,str);

while(1){
  cnt=get_line(sb,(char far *)tmps);
  sb+=cnt;
  if(!strcmp(tmps,"|EOF")){ret=2;goto done;}
  if(!strcmp(tmps,temp_buff)) break;
}
num_labels=0;
while(1){
     cnt=get_line(sb,(char far *)tmps);
     if(!strcmp(tmps,"|STOP")){
       if(buff_ptr!=buffer) {buff_end=buff_ptr;ret=0;goto done;}
       ret=2;
       goto done;
     }
     sb+=cnt;
     len=_fstrlen(tmps);
     if(len<2){
       *buff_ptr=0;
       buff_ptr++;
       continue;
     }
     quote_flag=0;
     p=0;
     for(i=0;i<len;i++){
        ch=tmps[i];
        if(ch==34) quote_flag^=1;
        else if(ch==13 || ch==10){  //check for CR
          temp_buff[p]=0;
          break;
        }
        else if((ch==39 || ch==96)  && !quote_flag){
          temp_buff[p]=0;
          break;
        }
        if(!quote_flag) ch=toupper(ch);
        if(quote_flag || ch>32){
          temp_buff[p++]=ch;
        }
     }
     temp_buff[p]=0;

     len=strlen(temp_buff);
     if(len<10 && temp_buff[len-1]==':'){       //line label
       temp_buff[len-1]=0;
       line_ptr[num_labels]=buff_ptr;
       _fstrcpy(line_label[num_labels++],(char far *) temp_buff);
       if(num_labels>31){ret=3;goto done;}
       *buff_ptr=0;
       buff_ptr++;
       continue;
     }
     _fstrcpy(buff_ptr,(char far *) temp_buff);
     buff_ptr+=strlen(temp_buff);
     *buff_ptr=0;
     buff_ptr++;
}
done:
if(sbuff) farfree(sbuff);
return ret;
}
//=========================================================================
void script_error(int err_num){
int  line_num;
char s[17];
char far *tb;
char ts[81];

line_num=1;
tb=buffer;

while(1){
  if(*tb==0) line_num++;
  if(tb>=buff_ptr) break;
  tb++;
}

if(err_num>ERROR_MAX) err_num=5;       //unknown=syntax

_fstrcpy((char far *) ts,scr_error[err_num]);
itoa(line_num,s,10);
strcat(ts," in Line #");
strcat(ts,s);

if(cheat){
  play_sound(DEAD,1);
  xfillrectangle(0,0,320,10,PAGES,0);
  xprint(0,0,ts,PAGES,14);
}
}
//=========================================================================
int get_string(void){

_fmemset(temps,0,255);
if(*buff_ptr=='"'){
  get_str();
}
else if(isalpha(*buff_ptr)){
  if((*buff_ptr+1)=='$'){
    _fstrcpy(temps,str_var[(*buff_ptr)-65]);
    buff_ptr+=2;
  }
}
else return 5;

if(_fstrlen(temps)>80) return 6;
return 0;
}
//=========================================================================
int cmd_goto(void){
int i,len;
char s[255];
char *p;

_fstrcpy(s,buff_ptr);
p=strchr(s,':');
if(p) *p=0;

for(i=0;i<num_labels;i++){
   len=strlen(s);
   if(len==0) break;
   if(!_fstrcmp((char far *) s,line_label[i])){
     new_ptr=line_ptr[i];
     buff_ptr+=len;
     return 0;
   }
}
return 8;
}
//=========================================================================
int cmd_if(void){
long tmpval1,tmpval2;
char exptype,ch;

if(!calc_value()) return 5;
tmpval1=lvalue;
exptype=*buff_ptr;
buff_ptr++;

ch=*buff_ptr;
if(ch==60 || ch==61 || ch==62){
  if(exptype==*buff_ptr) return 5;
  exptype+=*buff_ptr;
  buff_ptr++;
}
if(!calc_value()) return 5;
tmpval2=lvalue;
buff_ptr+=4;
switch(exptype){
      case 60:                              /* less than */
           if(tmpval1<tmpval2) goto iftrue;
           goto iffalse;
      case 61:                              /* equal */
           if(tmpval1==tmpval2) goto iftrue;
           goto iffalse;
      case 62:                              /* greater than */
           if(tmpval1>tmpval2) goto iftrue;
           goto iffalse;
      case 121:                              /* less than or equal */
           if(tmpval1<=tmpval2) goto iftrue;
           goto iffalse;
      case 122:                              /* less or greater (not equal) */
           if(tmpval1!=tmpval2) goto iftrue;
           goto iffalse;
      case 123:                              /* greater than or equal */
           if(tmpval1>=tmpval2) goto iftrue;
           goto iffalse;
      default:
           return 5;
}

iffalse:
while(*buff_ptr!=0) buff_ptr++;
while(*buff_ptr==0) buff_ptr++;

if(!_fstrncmp(buff_ptr,"ELSE",4)) buff_ptr+=4;

iftrue:
return 0;
}
//=========================================================================
int cmd_run(void){

if(!calc_value()) return 5;
buff_ptr++;
scr_index=lvalue;
return -100;
}
//=========================================================================
int cmd_addjewels(void){

if(!calc_value()) return 5;
buff_ptr++;
add_jewels((int) lvalue);
return 0;
}
//=========================================================================
int cmd_addhealth(void){

if(!calc_value()) return 5;
buff_ptr++;
add_health((int) lvalue);
return 0;
}
//=========================================================================
int cmd_addmagic(void){

if(!calc_value()) return 5;
buff_ptr++;
add_magic((int) lvalue);
return 0;
}
//=========================================================================
int cmd_addkeys(void){

if(!calc_value()) return 5;
buff_ptr++;
add_keys((int) lvalue);
return 0;
}
//=========================================================================
int cmd_addscore(void){

if(!calc_value()) return 5;
buff_ptr++;
add_score((int) lvalue);
return 0;
}
//=========================================================================
int cmd_say(int mode,int type){
char far *p;
int obj;

if(mode){
  if(!calc_value()) return 5;
  buff_ptr++;
  obj=(int) lvalue;
  if(obj<0 || obj>32) return 6;
  if(obj) obj+=10;
}
else obj=0;

memset(tmp_buff,0,TMP_SIZE);
p=(char far *) tmp_buff;
while(calc_string(0)){
     _fstrcpy((char far *) p,temps);
     p+=_fstrlen(temps);
     *(p)=10;
     p++;
}
*(p-1)=0;
display_speech(obj,(char *) scr_pic,type);
d_restore();
return 0;
}
//=========================================================================
int cmd_ask(void){
int i,v,p;
char title[41];
char *op[]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
char opts[10][41];

memset(tmp_buff,0,TMP_SIZE);
memset(opts,0,10*41);

if(!skip_colon()) return 5;

if(isalpha(*buff_ptr)){
  v=*buff_ptr-65;
  buff_ptr++;
  if(*buff_ptr!=',') return 5;
  buff_ptr++;
}
else return 5;

if(!calc_string(1)) return 5;
_fstrncpy((char far *) title,temps,41);
title[40]=0;

if(*buff_ptr==','){
  buff_ptr++;
  if(!calc_value()) return 5;
  buff_ptr++;
  p=(int) lvalue;
}
else return 5;

i=0;
while(calc_string(0)){
     _fstrncpy((char far *) opts[i],temps,41);
     opts[i][40]=0;
     op[i]=opts[i];
     i++;
     if(i>9) return 3;
}
if(p>i) p=0;
num_var[v]=select_option(op,title,p-1);
d_restore();
return 0;
}
//=========================================================================
int cmd_sound(void){

if(!calc_value()) return 5;
buff_ptr++;
if(lvalue<1 || lvalue>16) return 6;
play_sound((int) lvalue-1,1);
return 0;
}
//=========================================================================
int cmd_settile(void){
int screen,pos,tile;
LEVEL far *lvl;

if(!calc_value()) return 5;
buff_ptr++;
screen=(int) lvalue;
if(!calc_value()) return 5;
buff_ptr++;
pos=(int) lvalue;
if(!calc_value()) return 5;
tile=(int) lvalue;
if(screen<0 || screen>119) return 6;
if(pos<0 || pos>239) return 6;
if(tile<0 || tile>230) return 6;
if(screen==current_level){
  place_tile(pos%20,pos/20,tile);
}
else{
  lvl=(LEVEL far *) (sd_data+(screen*512));
  lvl->icon[pos/20][pos%20]=tile;
}
return 0;
}
//=========================================================================
int cmd_itemgive(void){
int i;

if(!calc_value()) return 5;
buff_ptr++;
i=(int) lvalue;
if(i<1 || i > 15) return 6;

thor_info.inventory|=64;
thor_info.item=7;
thor_info.object=i;
display_item();
thor_info.object_name=object_names[thor_info.object-1];
return 0;
}
//=========================================================================
int cmd_itemtake(void){

delete_object();
return 0;
}
//=========================================================================
int cmd_setflag(void){
int i;
char b;
char *sp;

if(!calc_value()) return 5;
i=(int) lvalue;
if(i<1 || i>64) return 6;
sp=(char *) &setup;
sp+=(i/8);
b=1;
b=b << (i%8);
*sp|=b;
return 0;
}
//=========================================================================
int cmd_ltoa(void){
int sv;
char str[21];

if(!calc_value()) return 5;
buff_ptr++;

if(isalpha(*buff_ptr)){
  if(*(buff_ptr+1)=='$'){
    sv=(*buff_ptr)-65;
    buff_ptr+=2;
  }
  else return 5;
}
else return 5;

ltoa(lvalue,str,10);
_fstrcpy(str_var[sv],(char far *) str);
return 0;
}
//=========================================================================
int cmd_pause(void){

if(!calc_value()) return 5;
buff_ptr++;
if(lvalue<1 || lvalue>65535l) return 6;

pause((int) lvalue);
return 0;
}
//=========================================================================
int cmd_visible(void){

if(!calc_value()) return 5;
buff_ptr++;
if(lvalue<1 || lvalue>16) return 6;

actor_visible((int) lvalue);
return 0;
}
//=========================================================================
int cmd_random(void){
int v,r;

if(isalpha(*buff_ptr)){
  v=*buff_ptr-65;
  buff_ptr++;
  if(*buff_ptr!=',') return 5;
  buff_ptr++;
}
else return 5;

if(!calc_value()) return 5;
buff_ptr++;
r=(int) lvalue;
if(r<1 || r>1000) return 6;

num_var[v]=rand() % r;
return 0;
}
//=========================================================================
void scr_func1(void){

play_sound(FALL,1);
if(key_flag[_FOUR]) return;
new_level=109;
new_level_tile=215;
thor->x=(new_level_tile%20)*16;
thor->y=((new_level_tile/20)*16)-2;

thor->last_x[0]=thor->x;
thor->last_x[1]=thor->x;
thor->last_y[0]=thor->y;
thor->last_y[1]=thor->y;
thor->show=2;
}
//=========================================================================
char far *offense[]={"Cussing","Rebellion","Kissing Your Mother Goodbye",
                     "Being a Thunder God","Door-to-Door Sales",
                     "Carrying a Concealed Hammer",
                    };

char far *reason[]={"We heard you say 'Booger'.",
                    "You look kind of rebellious.",
                    "Your mother turned you in.",
                    "We don't want you here.",
                    "Nobody wants your sweepers.",
                    "That's a dangerous weapon.",
                   };
//=========================================================================
void scr_func2(void){
int r;

r=rnd(6);
_fstrcpy(str_var[0],offense[r]);
_fstrcpy(str_var[1],reason[r]);
}
//=========================================================================
void scr_func3(void){
int p,x,y,o;

p=(((thor->y+8)/16)*20)+((thor->x+7)/16);
y=p/20;
x=p%20;
if(y<0 || x<0 || y>11 || x>19){
  play_sound(BRAAPP,1);
  while(key_flag[key_magic]) rotate_pal();
  return;
}
if(scrn.icon[y][x]<174 || scrn.icon[y][x]>178){
  play_sound(BRAAPP,1);
  while(key_flag[key_magic]) rotate_pal();
  return;
}
num_var[0]=1;
play_sound(WOOP,1);
if(current_level==106 && p==69){
  place_tile(x,y,220);
  while(key_flag[key_magic]) rotate_pal();
  return;
}

while(key_flag[key_magic]) rotate_pal();
place_tile(x,y,191);
if((rand()%100) < 25 || (current_level==13 && p==150 && !setup.f26 && setup.f28)){
  if(!object_map[p] && scrn.icon[y][x]>=140){  //nothing there and solid
    o=(rand()%5)+1;
    if(current_level==13 && p==150 && !setup.f26 && setup.f28) o=20;
    object_map[p]=o;
    object_index[p]=31;  //actor is 3-15
    x=(p%20)*16;
    y=(p/20)*16;
    xfput(x,y,PAGE2,(char far *) objects[o-1]);
    xcopyd2d(x,y,x+16,y+16,x,y,PAGE2,draw_page,320,320);
    xcopyd2d(x,y,x+16,y+16,x,y,PAGE2,display_page,320,320);
    pause(30);
  }
}
}
//=========================================================================
void scr_func4(void){

thunder_flag=60;
}
//=========================================================================
void scr_func5(void){

scrn.actor_loc[0]-=2;
scrn.actor_loc[1]-=2;
scrn.actor_loc[2]-=2;
scrn.actor_loc[3]-=2;
actor[3].i1=16;
}
//=========================================================================
void scr_func1(void);

void (*scr_func[])() =
     { scr_func1,
       scr_func2,
       scr_func3,
       scr_func4,
       scr_func5,
     };
//=========================================================================
int cmd_exec(void){

if(!calc_value()) return 5;
buff_ptr++;
if(lvalue<1 || lvalue>10) return 6;

scr_func[(int) (lvalue-1)]();
return 0;
}
//=========================================================================
int exec_command(int num){
int ret;
char ch;

ret=0;
switch(num){
      case 1:                          //end
        return 0;
      case 2:                          //goto
        ret=cmd_goto();
        if(!ret) buff_ptr=new_ptr;
        break;
      case 3:                          //gosub
        ret=cmd_goto();
        if(!ret){
          gosub_ptr++;
          if(gosub_ptr>31){ret=10;break;}
          gosub_stack[gosub_ptr]=buff_ptr;
          buff_ptr=new_ptr;
        }
        break;
      case 4:                         //return
        if(!gosub_ptr) {ret=9;break;}
        buff_ptr=gosub_stack[gosub_ptr--];
        break;
      case 5:                         //for
        for_ptr++;
        if(for_ptr>10) {ret=10;break;}
        ch=*buff_ptr;
        if(!isalpha(ch)){ret=5;break;}
        ch-=65;
        for_var[for_ptr]=ch;
        buff_ptr+=2;
        if(!calc_value()) {ret=5;break;}
        num_var[for_var[for_ptr]]=lvalue;
        buff_ptr+=2;
        if(!calc_value()) {ret=5;break;}
        for_val[for_ptr]=lvalue;
        for_stack[for_ptr]=buff_ptr;
        break;
      case 6:                         //next
        if(!for_ptr) {ret=11;break;}
        num_var[for_var[for_ptr]]=num_var[for_var[for_ptr]]+1;
        if(num_var[for_var[for_ptr]]<=for_val[for_ptr])
          buff_ptr=for_stack[for_ptr];
        else for_ptr--;
        break;
      case 7:                         //if
        ret=cmd_if();
        break;
      case 8:                         //else
        while(*buff_ptr!=0) buff_ptr++;
        break;
      case 9:                         //run
        ret=cmd_run();
        if(ret<0) return -100;
        break;
      case 10:                        //addjewels
        ret=cmd_addjewels();
        break;
      case 11:                        //addhealth
        ret=cmd_addhealth();
        break;
      case 12:                        //addmagic
        ret=cmd_addmagic();
        break;
      case 13:                        //addkeys
        ret=cmd_addkeys();
        break;
      case 14:                        //addscore
        ret=cmd_addscore();
        break;
      case 15:                        //say
        ret=cmd_say(0,1);
        break;
      case 16:                        //ask
        ret=cmd_ask();
        break;
      case 17:                        //sound
        ret=cmd_sound();
        break;
      case 18:                        //settile
        ret=cmd_settile();
        break;
      case 19:                        //itemgive
        ret=cmd_itemgive();
        break;
      case 20:                        //itemtake
        ret=cmd_itemtake();
        break;
      case 21:                        //itemsay
        ret=cmd_say(1,1);
        break;
      case 22:                        //setflag
        ret=cmd_setflag();
        break;
      case 23:                        //ltoa
        ret=cmd_ltoa();
        break;
      case 24:                        //pause
        ret=cmd_pause();
        break;
      case 25:                        //text
        ret=cmd_say(0,0);
        break;
      case 26:                        //exec
        ret=cmd_exec();
        break;
      case 27:                        //visible
        ret=cmd_visible();
        break;
      case 28:                        //random
        ret=cmd_random();
        break;
      default:
        ret=5;
}
if(ret>0) {script_error(ret);return 0;}
return 1;
}
