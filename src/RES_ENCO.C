#include <stdio.h>

#include "res_man.h"
//==========================================================================
int32_t res_decode(char far *buff){


    return lzss_decompress(res_lzss_buff,buff);
}
