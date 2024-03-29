#include "lvp_reader_module.h"

static int ff_interrupt_call(void *data){
   LVPReaderModule *m = (LVPReaderModule*)data;
   if(m->is_interrupt){
       return AVERROR_EXIT;
   }
   return 0;
}

static void* reader_thread(void *data){
    LVPReaderModule *m = (LVPReaderModule*)data;
    m->is_reader_thread_run = LVP_TRUE;
    m->avctx = avformat_alloc_context();
    AVFormatContext *fmt = m->avctx;
    lvp_debug(m->log,"in reader thread",NULL);

    fmt->interrupt_callback.opaque = m;
    fmt->interrupt_callback.callback = ff_interrupt_call;

    //todo set option
    lvp_mutex_lock(&m->avctx_mutex);
    int ret = avformat_open_input(&m->avctx,m->input_url,NULL,NULL);
    lvp_mutex_unlock(&m->avctx_mutex);
    if(ret<0){
        LVPSENDEVENT(m->ctl,LVP_EVENT_OPEN_ERROR,NULL);
        lvp_error(m->log,"avformat open input return %d",ret);
		goto rerror;
    }
	ret = avformat_find_stream_info(fmt, NULL);
	if (ret < 0) {
		goto rerror;
	}
    lvp_mutex_lock(&m->avctx_mutex);
    int best_index = av_find_best_stream(fmt,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);
    if(best_index>=0)
        m->astream = fmt->streams[best_index];
    best_index = -1;
    best_index = av_find_best_stream(fmt,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);
    if(best_index>=0)
        m->vstream = fmt->streams[best_index];
    best_index = -1;
    best_index = av_find_best_stream(fmt,AVMEDIA_TYPE_SUBTITLE,-1,-1,NULL,0);
    if(best_index>=0)
        m->sub_stream = fmt->streams[best_index];
    
    lvp_mutex_unlock(&m->avctx_mutex);
    if(m->astream){
        LVPSENDEVENT(m->ctl,LVP_EVENT_SELECT_STREAM,m->astream);
    }
    if(m->vstream){
        LVPSENDEVENT(m->ctl,LVP_EVENT_SELECT_STREAM,m->vstream);
    }
    if(m->sub_stream){
        LVPSENDEVENT(m->ctl,LVP_EVENT_SELECT_STREAM,m->sub_stream);
    }

    ret = 0;
    AVPacket *ipkt = av_packet_alloc();
    LVP_BOOL need_read = LVP_TRUE;

	//for submodule use
	LVPEvent* sub_ev = lvp_event_alloc(ipkt, LVP_EVENT_READER_GOT_FRAME, LVP_FALSE);

    //for other core module use
    LVPEvent *ev = lvp_event_alloc(ipkt,LVP_EVENT_READER_SEND_FRAME,LVP_TRUE);
    while (m->is_reader_thread_run)
    {
        if (need_read)
        {
            lvp_mutex_lock(&m->avctx_mutex);
            ret = av_read_frame(fmt,ipkt);
            lvp_mutex_unlock(&m->avctx_mutex);
			ipkt->pts = av_q2d(fmt->streams[ipkt->stream_index]->time_base) * ipkt->pts*1000;
			ipkt->duration= av_q2d(fmt->streams[ipkt->stream_index]->time_base) * ipkt->duration*1000;
            need_read = LVP_FALSE;
        }

        //not select stream
		int select_stream = 0;
		if (m->astream && ipkt->stream_index == m->astream->index) {
			select_stream = 1;
		}
		if (m->vstream && ipkt->stream_index == m->vstream->index) {
			select_stream = 1;
		}
		if (m->sub_stream && ipkt->stream_index == m->sub_stream->index) {
			select_stream = 1;
		}
		if (select_stream == 0) {
            need_read = LVP_TRUE;
			continue;
		}
        
        if(ret<0){
            if(ret == AVERROR_EOF){
                LVPSENDEVENT(m->ctl,LVP_EVENT_READER_EOF,NULL);
                printf("EOF\n");
            }else{
                LVPSENDEVENT(m->ctl,LVP_EVENT_READER_ERROR,NULL);
            }
            m->is_reader_thread_run = LVP_FALSE;
        }

        //LVPSENDEVENT(m->ctl,LVP_EVENT_READER_GOT_FRAME,&ipkt);
		lvp_event_control_send_event(m->ctl, sub_ev);

		int e_ret = lvp_event_control_send_event(m->ctl, ev);

        //need_read = e_ret == LVP_OK?LVP_TRUE:LVP_FALSE;
		if (e_ret == LVP_OK) {
			av_packet_unref(ipkt);
			need_read = LVP_TRUE;
		}
		else {
			need_read = LVP_FALSE;
			lvp_sleep(30);
		}

    }

    av_packet_unref(ipkt);
	av_packet_free(&ipkt);
    lvp_event_free(ev);
	lvp_event_free(sub_ev);

	rerror:
    lvp_debug(m->log,"out reader thread",NULL);
    return NULL;
}

static int handle_play(LVPEvent *ev,void *usr_data){
   LVPReaderModule *m = (LVPReaderModule*)usr_data;
   if(!m->input_url){
       lvp_error(m->log,"need input",NULL);
       return LVP_E_NO_MEDIA;
   }
   LVP_BOOL ret = lvp_thread_create(&m->reader_thread,reader_thread,m);
   if(ret == LVP_FALSE){
       lvp_error(m->log,"create thread error",NULL);
       return ret;
   }
   return ret?LVP_OK:LVP_E_FATAL_ERROR;
}

static int handle_set_url(LVPEvent *ev, void *usr_data){
    LVPReaderModule *m = (LVPReaderModule*)usr_data;
    int ret = lvp_str_dump((const char *)ev->data,&m->input_url);
    return ret;
}

static int handle_stop(LVPEvent *ev, void *usr_data){
    LVPReaderModule *m = (LVPReaderModule*)usr_data;
    m->is_interrupt = LVP_TRUE;
    m->is_reader_thread_run = LVP_FALSE;
    //make sure thread created
    if(m->avctx){
        lvp_thread_join(m->reader_thread);
        m->reader_thread = 0;
    }
    return LVP_OK;
}

static int handle_seek(LVPEvent *ev, void *usr_data){
    LVPReaderModule *m = (LVPReaderModule*)usr_data;

    if(m->avctx){
        int64_t time = *(int64_t*)ev->data;
        int64_t min_time = time - 1000000;
        int64_t max_time = time + 1000000;
        int ret = LVP_OK;
        lvp_mutex_lock(&m->avctx_mutex);
        ret = avformat_seek_file(m->avctx,-1,min_time,time,max_time,AVSEEK_FLAG_FRAME);
        lvp_mutex_unlock(&m->avctx_mutex);
        return ret;
    }else{
        lvp_error(m->log,"avctx not create",NULL);
        return LVP_E_USE_NULL;
    }
}

static int handle_change_stream(LVPEvent *ev, void *usr_data){
    LVPReaderModule *m = (LVPReaderModule*)usr_data;

    int select_index = *(int*)ev->data;
    if(m->avctx->nb_streams<=select_index || select_index < 0){
        lvp_waring(m->log,"select index error, the media only %d stream",m->avctx->nb_streams);
        return LVP_E_NO_MEDIA;
    }
    AVStream *select = m->avctx->streams[select_index];
    if(select->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        m->astream = select;
    else if(select->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        m->vstream = select;
    else if(select->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE)
        m->sub_stream = select;
    else
        return LVP_E_NO_MEDIA;
    
    LVPSENDEVENT(m->ctl,LVP_EVENT_SELECT_STREAM,select->codecpar);
    return LVP_OK;
}


static int module_init(struct lvp_module *module, 
                                    LVPMap *options,
                                    LVPEventControl *ctl,
                                    LVPLog *log){
   assert(module);
   assert(options); 
   assert(ctl);
   //get reader mem
   LVPReaderModule *reader = (LVPReaderModule*)module->private_data;
   //set event control
   reader->ctl = ctl;
   //set log
   reader->log = lvp_log_alloc((const char*)module->name);
   reader->log->log_call = log->log_call;
   reader->log->usr_data = log->usr_data;

   int ret = lvp_mutex_create(&reader->avctx_mutex);

   if(ret != LVP_TRUE){
       lvp_error(reader->log,"create mutex error",NULL);
   }
   
   ret = lvp_event_control_add_listener(ctl,LVP_EVENT_SET_URL,handle_set_url,reader);
   if(ret){
       lvp_error(reader->log,"add handler %s error",LVP_EVENT_SET_URL);
       return ret;
   }

   ret = lvp_event_control_add_listener(ctl,LVP_EVENT_PLAY,handle_play,reader);
   if(ret){
       lvp_error(reader->log,"add handler %s error",LVP_EVENT_PLAY);
       return ret;
   }

   ret = lvp_event_control_add_listener(ctl,LVP_EVENT_SEEK,handle_seek,reader);
   if(ret){
       lvp_error(reader->log,"add handler %s error",LVP_EVENT_SEEK);
       return ret;
   }

   ret = lvp_event_control_add_listener(ctl,LVP_EVENT_STOP,handle_stop,reader);
   if(ret){
       lvp_error(reader->log,"add handler %s error",LVP_EVENT_STOP);
       return ret;
   }

   ret = lvp_event_control_add_listener(ctl,LVP_EVENT_CHANGE_STREAM,handle_change_stream,reader);
   if(ret){
       lvp_error(reader->log,"add handler %s error",LVP_EVENT_CHANGE_STREAM);
       return ret;
   }

   lvp_debug(reader->log,"init %s done",module->name);

   return ret;
}

static void module_close(struct lvp_module *module){
    LVPReaderModule *m = (LVPReaderModule*)module->private_data;
    m->is_interrupt = LVP_TRUE;

    lvp_event_control_remove_listener(m->ctl,LVP_EVENT_PLAY,handle_play,m);
    lvp_event_control_remove_listener(m->ctl,LVP_EVENT_SET_URL,handle_set_url,m);
    lvp_event_control_remove_listener(m->ctl,LVP_EVENT_SEEK,handle_seek,m);
    lvp_event_control_remove_listener(m->ctl,LVP_EVENT_STOP,handle_stop,m);
    lvp_event_control_remove_listener(m->ctl,LVP_EVENT_CHANGE_STREAM,handle_change_stream,m);

    if(m->reader_thread!=0){
		m->is_reader_thread_run = 0;
        lvp_thread_join(m->reader_thread);
        m->reader_thread = 0;
    }
    if(m->avctx){
        lvp_mutex_lock(&m->avctx_mutex);
        avformat_close_input(&m->avctx);
        lvp_mutex_unlock(&m->avctx_mutex);
        lvp_mutex_free(&m->avctx_mutex);
    }
    if(m->input_url){
        lvp_mem_free(m->input_url);
    }
    if(m->log){
        lvp_log_free(m->log);
    }
	lvp_mem_free(m);
}

LVPModule lvp_reader_module = {
    .version = lvp_version,
    .name = "LVP_READER_MODULE",
    .type = LVP_MODULE_CORE|LVP_MODULE_READER,
    .private_data_size = sizeof(LVPReaderModule),
    .private_data = NULL,
    .module_init = module_init,
    .module_close = module_close,
};