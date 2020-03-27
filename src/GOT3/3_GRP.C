// THOR - The God of Thunder
//Source code released to the public domain on March 27th, 2020.

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <ctype.h>

#include <res_man.h>
#include <3_define.h>
#include <3_proto.h>
//===========================================================================
extern char far text[94][72];
extern union REGS in,out;
char pbuff[768];
extern char dialog_color[16];
extern char cheat;
extern unsigned int display_page,draw_page;
extern volatile char key_flag[100];
extern ACTOR actor[MAX_ACTORS];
//===========================================================================
void xprint(int x,int y,char *string,unsigned int page,int color){
char ch;
char str[4];

str[3]=0;
x &= 0xfffc;
while(*string){
   ch=*string++;
   if(ch=='~' && isxdigit(*string)){
     ch=*string++;
     if(isdigit(ch)) ch-=48;
     else{
       ch=toupper(ch)-55;
     }
     color=dialog_color[ch];
     continue;
   }
   if(ch>31 && ch<127){
     xtext1(x,y,page,text[ch-32],0);
     xtext(x,y,page,text[ch-32],color);
   }
   x+=8;
}
}
//===========================================================================
void xprintx(int x,int y,char *string,unsigned int page,int color){
char ch;
char str[4];
int c;

c=26;
str[3]=0;
x &= 0xfffc;
while(*string){
   ch=*string++;
   if(ch=='~' && isxdigit(*string)){
     ch=*string++;
     if(isdigit(ch)) ch-=48;
     else{
       ch=toupper(ch)-55;
     }
     color=dialog_color[ch];
     continue;
   }
   if(ch>31 && ch<127){
     xtextx(x,y-1,page,text[ch-32],c);
     xtextx(x,y+1,page,text[ch-32],c);
     xtextx(x+1,y,page,text[ch-32],c);
     xtextx(x-1,y,page,text[ch-32],c);
     xtextx(x-1,y-1,page,text[ch-32],c);
     xtextx(x+1,y+1,page,text[ch-32],c);
     xtextx(x+1,y-1,page,text[ch-32],c);
     xtextx(x-1,y+1,page,text[ch-32],c);
     xtext(x,y,page,text[ch-32],color);
   }
   x+=8;
}
}
//===========================================================================
void split_screen(void){

asm{

mov dx,0x03d4    //CRTC
mov al,0x18      //select LINE COMPARE reg
out dx,al
inc dx
mov ax,128       //set bits 0-7 of LINE COMPARE
                 //I believe this is what I changed
                 //I am loading the LINE compare with
                 //384 (192x2) (128 here + 256 in bit 8)
                 //since mode X displays each scan line
                 //twice. I was originally loading
                 //385, since that was the first
                 //scan line I wanted for the upper
                 //half of the screen.
out dx,al
dec dx
mov al,7         //OVERFLOW reg
out dx,al
inc dx
in  al,dx
or  al,0x10      //set bit 8 of LINE COMPARE
out dx,al
dec dx
mov al,9
out dx,al
inc dx
in  al,dx
and al,0xbf      //clear bit 9 of LINE COMPARE
out dx,al
}
}
//===========================================================================
int load_palette(void){
int i;
char r,g,b,n;
#define DAC_READ_INDEX	03c7h
#define DAC_WRITE_INDEX 03c8h
#define DAC_DATA	03c9h


if(res_read("palette",(char far *) pbuff)<0) return 0;
for(i=0;i<768;i++) pbuff[i]=pbuff[i] >> 2;

for(i=240;i<244;i++){
   *(pbuff+((i*3)+0))=0;
   *(pbuff+((i*3)+1))=0;
   *(pbuff+((i*3)+2))=0x3b;
}
for(i=244;i<248;i++){
   *(pbuff+((i*3)+0))=0x3b;
   *(pbuff+((i*3)+1))=0;
   *(pbuff+((i*3)+2))=0;
}
in.h.al=0x10;
in.h.ah=0x10;
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
return 1;
}
/*=========================================================================*/
void xbox(int x1,int y1,int x2,int y2,unsigned page,int color){

xline(x1,y1,x2,y1,page,color);
xline(x1,y2,x2,y2,page,color);
xline(x1,y1,x1,y2,page,color);
xline(x2,y1,x2,y2,page,color);
}
/*=========================================================================*/
void xline(int x0,int y0,int x1,int y1,int page,int color){
int x,y;

if(x0==x1 && y0==y1) xpset(x0, y0, page, color);

else if(abs(x1-x0) >= abs(y1-y0)){
     if(x1<x0){
       x=x1;
       y=y1;
       x1=x0;
       y1=y0;
       x0=x;
       y0=y;
     }
     for(x=x0;x<=x1;x++){
        y=(int) (y0+((x-x0)*(long)(y1-y0))/(x1-x0));
        xpset(x, y, page, color);
     }
}
else{
  if(y1<y0){
    x=x1;
    y=y1;
    x1=x0;
    y1=y0;
    x0=x;
    y0=y;
  }
  for(y=y0;y<=y1;y++){
     x=(int) (x0+((y-y0)*(long)(x1-x0))/(y1-y0));
     xpset(x, y, page, color);
  }
}
}
/*=========================================================================*/
void fade_in(void){

pal_fade_in(pbuff);
}
/*=========================================================================*/
void fade_out(void){

pal_fade_out(pbuff);
}
//===========================================================================
void unsplit_screen(void){

asm{

mov dx,0x03d4    //CRTC
mov al,0x18      //select LINE COMPARE reg
out dx,al
inc dx
mov ax,255      //set bits 0-7 of LINE COMPARE
out dx,al
dec dx
mov al,7         //OVERFLOW reg
out dx,al
inc dx
in  al,dx
or  al,0x10      //set bit 8 of LINE COMPARE
out dx,al
dec dx
mov al,9
out dx,al
inc dx
in  al,dx
or  al,64
//and al,0xbf      //clear bit 9 of LINE COMPARE
out dx,al
}
}
//==========================================================================
void screen_dump(void){

if(cheat){
  if(display_page!=PAGE0) return;
  play_sound(YAH,1);
  unsplit_screen();
  xshowpage(display_page);
  xcopyd2d(0,0,320,48,0,192,PAGES,display_page,320,320);
  while(key_flag[_K]);
  while(!key_flag[ESC]);
  split_screen();
  xcopyd2d(0,0,320,48,0,193,display_page,display_page,320,320);
}
}
//===========================================================================
void d_restore(void){

xdisplay_actors(&actor[MAX_ACTORS-1],draw_page);
xshowpage(draw_page);
xcopyd2d(0,0,320,192,0,0,PAGE2,display_page,320,320);
xdisplay_actors(&actor[MAX_ACTORS-1],display_page);
xshowpage(display_page);
xerase_actors(actor,draw_page);
}
