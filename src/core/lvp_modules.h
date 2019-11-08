#ifndef _LVP_MODULES_H_
#define _LVP_MODULES_H_
#include "lvp_core.h"

extern LVPModule lvp_reader_module;
extern LVPModule lvp_pkt_filter;
extern LVPModule lvp_audio_pkt_cache_module;
extern LVPModule lvp_video_pkt_cache_module;
extern LVPModule lvp_audio_frame_cache_module;
extern LVPModule lvp_video_frame_cache_module;

//all  module 
static LVPModule *LVPModules[]={
    &lvp_reader_module,
    &lvp_pkt_filter,
    &lvp_audio_pkt_cache_module,
    &lvp_video_pkt_cache_module,
    &lvp_audio_frame_cache_module,
    &lvp_video_frame_cache_module,
};
#endif