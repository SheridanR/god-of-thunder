#include <stdio.h>
#include <stdlib.h>

#include "define.h"
#include "proto.h"
#include "res_man.h"
//==========================================================================
void far *res_falloc_read(char *name){
    int16_t num;
    void far *fp;

    if(!res_active) return 0;
    if(!res_fp) return 0;

    num=res_find_name(name);
    if(num<0) return 0;

    fp=farmalloc(res_header[num].original_size);
    if(!fp) return 0;

    res_read(name,fp);
    return fp;
}
