#include "lvp_decoder.h"
#include <libavformat/avformat.h>

static void close_decoder(LVPDecoder *decoder){
    if(decoder->decoder_thread_run == 1){
        decoder->decoder_thread_run = 0;
        lvp_thread_join(decoder->dec_thread);
    }
    if(decoder->codec){
        decoder->codec = NULL;
    }
    if(decoder->avctx){
        avcodec_close(decoder->avctx);
        avcodec_free_context(&decoder->avctx);
        decoder->avctx = NULL;
    }
    if(decoder->iframe){
        av_frame_free(&decoder->iframe);
    }
}

static int init_audio_decoder(LVPDecoder *decoder){
    AVStream *stream = decoder->stream;
    decoder->codec = avcodec_find_decoder(stream->codecpar->codec_id);
    if(decoder->codec == NULL){
        lvp_error(decoder->log,"can not find decoder for codec id %d",stream->codecpar->codec_id);
        return LVP_E_FATAL_ERROR;
    }
    decoder->avctx = avcodec_alloc_context3(decoder->codec);
    if(decoder->avctx == NULL){
        lvp_error(decoder->log,"alloc context error",NULL);
        return LVP_E_NO_MEM;
    }
    avcodec_parameters_to_context(decoder->avctx,stream->codecpar);

    //todo set options
    int ret = avcodec_open2(decoder->avctx,decoder->codec,NULL);
    if(ret != LVP_OK){
        lvp_error(decoder->log,"open avcodec error",NULL);
        return LVP_E_FATAL_ERROR;
    }
    return LVP_OK;
}

#ifdef USE_HW

static int find_linux_decoder(LVPDecoder *decoder){
    return LVP_E_FATAL_ERROR;
}

static int find_win32_decoder(LVPDecoder *decoder){
    return LVP_E_FATAL_ERROR;
}

static int find_ios_decoder(LVPDecoder *decoder){
    return LVP_E_FATAL_ERROR;
}

static int find_android_decoder(LVPDecoder *decoder){
    return LVP_E_FATAL_ERROR;
}

#endif

//options lvpop_codec
static int find_best_video_decoder(LVPDecoder *decoder){
    if(decoder->options){
        const char *force_codec_name = lvp_map_get(decoder->options,"lvpop_codec");
        if(force_codec_name)
            decoder->codec = avcodec_find_decoder_by_name(force_codec_name);
    }

//todo use hwaccel
#if USE_HW
    //use default codec qsv -> nvdia -> dxva2/vaapi -> h264
    //if android mediacodec -> h264
    //if ios videotoolbox -> h264
    if(!decoder->codec){

#ifdef LVP_LINUX

    find_linux_decoder(decoder);

#endif

#ifdef LVP_ANDROID
#endif

#ifdef LVP_IOS
#endif

#ifdef LVP_WIN32

#endif

    }
#endif //use hwaccel

    decoder->codec = avcodec_find_decoder(decoder->stream->codecpar->codec_id);

    return LVP_OK;

}

static void* decoder_thread(void *data){
    LVPDecoder *d = (LVPDecoder*)data;
    lvp_debug(d->log,"in decoder thread type %d",d->codec_type);
    d->decoder_thread_run = 1;
    LVPEvent *ev = lvp_event_alloc(NULL,LVP_EVENT_REQ_PKT,LVP_TRUE);
    LVPEvent *sev = lvp_event_alloc(NULL,LVP_EVENT_DECODER_SEND_FRAME,LVP_TRUE);
    int need_req = 1;
    d->iframe = av_frame_alloc();
    while (d->decoder_thread_run == 1)
    {
        int ret = 0;
        if(need_req == 1){
            ev->data = &d->codec_type;
            ret = lvp_event_control_send_event(d->ctl,ev);
            if(ret==LVP_E_NO_MEM){
                lvp_sleep(10);
                continue;
            }else if(ret != LVP_OK)
            {
                lvp_error(d->log,"req pkt error",NULL);
                break;
            }
        }
        d->ipkt = (AVPacket*)ev->data;
        ret = avcodec_send_packet(d->avctx,d->ipkt);
        if(ret<0&&ret!=AVERROR(EAGAIN)){
            lvp_error(d->log,"send packet error %d",ret);
            break;
        }
        if(ret == AVERROR(EAGAIN)){
            need_req = 0;
        }else{
            need_req = 1;
        }

        ret = avcodec_receive_frame(d->avctx,d->iframe);
        if(ret<0&&ret!=AVERROR(EAGAIN)){
            lvp_error(d->log,"receive frame error %d",ret);
            break;
        }
        if(ret == 0){
            LVPSENDEVENT(d->ctl,LVP_EVENT_DECODER_GOT_FRAME,d->iframe);
            sev->data = d->iframe;
            retry:
            ret = lvp_event_control_send_event(d->ctl,sev);
            if(ret != LVP_OK && ret != LVP_E_NO_MEM){
                break;
            }
            if(ret == LVP_E_NO_MEM && d->decoder_thread_run == 1){
                lvp_sleep(1);
                goto retry;
            }
        }
    }
    
    lvp_event_free(ev);
    lvp_event_free(sev);

    lvp_debug(d->log,"out decoder thread type %d",d->codec_type);
    return NULL;
}

static int init_video_decoder(LVPDecoder *decoder){
    AVStream *stream = decoder->stream;
    int ret = find_best_video_decoder(decoder);
    if(ret != LVP_OK){
        lvp_error(decoder->log,"can not find decoder for codec id %d",stream->codecpar->codec_id);
        return LVP_E_FATAL_ERROR;
    }
    decoder->avctx = avcodec_alloc_context3(decoder->codec);
    ret = avcodec_parameters_to_context(decoder->avctx,decoder->stream->codecpar);
    if(ret<0){
        lvp_error(decoder->log,"param to context error",NULL);
        return LVP_E_FATAL_ERROR;
    }
    //todo set options
    ret = avcodec_open2(decoder->avctx,decoder->codec,NULL);
    if(ret < 0){
        lvp_error(decoder->log,"avcodec open2 error",NULL);
        return LVP_E_FATAL_ERROR;
    }
    return LVP_OK;
}

static int handle_select_stream(LVPEvent *ev, void *usrdata){
    LVPDecoder *decoder = (LVPDecoder*)usrdata;
    AVStream *stream = (AVStream*)ev->data;
    if(stream->codecpar->codec_type != decoder->codec_type){
        return LVP_OK;
    }
    close_decoder(decoder);

    decoder->stream = stream;

    int ret = 0;
    if(decoder->codec_type == AVMEDIA_TYPE_AUDIO){
        ret = init_audio_decoder(decoder);
    }else if(decoder->codec_type == AVMEDIA_TYPE_VIDEO){
        ret = init_video_decoder(decoder);
    }

    if(ret != LVP_OK){
        return ret;
    }

    ret = lvp_thread_create(&decoder->dec_thread,decoder_thread,decoder);
    if(ret != LVP_TRUE){
        lvp_error(decoder->log,"create decoder thread error",NULL);
        return LVP_E_FATAL_ERROR;
    }
    return LVP_OK;
}

static int handle_close(LVPEvent *ev, void *usrdata){
    //LVPDecoder *decoder = (LVPDecoder*)usrdata;
    return LVP_OK;
}

static int handle_open(LVPEvent *ev, void *usrdata){
    //LVPDecoder *decoder = (LVPDecoder*)usrdata;
    return LVP_OK;
}



static int module_init(struct lvp_module *module, 
                                    LVPMap *options,
                                    LVPEventControl *ctl,
                                    LVPLog *log){
    assert(module);
    assert(ctl);
    assert(log);

    LVPDecoder *decoder = (LVPDecoder*)module->private_data;
    decoder->ctl = ctl;
    decoder->log = lvp_log_alloc(module->name);
    decoder->log->log_call = log->log_call;
    decoder->log->usr_data = log->usr_data;
    decoder->options = options;

    int ret = lvp_event_control_add_listener(ctl,LVP_EVENT_SELECT_STREAM,handle_select_stream,decoder);
    if(ret != LVP_OK){
        lvp_error(decoder->log,"add %s listener error",LVP_EVENT_SELECT_STREAM);
        return LVP_E_FATAL_ERROR;
    }
    ret = lvp_event_control_add_listener(ctl,LVP_EVENT_CLOSE,handle_close,decoder);
    if(ret != LVP_OK){
        lvp_error(decoder->log,"add %s listener error",LVP_EVENT_CLOSE);
        return LVP_E_FATAL_ERROR;
    }
    ret = lvp_event_control_add_listener(ctl,LVP_EVENT_OPEN,handle_open,decoder);
    if(ret != LVP_OK){
        lvp_error(decoder->log,"add %s listener error",LVP_EVENT_CLOSE);
        return LVP_E_FATAL_ERROR;
    }

    if (!strcmp(module->name,"LVP_AUDIO_DECODER"))
    {
        decoder->codec_type = AVMEDIA_TYPE_AUDIO;
    }else if(!strcmp(module->name,"LVP_VIDEO_DECODER")){
        decoder->codec_type = AVMEDIA_TYPE_VIDEO;
    }else{
        return LVP_E_NO_MEDIA;
    }
    
    return LVP_OK;
}

static void module_close(struct lvp_module *module){
    assert(module);
    //LVPDecoder *decoder = (LVPDecoder*)module->private_data;
    return ;
}

LVPModule lvp_audio_decoder = {
    .version = lvp_version,
    .name = "LVP_AUDIO_DECODER",
    .type = LVP_MODULE_CORE|LVP_MODULE_DECODER,
    .private_data_size = sizeof(LVPDecoder),
    .private_data = NULL,
    .module_init = module_init, 
    .module_close = module_close,
};

LVPModule lvp_video_decoder = {
    .version = lvp_version,
    .name = "LVP_VIDEO_DECODER",
    .type = LVP_MODULE_CORE|LVP_MODULE_DECODER,
    .private_data_size = sizeof(LVPDecoder),
    .private_data = NULL,
    .module_init = module_init, 
    .module_close = module_close,
};