#ifndef _LVP_PKT_FILTER_H_
#define _LVP_PKT_FILTER_H_
#include "../core/lvp_core.h"

typedef struct lvp_pkt_filter{
    LVPLog *log;
    LVPEventControl *ctl;

    LVPList *modules;
}LVPPktFilter;

#endif