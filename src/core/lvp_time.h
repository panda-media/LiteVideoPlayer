#ifndef _LVP_TIME_H_
#define _LVP_TIME_H_

#include "lvp_core.h"

static void inline lvp_sleep(uint64_t ms){
#ifdef LVP_LINUX
    usleep(ms*1000);
#endif

#ifdef LVP_WIN32
    Sleep(ms);
#endif
}

#endif