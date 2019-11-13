#ifndef _LVP_FRAME_FILTER_H_
#define _LVP_FRAME_FILTER_H_

#include "../core/lvp_core.h"

typedef struct lvp_frame_filter
{
    LVPLog *log;
    LVPEventControl *ctl;
    LVPList *modules;

}LVPFrameFilter;


#endif