#ifndef _LVP_PKT_FILTER_H_
#define _LVP_PKT_FILTER_H_
#include "../core/lvp_core.h"
#include <libavcodec/avcodec.h>

typedef struct lvp_pkt_filter{
    LVPLog *log;
    LVPEventControl *ctl;

	AVPacket* filtered_pkt;

    LVPList *modules;
}LVPPktFilter;

#endif