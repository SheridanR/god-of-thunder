#include <stdio.h>

#include "define.h"
#include "proto.h"
#include "res_man.h"
#include "new/new.h"
//==========================================================================
int32_t res_decode(char far *buff){
    return lzss_decompress(res_lzss_buff,buff);
}
