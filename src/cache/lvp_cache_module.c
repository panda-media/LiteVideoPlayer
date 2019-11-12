#include "lvp_cache_module.h"


static void cache_custom_pkt_free(void *data, void *usrdata){
    av_packet_free((AVPacket **)&data);
}
static void cache_custom_frame_free(void *data, void *usrdata){
    av_frame_free((AVFrame**)&data);
}

static int handle_pkt(LVPEvent *ev, void *usr_data){
    LVPCache *cache = (LVPCache*)usr_data;

    AVPacket *pkt = (AVPacket*)ev->data;
    if(cache->handle_stream->index != pkt->stream_index){
        return LVP_OK;
    }
    int64_t size = 0;
    lvp_queue_size(cache->data,&size);
    if(size >= cache->max_size){
        printf("CACHE FULL\n");
        return LVP_E_NO_MEM;
    }

    AVPacket *refpkt =  av_packet_clone(ev->data);
    if(refpkt == NULL){
        lvp_error(cache->log,"clone packet error",NULL);
        return LVP_E_FATAL_ERROR;
    }

    int ret = lvp_queue_push(cache->data,refpkt,NULL,cache_custom_pkt_free,1);
    if(ret == LVP_FALSE){
        return LVP_E_FATAL_ERROR;
    }
    return LVP_OK;
}

static int handle_req_pkt(LVPEvent *ev, void *usrdata){
    LVPCache *cache = (LVPCache*)usrdata;
    int* type = (int*)ev->data;
    if(*type != cache->media_type){
        return LVP_OK;
    }
    int64_t size = 0;
    lvp_queue_size(cache->data,&size);
    if(size==0){
        printf("CACHE FULL\n");
        return LVP_E_NO_MEM;
    }
    AVPacket *p = NULL;
    LVP_BOOL ret = lvp_queue_front(cache->data,(void **)&p,NULL);
    if(ret == LVP_FALSE){
        return LVP_E_FATAL_ERROR;
    }
    AVPacket *ref = av_packet_clone(p);
    ev->data = ref;
    lvp_queue_pop(cache->data);
    av_packet_free(&p);
    return LVP_OK;
}

static int handle_frame(LVPEvent *ev, void *usr_data){
    LVPCache *cache = (LVPCache*)usr_data;

    int64_t size = 0;
    lvp_queue_size(cache->data,&size);
    if(size >= cache->max_size){
        return LVP_E_NO_MEM;
    }

    AVFrame *refframe = av_frame_clone(ev->data);
    if(refframe== NULL){
        lvp_error(cache->log,"clone frame error",NULL);
        return LVP_E_FATAL_ERROR;
    }

    int ret = lvp_queue_push(cache->data,refframe,NULL,cache_custom_frame_free,1);
    if(ret == LVP_FALSE){
        return LVP_E_FATAL_ERROR;
    }

    return LVP_OK;
}

static int handle_req_frame(LVPEvent *ev, void *usrdata){
    LVPCache *cache = (LVPCache*)usrdata;
    int64_t size = lvp_queue_size(cache->data,&size);
    if(size==0){
        return LVP_E_NO_MEM;
    }
    AVFrame *f = NULL;
    lvp_queue_front(cache->data,(void**)&f,NULL);
    AVFrame *ref = av_frame_clone(f);
    ev->data = ref;
    lvp_queue_pop(cache->data);
    av_frame_free(&f);
    return LVP_OK;
}

static int init_pkt_cache(LVPCache *cache){
    int ret = lvp_event_control_add_listener(cache->ctl,LVP_EVENT_FILTER_SEND_PKT,handle_pkt,cache);
    if(ret!=LVP_OK){
        lvp_error(cache->log,"listener %s error",LVP_EVENT_FILTER_SEND_PKT);
    }
    ret = lvp_event_control_add_listener(cache->ctl,LVP_EVENT_REQ_PKT,handle_req_pkt,cache);
    if(ret != LVP_OK){
        lvp_error(cache->log,"listener %s error",LVP_EVENT_REQ_PKT);
    }
    return LVP_OK;
}

static int init_frame_cache(LVPCache *cache){
    int ret = lvp_event_control_add_listener(cache->ctl,LVP_EVENT_FILTER_SEND_FRAME,handle_frame,cache);
    if(ret != LVP_OK){
        lvp_error(cache->log,"listenner %s error",LVP_EVENT_FILTER_SEND_FRAME);
    }
    ret = lvp_event_control_add_listener(cache->ctl,LVP_EVENT_REQ_FRAME,handle_req_frame,cache);
    if(ret != LVP_OK){
        lvp_error(cache->log,"listener %s error",LVP_EVENT_REQ_FRAME);
    }
    return LVP_OK;
}

static int handle_seek(LVPEvent *ev, void *usrdata){
    LVPCache *cache = (LVPCache*)usrdata;
    LVP_BOOL ret = lvp_queue_clear(cache->data);
    if(ret == LVP_TRUE){
        return LVP_OK;
    }else{
        return LVP_E_FATAL_ERROR;
    }
}

static int handle_select_stream(LVPEvent *ev, void *usrdata){
    LVPCache *cache = (LVPCache*)usrdata;
    AVStream *stream = (AVStream*)ev->data;
    if(cache->media_type == stream->codecpar->codec_type){
        cache->handle_stream = ev->data;
    }
    return LVP_OK;
}

static int init(struct lvp_module *module, 
                                    LVPMap *options,
                                    LVPEventControl *ctl,
                                    LVPLog *log){
    assert(module);
    assert(ctl);
    LVPCache *cache = (LVPCache*)module->private_data;
    if(!strcmp(module->name,"LVP_AUDIO_PKT_CACHE")){
        cache->type = CACHE_TYPE_PKT;
        cache->media_type = AVMEDIA_TYPE_AUDIO;
    }
    else if(!strcmp(module->name,"LVP_VIDEO_PKT_CACHE")){
        cache->type = CACHE_TYPE_PKT;
        cache->media_type = AVMEDIA_TYPE_VIDEO;
    }
    else if(!strcmp(module->name,"LVP_AUDIO_FRAME_CACHE")){
        cache->type = CACHE_TYPE_FRAME;
        cache->media_type = AVMEDIA_TYPE_AUDIO;
    }
    else{
        cache->type = CACHE_TYPE_FRAME;
        cache->media_type = AVMEDIA_TYPE_VIDEO;
    }
    cache->log = lvp_log_alloc(module->name);
    cache->log->log_call = log->log_call;
    cache->log->usr_data = log->usr_data;
    cache->ctl = ctl;

    cache->data = lvp_queue_create();

    int ret = LVP_OK;
    if(cache->type == CACHE_TYPE_FRAME){
        cache->max_size = FRAME_SIZE;
        ret = init_frame_cache(cache);
    }
    else{
        cache->max_size = PKT_SIZE;
        ret = init_pkt_cache(cache);
    }

    if(ret!=LVP_OK){
        return ret;
    }

    ret = lvp_event_control_add_listener(cache->ctl,LVP_EVENT_SEEK,handle_seek,cache);
    if(ret != LVP_OK){
        return ret;
    }

    ret = lvp_event_control_add_listener(cache->ctl,LVP_EVENT_SELECT_STREAM,handle_select_stream,cache);
    
    return ret;

}

static void module_close(struct lvp_module *module){
    assert(module);
    LVPCache *cache = (LVPCache*)module->private_data;
    if(cache->data){
        lvp_queue_free(cache->data);
    }
}
                            


LVPModule lvp_audio_pkt_cache_module = {
    .version = lvp_version,
    .name = "LVP_AUDIO_PKT_CACHE",
    .type = LVP_MODULE_CORE,
    .private_data_size = sizeof(LVPCache),
    .private_data = NULL,
    .module_init = init, 
    .module_close = module_close,
};

LVPModule lvp_video_pkt_cache_module = {
    .version = lvp_version,
    .name = "LVP_VIDEO_PKT_CACHE",
    .type = LVP_MODULE_CORE,
    .private_data_size = sizeof(LVPCache),
    .private_data = NULL,
    .module_init = init, 
    .module_close = module_close,
};

LVPModule lvp_audio_frame_cache_module = {
    .version = lvp_version,
    .name = "LVP_AUDIO_FRAME_CACHE",
    .type = LVP_MODULE_CORE,
    .private_data_size = sizeof(LVPCache),
    .private_data = NULL,
    .module_init = init, 
    .module_close = module_close,
};

LVPModule lvp_video_frame_cache_module = {
    .version = lvp_version,
    .name = "LVP_VIDEO_FRAME_CACHE",
    .type = LVP_MODULE_CORE,
    .private_data_size = sizeof(LVPCache),
    .private_data = NULL,
    .module_init = init, 
    .module_close = module_close,
};