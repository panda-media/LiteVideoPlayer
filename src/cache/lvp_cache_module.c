#include "lvp_cache_module.h"


static void cache_custom_pkt_free(void *data, void *usrdata){
	if(data!=NULL)
		av_packet_free((AVPacket **)&data);
}
static void cache_custom_frame_free(void *data, void *usrdata){
    av_frame_free((AVFrame**)&data);
}

static void cache_custom_sub_free(void* data, void* usrdata) {
	avsubtitle_free((AVSubtitle*)data);
}

static int handle_pkt(LVPEvent *ev, void *usr_data){
    LVPCache *cache = (LVPCache*)usr_data;
    AVPacket *pkt = (AVPacket*)ev->data;
    if(cache->handle_stream == NULL || cache->handle_stream->index != pkt->stream_index){
        return LVP_OK;
    }
    if(cache->data->size >= cache->max_size){
        return LVP_E_NO_MEM;
    }

    AVPacket *refpkt =  av_packet_clone(ev->data);
    if(refpkt == NULL){
        lvp_error(cache->log,"clone packet error",NULL);
        return LVP_E_FATAL_ERROR;
    }

	lvp_mutex_lock(&cache->mutex);
    int ret = lvp_nqueue_push(cache->data,refpkt,NULL,cache_custom_pkt_free,LVP_TRUE);
	lvp_mutex_unlock(&cache->mutex);
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
	lvp_mutex_lock(&cache->mutex);
    if(cache->data->size ==0 ){
       // printf("CACHE FULL\n");
		lvp_mutex_unlock(&cache->mutex);
        return LVP_E_NO_MEM;
    }
    AVPacket *p = NULL;
	p = (AVPacket*)lvp_nqueue_pop(cache->data);
    if(p == NULL){
		lvp_error(cache->log, "cache queue error", NULL);
		lvp_mutex_unlock(&cache->mutex);
        return LVP_E_FATAL_ERROR;
    }
    if(p->pts<0){
	//	lvp_error(cache->log, "cache queue error", NULL);
	//	lvp_mutex_unlock(&cache->mutex);
    //   return LVP_E_FATAL_ERROR;
    }
    AVPacket *ref = av_packet_clone(p);
	lvp_mutex_unlock(&cache->mutex);
    ev->data = ref;
    av_packet_free(&p);
    return LVP_OK;
}


static int handle_frame(LVPEvent *ev, void *usr_data){
    LVPCache *cache = (LVPCache*)usr_data;

	AVFrame* f = (AVFrame*)ev->data;
	int type = f->width > 0 ? AVMEDIA_TYPE_VIDEO : AVMEDIA_TYPE_AUDIO;
	if (cache->media_type != type) {
		return LVP_OK;
	}

    //this frame no audio sample we don't need cache it
    if(type == AVMEDIA_TYPE_AUDIO && f->nb_samples == 0){
      //  printf("NCACHE\n");
        return LVP_OK;
    }

    if(cache->data->size >= cache->max_size){
        return LVP_E_NO_MEM;
    }

    AVFrame *refframe = av_frame_clone(f);
    if(refframe== NULL){
        lvp_error(cache->log,"clone frame error",NULL);
        return LVP_E_FATAL_ERROR;
    }
	lvp_mutex_lock(&cache->mutex);
    int ret = lvp_nqueue_push(cache->data,refframe,NULL,cache_custom_frame_free,1);
	lvp_mutex_unlock(&cache->mutex);
    if(ret == LVP_FALSE){
        return LVP_E_FATAL_ERROR;
    }

    return LVP_OK;
}

static int handle_sub(LVPEvent* ev, void* usrdata) {
	LVPCache* cache = (LVPCache*)usrdata;
	AVSubtitle* sub = (AVSubtitle*)ev->data;
	if (cache->data->size >= cache->max_size) {
		return LVP_E_NO_MEM;
	}

	lvp_mutex_lock(&cache->mutex);
	int ret = lvp_nqueue_push(cache->data, sub, NULL,cache_custom_sub_free,1);
	lvp_mutex_unlock(&cache->mutex);
	if (ret == LVP_FALSE) {
		return LVP_E_FATAL_ERROR;
	}
	return LVP_OK;
}

static int handle_req_sub(LVPEvent* ev, void* usrdata) {
	LVPCache* cache = (LVPCache*)usrdata;
	lvp_mutex_lock(&cache->mutex);
	if (cache->data->size == 0)
	{
		lvp_mutex_unlock(&cache->mutex);
		return LVP_E_NO_MEM;
	}
	AVSubtitle* sub = NULL;
	sub = (AVSubtitle*)lvp_nqueue_pop(cache->data);
	lvp_mutex_unlock(&cache->mutex);
	if (sub == NULL) {
		return LVP_E_NO_MEM;
	}
	ev->data = sub;
	return LVP_OK;
}

static int handle_req_frame(LVPEvent *ev, void *usrdata){
    LVPCache *cache = (LVPCache*)usrdata;
    int type = *(int*)ev->data;
    if(type != cache->media_type){
        return LVP_OK;
    }
	lvp_mutex_lock(&cache->mutex);
    if(cache->data->size == 0){
		lvp_mutex_unlock(&cache->mutex);
        return LVP_E_NO_MEM;
    }
    AVFrame *f = NULL;
	f = (AVFrame*)lvp_nqueue_pop(cache->data);
	if (f == NULL)
	{
		lvp_mutex_unlock(&cache->mutex);
		return LVP_E_NO_MEM;
	}
    AVFrame *ref = av_frame_clone(f);
	lvp_mutex_unlock(&cache->mutex);
    ev->data = ref;
    av_frame_free(&f);
    return LVP_OK;
}

static int init_pkt_cache(LVPCache *cache){
    int ret = lvp_event_control_add_listener(cache->ctl,LVP_EVENT_FILTER_SEND_PKT,handle_pkt,cache);
    if(ret!=LVP_OK){
        lvp_error(cache->log,"listener %s error",LVP_EVENT_FILTER_SEND_PKT);
		return ret;
    }
    ret = lvp_event_control_add_listener(cache->ctl,LVP_EVENT_REQ_PKT,handle_req_pkt,cache);
    if(ret != LVP_OK){
        lvp_error(cache->log,"listener %s error",LVP_EVENT_REQ_PKT);
		return ret;
    }
    return LVP_OK;
}

static int init_frame_cache(LVPCache *cache){
    int ret = lvp_event_control_add_listener(cache->ctl,LVP_EVENT_FILTER_SEND_FRAME,handle_frame,cache);
    if(ret != LVP_OK){
        lvp_error(cache->log,"listener %s error",LVP_EVENT_FILTER_SEND_FRAME);
		return ret;
    }
    ret = lvp_event_control_add_listener(cache->ctl,LVP_EVENT_REQ_FRAME,handle_req_frame,cache);
    if(ret != LVP_OK){
        lvp_error(cache->log,"listener %s error",LVP_EVENT_REQ_FRAME);
		return ret;
    }
    return LVP_OK;
}

static int init_sub_cache(LVPCache* cache) {
	int ret = lvp_event_control_add_listener(cache->ctl, LVP_EVENT_DECODER_SEND_SUB, handle_sub, cache);
	if (ret != LVP_OK) {
		lvp_error(cache->log, "listener %s error", LVP_EVENT_DECODER_SEND_SUB);
		return ret;
	}
	ret = lvp_event_control_add_listener(cache->ctl, LVP_EVENT_REQ_SUB, handle_req_sub, cache);
	if (ret != LVP_OK) {
		lvp_error(cache->log, "listener %s error", LVP_EVENT_REQ_SUB);
		return ret;
	}
	return LVP_OK;
}


static int handle_seek(LVPEvent *ev, void *usrdata){
    LVPCache *cache = (LVPCache*)usrdata;
	lvp_mutex_lock(&cache->mutex);
    lvp_nqueue_clear(cache->data);
	lvp_mutex_unlock(&cache->mutex);
    return LVP_OK;
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
	LVP_BOOL tf = lvp_mutex_create(&cache->mutex);
	if (tf != LVP_TRUE)
	{
		lvp_error(NULL, "create mutex error", NULL);
		return LVP_E_FATAL_ERROR;
	}
    if(!strcmp(module->name,"LVP_AUDIO_PKT_CACHE")){
        cache->type = CACHE_TYPE_PKT;
        cache->media_type = AVMEDIA_TYPE_AUDIO;
    }
    else if(!strcmp(module->name,"LVP_VIDEO_PKT_CACHE")){
        cache->type = CACHE_TYPE_PKT;
        cache->media_type = AVMEDIA_TYPE_VIDEO;
	}
	else if(!strcmp(module->name,"LVP_SUB_PKT_CACHE")){
        cache->type = CACHE_TYPE_PKT;
		cache->media_type = AVMEDIA_TYPE_SUBTITLE;
	}
    else if(!strcmp(module->name,"LVP_AUDIO_FRAME_CACHE")){
        cache->type = CACHE_TYPE_FRAME;
        cache->media_type = AVMEDIA_TYPE_AUDIO;
    }
    else if(!strcmp(module->name,"LVP_VIDEO_FRAME_CACHE")){
        cache->type = CACHE_TYPE_FRAME;
        cache->media_type = AVMEDIA_TYPE_VIDEO;
	}
	else if (!strcmp(module->name, "LVP_SUB_FRAME_CACHE")) {
        cache->type = CACHE_TYPE_SUBTITLE;
        cache->media_type = AVMEDIA_TYPE_VIDEO;
	}
    cache->log = lvp_log_alloc(module->name);
    cache->log->log_call = log->log_call;
    cache->log->usr_data = log->usr_data;
    cache->ctl = ctl;

    int ret = LVP_OK;
    if(cache->type == CACHE_TYPE_FRAME){
        cache->max_size = FRAME_SIZE;
        ret = init_frame_cache(cache);
    }
    else if(cache->type == CACHE_TYPE_PKT){
        cache->max_size = PKT_SIZE;
        ret = init_pkt_cache(cache);
	}
	else if (cache->type == CACHE_TYPE_SUBTITLE) {
        cache->max_size = PKT_SIZE;
		ret = init_sub_cache(cache);
	}

	cache->data = lvp_nqueue_alloc(cache->max_size);

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
	lvp_event_control_remove_listener(cache->ctl, LVP_EVENT_REQ_FRAME, handle_req_frame, cache);
	lvp_event_control_remove_listener(cache->ctl, LVP_EVENT_REQ_PKT, handle_req_pkt, cache);
	lvp_event_control_remove_listener(cache->ctl, LVP_EVENT_REQ_SUB, handle_req_sub, cache);;
	lvp_event_control_remove_listener(cache->ctl, LVP_EVENT_FILTER_SEND_FRAME, handle_frame, cache);
	lvp_event_control_remove_listener(cache->ctl, LVP_EVENT_FILTER_SEND_PKT, handle_pkt, cache);
	lvp_event_control_remove_listener(cache->ctl, LVP_EVENT_DECODER_SEND_SUB, handle_sub, cache);
    lvp_mutex_lock(&cache->mutex);
    if(cache->data){
        lvp_nqueue_free(cache->data);
    }
	if (cache->log) {
		lvp_log_free(cache->log);
	}
    lvp_mutex_unlock(&cache->mutex);
	lvp_mutex_free(&cache->mutex);
	lvp_mem_free(cache);
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

LVPModule lvp_sub_pkt_cache_module = {
    .version = lvp_version,
    .name = "LVP_SUB_PKT_CACHE",
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

LVPModule lvp_sub_frame_cache_module = {
    .version = lvp_version,
    .name = "LVP_SUB_FRAME_CACHE",
    .type = LVP_MODULE_CORE,
    .private_data_size = sizeof(LVPCache),
    .private_data = NULL,
    .module_init = init, 
    .module_close = module_close,
};