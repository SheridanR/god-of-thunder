#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "res_man.h"

FILE *res_fp;
RES_HEADER res_header[RES_MAX_ENTRIES];
int16_t res_active=0;
int16_t res_changed;
char far *res_lzss_buff;
//==========================================================================
void res_init(char far *buff){

    // TODO res_init is this randomize() call necessary?
    //randomize();
    res_fp=0;
    res_active=1;
    res_changed=0;
    res_lzss_buff=buff;
}
//==========================================================================
int16_t res_open(char *filename){

    if(!res_active) return RES_NOT_ACTIVE;
    if(!(res_fp=fopen(filename,"rb+"))) return RES_CANT_OPEN;
    if(fread(&res_header,1,RES_MAX_ENTRIES*sizeof(RES_HEADER),
        res_fp)!=RES_MAX_ENTRIES*sizeof(RES_HEADER)){
        fclose(res_fp);
        return RES_CANT_READ;
    }
    res_decrypt((char far *) &res_header,RES_MAX_ENTRIES*sizeof(RES_HEADER),128);
    res_changed=0;
    return 1;
}
//==========================================================================
int16_t res_close(void){

    if(!res_active) return RES_NOT_ACTIVE;
    if(res_changed){
        res_encrypt((char far *) &res_header,RES_MAX_ENTRIES*sizeof(RES_HEADER),128);
        if(fseek(res_fp,0l,SEEK_SET)) return RES_CANT_SEEK;
        if(fwrite(&res_header,1,RES_MAX_ENTRIES*sizeof(RES_HEADER),
            res_fp)!=RES_MAX_ENTRIES*sizeof(RES_HEADER)){
            fclose(res_fp);
            return RES_CANT_WRITE;
        }
    }
    if(res_fp) if(!fclose(res_fp)) return 1;
    return RES_CANT_CLOSE;
}
//==========================================================================
void res_encrypt(char far *buff,int32_t len,char key){

    while(len){
        len--;
        *buff^=key;
        buff++;
        key++;
    }
}
//==========================================================================
void res_decrypt(char far *buff,int32_t len,char key){

    while(len){
        len--;
        *buff^=key;
        buff++;
        key++;
    }
}
