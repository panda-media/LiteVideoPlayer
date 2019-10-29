#include "lvp_reader_module.h"

static int ff_interrupt_call(void *data){
   LVPReaderModule *m = (LVPReaderModule*)data;
   if(m->is_interrupt){
       return AVERROR_EXIT;
   }
   return 0;
}

static void reader_thread(void *data){
    LVPReaderModule *m = (LVPReaderModule*)data;
    AVFormatContext *fmt = m->avctx = avformat_alloc_context();

    lvp_debug(m->log,"in reader thread",NULL);

    fmt->interrupt_callback.opaque = m;
    fmt->interrupt_callback.callback = ff_interrupt_call;

    //todo use set option
    int ret = avformat_open_input(&fmt,m->input_url,NULL,NULL);
    if(ret<0){
        LVPSENDEVENT(m->ctl,LVP_EVENT_OPEN_ERROR,NULL);
        lvp_error(m->log,"avformat open input return %d",ret);
        return ;
    }
    m->astream = av_find_best_stream(fmt,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);
    m->vstream = av_find_best_stream(fmt,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);
    m->sub_stream = av_find_best_stream(fmt,AVMEDIA_TYPE_SUBTITLE,-1,-1,NULL,0);
    if(m->astream){
        LVPSENDEVENT(m->ctl,LVP_EVENT_SELECT_STREAM,m->astream->codecpar);
    }
    if(m->vstream){
        LVPSENDEVENT(m->ctl,LVP_EVENT_SELECT_STREAM,m->vstream->codecpar);
    }
    if(m->sub_stream){
        LVPSENDEVENT(m->ctl,LVP_EVENT_SELECT_STREAM,m->sub_stream->codecpar);
    }

    m->is_reader_thread_run = LVP_TRUE;
    int ret = 0;
    AVPacket ipkt;
    LVP_BOOL need_read = LVP_TRUE;
    while (m->is_reader_thread_run)
    {
        if (need_read)
        {
            av_packet_unref(&ipkt);
            ret = av_read_frame(fmt,&ipkt);
            need_read = LVP_FALSE;
        }

        //not select stream
        if(ipkt.stream_index != m->astream->index &&
            ipkt.stream_index != m->vstream->index &&
            ipkt.stream_index != m->sub_stream->index){
            need_read = LVP_TRUE;
            continue;
        }
        
        if(ret<0){
            if(ret == AVERROR_EOF){
                LVPSENDEVENT(m->ctl,LVP_EVENT_READER_EOF,NULL);
            }else{
                LVPSENDEVENT(m->ctl,LVP_EVENT_READER_ERROR,NULL);
            }
            m->is_reader_thread_run = LVP_FALSE;
        }
        LVPEvent *ev = lvp_event_alloc(&ipkt,LVP_EVENT_READER_GOT_FRAME);
        int e_ret = lvp_event_control_send_event(m->ctl,ev);
        need_read = e_ret == LVP_OK?LVP_TRUE:LVP_FALSE;
        lvp_event_free(ev);
    }

    lvp_debug(m->log,"out reader thread",NULL);
}

static int handle_open(LVPEvent *ev,void *usr_data){
   LVPReaderModule *m = (LVPReaderModule*)usr_data;
   if(m->input_url){
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
    return LVP_OK;
}

static int handle_seek(LVPEvent *ev, void *usr_data){
}

static int handle_change_stream(LVPEvent *ev, void *usr_data){
}


static int module_init(struct lvp_module *module, 
                                    LVPMap *options,
                                    LVPEventControl *ctl,
                                    lvp_custom_log clog){
   assert(module);
   assert(options); 
   assert(ctl);
   //get reader mem
   LVPReaderModule *reader = (LVPReaderModule*)module->private_data;
   //set event control
   reader->ctl = ctl;
   //set log
   reader->log = lvp_log_alloc((const char*)module->name);
   reader->log->log_call = clog;
   
   int ret = lvp_event_control_add_listener(ctl,LVP_EVENT_SET_URL,handle_set_url,reader);
   if(ret){
       lvp_error(reader->log,"add handler %s error",LVP_EVENT_SET_URL);
       return ret;
   }

   ret = lvp_event_control_add_listener(ctl,LVP_EVENT_OPEN,handle_open,reader);
   if(ret){
       lvp_error(reader->log,"add handler %s error",LVP_EVENT_OPEN);
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
}

LVPModule lvp_reader_module = {
    .version = lvp_version,
    .name = "LVP_READER_MODULE",
    .type = LVP_MODULE_CORE,
    .private_data_size = sizeof(LVPReaderModule),
    .private_data = NULL,
    .module_init = module_init,
    .module_close = module_close,
};