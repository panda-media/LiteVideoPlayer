#ifndef _LVP_HWACCEL_DECODER_H_
#define _LVP_HWACCEL_DECODER_H_
#include "lvp_decoder.h"

typedef struct lvp_hwaccel_info{
    int hwaccel_num;
    char *hwaccel_name;
}LVPHwaccelInfo;

int lvp_set_up_hwaccel_decoder(LVPDecoder* decoder);

#endif