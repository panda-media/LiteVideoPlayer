#ifndef _LVP_MODULES_H_
#define _LVP_MODULES_H_
#include "lvp_core.h"

extern LVPModule lvp_reader_module;
extern LVPModule lvp_pkt_filter;
extern LVPModule lvp_frame_filter;
extern LVPModule lvp_audio_pkt_cache_module;
extern LVPModule lvp_video_pkt_cache_module;
extern LVPModule lvp_sub_pkt_cache_module;
extern LVPModule lvp_audio_frame_cache_module;
extern LVPModule lvp_video_frame_cache_module;
extern LVPModule lvp_sub_frame_cache_module;
extern LVPModule lvp_audio_decoder;
extern LVPModule lvp_video_decoder;
extern LVPModule lvp_sub_decoder;
extern LVPModule lvp_video_render;
extern LVPModule lvp_audio_render;
extern LVPModule lvp_avsync_module;
extern LVPModule lvp_audio_resample;

//all  module 
static LVPModule *LVPModules[]={
    &lvp_reader_module,
    &lvp_pkt_filter,
    &lvp_frame_filter,
    &lvp_audio_pkt_cache_module,
    &lvp_video_pkt_cache_module,
	&lvp_sub_pkt_cache_module,
    &lvp_audio_frame_cache_module,
    &lvp_video_frame_cache_module,
	&lvp_sub_frame_cache_module,
    &lvp_audio_decoder,
    &lvp_video_decoder,
	&lvp_sub_decoder,
	&lvp_video_render,
	&lvp_audio_render,
	&lvp_avsync_module,
	&lvp_audio_resample,
};
#endif