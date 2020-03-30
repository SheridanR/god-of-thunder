#include <stdio.h>
#include <string.h>

#include "res_man.h"
//==========================================================================
int16_t res_find_name(char *name){
    int16_t i;

    if(!res_active) return RES_NOT_ACTIVE;
    if(!res_fp) return RES_NOT_OPEN;

    _strupr(name);
    for(i=0;i<RES_MAX_ENTRIES;i++){
        if(!strcmp(name,res_header[i].name)) return i;
    }
    return RES_ENTRY_NOT_FOUND;
}
//==========================================================================
int16_t res_find_empty(void){
    int16_t i;

    if(!res_active) return RES_NOT_ACTIVE;
    if(!res_fp) return RES_NOT_OPEN;

    for(i=0;i<RES_MAX_ENTRIES;i++){
        if(res_header[i].length==0) return i;
    }
    return RES_FULL;
}
