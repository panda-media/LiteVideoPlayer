#include "lvp_core.h"

LVPCore* lvp_core_alloc(){
    LVPCore *core = (LVPCore*)lvp_mem_mallocz(sizeof(*core));
    core->modules = lvp_list_alloc();
    core->options = lvp_map_alloc();
    core->event_control = lvp_event_control_alloc();
    core->log = lvp_log_alloc("LVP CORE");
    return core;
}

void lvp_core_free(LVPCore *core){
    assert(core);
    if(core->modules){
        lvp_list_free(core->modules);
    }
    if(core->options){
        lvp_map_free(core->options);
    }
    if(core->event_control){
        lvp_event_control_free(core->event_control);
    }
    if(core->log){
        lvp_log_free(core->log);
    }
    if(core->input_str){
        lvp_mem_free(core->input_str);
    }
    if(core->option_str){
        lvp_mem_free(core->option_str);
    }
	lvp_mem_free(core);
}

int lvp_core_set_url(LVPCore *core, const char *input){
    assert(core);
    assert(input);
    int ret = lvp_str_dump(input,&core->input_str);
    return ret;
}

int lvp_core_set_option(LVPCore *core, const char *options){
    assert(core);
    assert(options);
    int ret = lvp_str_dump(options,&core->option_str);
    if(ret!=LVP_OK){
        return ret;
    }
    ret = lvp_str_parse_to_options(options,core->options);
    return ret;
}

int lvp_core_set_custom_log(LVPCore *core,lvp_custom_log log,void *usr_data){
    assert(core);
    core->log->usr_data = usr_data;
    core->log->log_call = log;
    return LVP_OK;
}

static void lvp_module_custom_free(void* data, void* usr_data) {
	LVPModule* m = (LVPModule*)data;
	m->module_close(m);
	lvp_mem_free(m);
}

static int init_basic_module(LVPCore *core,const char *default_module_name,
                             const char *option_key, LVPModuleType type){
    LVPModule *module = NULL;
    const char *module_name = NULL;
    if(option_key != NULL){
        module_name = lvp_map_get(core->options,option_key);
    }
    module_name = module_name == NULL ? default_module_name : module_name;
    module = lvp_module_get_module(module_name,type);
    if(!module){
        lvp_error(NULL,"can not find module %s for type %d",module_name?module_name:default_module_name,type);
        return LVP_E_NO_MEDIA;
    }
    int ret = lvp_module_init(module,core->options,core->event_control,core->log);
    if(ret != LVP_OK){
        lvp_mem_free(module);
        return ret;
    }
    //所有module只负责释放结构体，module自己应该遵循习惯通过close自己释放private data 
    lvp_list_add(core->modules,module,NULL,lvp_module_custom_free,1);
    return LVP_OK;
}

static int init_core_modules(LVPCore *core){
    assert(core);

    //reader
    int ret = init_basic_module(core,"LVP_READER_MODULE","reader",LVP_MODULE_CORE|LVP_MODULE_READER);
    if(ret != LVP_OK){
        goto error;
    }

    //pktfilter 
    ret = init_basic_module(core,"LVP_PKT_FILTER","pkt_filter",LVP_MODULE_CORE|LVP_MODULE_PKT_FILTER);
    if(ret!=LVP_OK){
        goto error;
    }

    ret = init_basic_module(core,"LVP_FRAME_FILTER","frame_filter",LVP_MODULE_CORE|LVP_MODULE_FRAME_FILTER);

    //pkt cache
    ret = init_basic_module(core,"LVP_AUDIO_PKT_CACHE",NULL,LVP_MODULE_CORE);

    //pkt cache
    ret = init_basic_module(core,"LVP_VIDEO_PKT_CACHE",NULL,LVP_MODULE_CORE);

    ret = init_basic_module(core,"LVP_SUB_PKT_CACHE",NULL,LVP_MODULE_CORE);

    ret = init_basic_module(core,"LVP_VIDEO_FRAME_CACHE",NULL,LVP_MODULE_CORE);

    ret = init_basic_module(core,"LVP_AUDIO_FRAME_CACHE",NULL,LVP_MODULE_CORE);

    ret = init_basic_module(core,"LVP_SUB_FRAME_CACHE",NULL,LVP_MODULE_CORE);

    ret = init_basic_module(core,"LVP_AUDIO_DECODER","audio_decoder",LVP_MODULE_CORE|LVP_MODULE_DECODER);

    ret = init_basic_module(core,"LVP_VIDEO_DECODER","video_decoder",LVP_MODULE_CORE|LVP_MODULE_DECODER);

    ret = init_basic_module(core,"LVP_SUB_DECODER","sub_decoder",LVP_MODULE_CORE|LVP_MODULE_DECODER);

    ret = init_basic_module(core,"LVP_AUDIO_RENDER","audio_render",LVP_MODULE_CORE|LVP_MODULE_RENDER);

    ret = init_basic_module(core,"LVP_VIDEO_RENDER","video_render",LVP_MODULE_CORE|LVP_MODULE_RENDER);

    ret = init_basic_module(core,"LVP_AVSYNC_MODULE","video_render",LVP_MODULE_CORE);

    return LVP_OK;

    error:
        return ret;
}

int lvp_core_play(LVPCore *core){
    assert(core);
    int ret = init_core_modules(core);
    if(ret!=LVP_OK){
        return ret;
    }
    LVPSENDEVENT(core->event_control,LVP_EVENT_SET_URL,core->input_str);
    LVPEvent *ev = lvp_event_alloc(NULL,LVP_EVENT_PLAY,LVP_TRUE);
    ret = lvp_event_control_send_event(core->event_control,ev);
    lvp_event_free(ev);
    return ret;
    
}
int lvp_core_pause(LVPCore *core){
    assert(core);
    LVPSENDEVENT(core->event_control,LVP_EVENT_PAUSE,NULL);
    return LVP_OK;
}

int lvp_core_resume(LVPCore *core){
    assert(core);
    LVPSENDEVENT(core->event_control,LVP_EVENT_RESUME,NULL);
    return LVP_OK;
}

int lvp_core_stop(LVPCore *core){
    assert(core);
    LVPSENDEVENT(core->event_control,LVP_EVENT_STOP,NULL);
    return LVP_OK;
}

int lvp_core_seek(LVPCore *core,double pts){
    assert(core);
    //make pts is micro second
    int core_pts = pts*1000*1000;
    LVPEvent *ev = lvp_event_alloc(&core_pts,LVP_EVENT_SEEK,LVP_TRUE);
    int ret = lvp_event_control_send_event(core->event_control,ev);
    lvp_event_free(ev);
    return ret;
}
