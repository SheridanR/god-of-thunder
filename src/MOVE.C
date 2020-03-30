// THOR - The God of Thunder
// See LICENSE for details

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "define.h"
#include "proto.h"
//===========================================================================
extern uint16_t draw_page,display_page;
extern char pge;
extern int16_t exit_flag;
extern volatile char key_flag[100];
extern char break_code;
extern char scan_code;
extern int16_t new_level,current_level;
extern LEVEL scrn;
extern char *scrnp;

extern char *sd_data;
extern char *bg_pics;
extern char diag;
extern ACTOR *thor;
extern int16_t thor_x1,thor_y1,thor_x2,thor_y2;
extern ACTOR *hammer;
extern ACTOR actor[MAX_ACTORS];
extern ACTOR enemy[MAX_ENEMIES];  //current enemies
extern ACTOR shot[MAX_ENEMIES];   //current shots
extern char enemy_type[MAX_ENEMIES];

extern ACTOR explosion;
extern ACTOR sparkle;
extern char shot_ok;
extern char *std_sound_start;
extern int16_t max_shot;
extern THOR_INFO thor_info;
extern char object_map[240];
extern int16_t shield_on,hourglass_flag;
extern char cheat;
extern int16_t killgg_inform;
extern SETUP setup;
extern int16_t thunder_flag;

extern int16_t (*movement_func[]) (ACTOR *actr);
extern int16_t (*shot_movement_func[]) (ACTOR *actr);
extern int16_t (*shot_pattern_func[]) (ACTOR *actr);
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
int16_t point_within(int16_t x,int16_t y,int16_t x1,int16_t y1,int16_t x2,int16_t y2){

    if((x>=x1) && (x<=x2) && (y>=y1) && (y<=y2)) return 1;
    return 0;
}
//===========================================================================
int16_t  overlap(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t x3,int16_t y3,int16_t x4,int16_t y4){

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
int16_t reverse_direction(ACTOR *actr){

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
int16_t kill_good_guy(void){

    if(!killgg_inform && !thunder_flag){
        odin_speaks(2010,0);
        killgg_inform=1;
    }
    add_score(-1000);
    return 0;
}
//===========================================================================
void actor_damaged(ACTOR *actr,int16_t damage){

    if(!setup.skill) damage*=2;
    else if(setup.skill==2) damage/=2;

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
    int16_t damage;

    actr->hit_thor=1;
    if(cheat) if(key_flag[_FOUR]) return;
    damage=actr->strength;
    if(damage!=255){
        if(!setup.skill) damage/=2;
        else if(setup.skill==2) damage*=2;
    }
    if((!thor->vunerable && !shield_on) || damage==255){
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
    int16_t x,y,x1,y1,r,n,t;

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
        actr->type=(char)t;
        actr->actor_num=(char)n;
        actr->rating=(char)r;
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
int16_t _actor_shoots(ACTOR *actr,int16_t dir){
    int16_t t,i,cx,cy;
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
            if(cy>174) cy=174;
            if(cx>304) cx=304;
            act->x=cx;
            act->y=cy;
            act->last_dir=(char)dir;
            act->next=0;
            act->dir=(char)dir;
            if(act->directions==1) act->dir=0;
            else if(act->directions==4 && act->frames==1){
                act->next=(char)dir;
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
            actr->shot_actor=(char)i;
            actr->num_shots++;
            actr->shot_cnt=20;
            shot_ok=0;
            return 1;
        }
    }
    return 0;
}
//===========================================================================
void actor_always_shoots(ACTOR *actr,int16_t dir){


    _actor_shoots(actr,dir);
}
//===========================================================================
int16_t actor_shoots(ACTOR *actr,int16_t dir){
    int16_t i,cx,cy,tx,ty;
    int16_t icn;

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
    int16_t i;

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
        if(i!=actr->dir) actr->dir=(char)i;

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


