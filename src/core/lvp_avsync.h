#ifndef _LVP_AVSYNC_H_
#define _LVP_AVSYNC_H_

#include "lvp_core.h"
#include <libavformat/avformat.h>

typedef struct lvp_avsync
{
    LVPEventControl *ctl;
    LVPLog *log;

    uint64_t audio_time;
    uint64_t video_time;
    uint64_t sub_time;
    lvp_thread sync_thread_t;
    int sync_run;
    int has_video; 
    int has_audio;
    uint64_t ex_time;

    AVFrame *aframe;
    AVFrame *vframe;
    AVFrame *sframe;

    LVPEvent *req_event;
    LVPEvent *update_audio_event;
    LVPEvent *update_video_event;
}LVPAVSync;


#endif