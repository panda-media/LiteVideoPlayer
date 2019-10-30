#include "lvp_core.h"
#include "lvp_modules.h"

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
        lvp_mem_free(core->modules);
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
    //todo parse option
    return LVP_E_FATAL_ERROR;
}

int lvp_core_set_custom_log(LVPCore *core,lvp_custom_log log,void *usr_data){
    assert(core);
    core->log->usr_data = usr_data;
    core->log->log_call = log;
    return LVP_OK;
}

static int init_core_modules(LVPCore *core){
    assert(core);
    int size = sizeof(LVPModules)/sizeof(LVPModules[0]);
    for (size_t i = 0; i < size; i++)
    {
        LVPModule *m = LVPModules[i];
        if(m->type == LVP_MODULE_CORE){
            LVPModule *tmp = (LVPModule*)lvp_mem_mallocz(sizeof(*tmp));
            if(tmp==NULL){
                return LVP_E_NO_MEM;
            }
            memcpy(tmp,m,sizeof(*tmp));
            tmp->private_data = lvp_mem_mallocz(tmp->private_data_size);
            int ret = tmp->module_init(tmp,core->options,core->event_control,core->log);
            if(ret != LVP_OK){
                lvp_error(core->log,"init module %s error return %d",tmp->name,ret);
            }
            //core 不会释放module的private
            //但是 core会在stop时调用所有module的close方法，所有module的private data 应该在此时释放
            lvp_list_add(core->modules,tmp,NULL,NULL,0);
        }
    }
    return LVP_OK;
}

int lvp_core_play(LVPCore *core){
    assert(core);
    int ret = init_core_modules(core);
    if(ret!=LVP_OK){
        return ret;
    }
    LVPSENDEVENT(core->event_control,LVP_EVENT_SET_URL,core->input_str);
    LVPEvent *ev = lvp_event_alloc(NULL,LVP_EVENT_PLAY);
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
    LVPEvent *ev = lvp_event_alloc(&core_pts,LVP_EVENT_SEEK);
    int ret = lvp_event_control_send_event(core->event_control,ev);
    lvp_event_free(ev);
    return ret;
}
